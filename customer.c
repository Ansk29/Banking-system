#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/file.h>
#include"customer.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_CUSTOMERS 100
#define MAX_LENGTH 100


Customer customers[MAX_CUSTOMERS]; // Array store karne ko
int customerCount = 0; // add wale mei use eaayega 




// Function to load customers from the text file
/*jo data file mei se nikal ke structure ke array mein store kar liya he */
void loadCustomers() {
    FILE *file = fopen("customers.txt", "r");
    if (!file) {
        printf("Error: Unable to open customers database.\n");
        return;
    }

    while (fscanf(file, "%d,%99[^,],%99[^,],%f\n", 
                  &customers[customerCount].id, 
                  customers[customerCount].username, 
                  customers[customerCount].password, 
                  &customers[customerCount].balance) == 4) {
        customerCount++;
    }
    
    fclose(file);
}

// Function to save customers to the text file

/* save kar do details ek for loop lagakar hamare isme fprintf sara data*/
void saveCustomers() {
    FILE *file = fopen("customers.txt", "w");
    if (!file) {
        printf("Error: Unable to open customers database for writing.\n");
        return;
    }

    for (int i = 0; i < customerCount; i++) {
        fprintf(file, "%d,%s,%s,%.2f\n", customers[i].id, customers[i].username, customers[i].password,
                customers[i].balance);
    }
    fclose(file);
}

// Function to validate customer login

// Function to validate customer login

/*buffer banao 
variable banao 
msg send karo olgin id 
read karo 

password send karo
read karo
store karo

for loop lagao har se check karo 
a)login id = username
b)password = password

mila koi index aisa toh send karo success 
varna failure message send kar do;

 */
int customer_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char login_username[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};

    // Ask for customer login username
    char *login_message = "Enter Customer Username: ";
    send(client_socket, login_message, strlen(login_message), 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(login_username, buffer);

    // Ask for customer password
    char *password_message = "Enter Customer Password: ";
    send(client_socket, password_message, strlen(password_message), 0);
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(password, buffer);

    // Validate credentials
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].username, login_username) == 0 && 
            strcmp(customers[i].password, password) == 0) {
            send(client_socket, "Login successful. Welcome Customer!\n", 36, 0);
            return i;  // Return the index of the logged-in customer
        }
    }

    send(client_socket, "Invalid username or password.\n", 31, 0);
    return -1;  // Login failure
}

// Function to handle fund transfer

// Function to handle fund transfer
/*Fund transfer karna he  
variables banao required 
send karo prompt recipient ka id
read karo data 

ab check karna hoga ki recipient he ki nahi 
agar nahi he toh return kar do bhai not found 
agar he toh 
waps prompt send karo ki kitna amount transfer 
read karo response ko 

amount ko integer mei convert 
checks lagao 
customer ke index mein se fund remove kar do aur reciver ke amount mein add kar do
save kar do haamra update*/


void transfer_funds(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    int recipient_id;
    float transfer_amount;

    // Get recipient customer ID
    send(client_socket, "Enter recipient customer ID: ", 29, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    recipient_id = atoi(buffer);

    // Open the customer file for locking
    FILE *file = fopen("customers.txt", "r+"); // Open in read/write mode
    if (!file) {
        send(client_socket, "Error accessing customer data.\n", 31, 0);
        return;
    }

    // Apply exclusive lock to the file
    if (flock(fileno(file), LOCK_EX) != 0) {
        send(client_socket, "Unable to lock file.\n", 22, 0);
        fclose(file);
        return;
    }

    // Find the recipient customer
    int recipientIndex = -1;
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id == recipient_id) {
            recipientIndex = i;
            break;
        }
    }

    if (recipientIndex == -1) {
        send(client_socket, "Invalid customer ID.\n", 21, 0);
        // Unlock the file before returning
        flock(fileno(file), LOCK_UN);
        fclose(file);
        return;
    }

    // Get transfer amount
    send(client_socket, "Enter transfer amount: ", 23, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    transfer_amount = atof(buffer);

    if (transfer_amount > customers[customerIndex].balance) {
        send(client_socket, "Insufficient balance for transfer.\n", 35, 0);
    } else {
        // Perform the transfer
        customers[customerIndex].balance -= transfer_amount;
        customers[recipientIndex].balance += transfer_amount;

        // Inform the customer of the successful transfer
        sprintf(buffer, "Transfer successful! $%.2f has been sent to customer ID %d.\n", transfer_amount, recipient_id);
        send(client_socket, buffer, strlen(buffer), 0);

        // Save updated customer data to the file
        saveCustomers(); // Assuming this writes to CUSTOMER_FILE
    }

    // Unlock the file after the transfer
    flock(fileno(file), LOCK_UN);
    fclose(file);
}

// Function to apply for a loan

void apply_loan(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    float loan_amount;
    int loanID = 0; // Initialize loanID
    FILE *file;

    // Get loan amount from the customer
    send(client_socket, "Enter loan amount: ", 19, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    loan_amount = atof(buffer);

    if (loan_amount <= 0) {
        send(client_socket, "Invalid loan amount. Must be greater than zero.\n", 49, 0);
        return;
    }

    // Generate a unique loan ID (based on the existing loan_request.txt file)
    file = fopen("loan_request.txt", "r");
    if (file) {
        LoanRequest tempRequest;
        while (fscanf(file, "%d,%d,%f,%s", &tempRequest.loanID, &tempRequest.customerId, &tempRequest.loanAmount, tempRequest.status) != EOF) {
            if (tempRequest.loanID > loanID) {
                loanID = tempRequest.loanID; // Keep track of the highest loan ID
            }
        }
        fclose(file);
    }
    loanID++; // Increment for the new loan ID

    // Create a new loan request
    LoanRequest newRequest;
    newRequest.loanID = loanID; // Assign the generated loanID
    newRequest.customerId = customers[customerIndex].id; // Associate loan with the customer
    newRequest.loanAmount = loan_amount;
    strcpy(newRequest.status, "Pending"); // Default status is "Pending"

    // Append the loan request to loan_request.txt
    file = fopen("loan_request.txt", "a");
    if (!file) {
        send(client_socket, "Error: Unable to submit loan request.\n", 38, 0);
        return;
    }

    // Write loan request to the file
    fprintf(file, "%d,%d,%.2f,%s\n", newRequest.loanID, newRequest.customerId, newRequest.loanAmount, newRequest.status);
    fclose(file);

    // Inform the customer about the successful loan application
    sprintf(buffer, "Loan application for $%.2f has been submitted with Loan ID: %d.\n", loan_amount, loanID);
    send(client_socket, buffer, strlen(buffer), 0);
}


// Function to change password
/* 
buffer banaya ;
do variable banana 
old password ka prompt send karo
read karo user ka data 
usko string mei copy karo ek 
ab is string ko compare karenge purane password se agr match ni hua toh incorrecr
if matched 
send karo promt new password ka
read karo user ka input 

usko buffer mei se newpass wale string mei copy kar lo ;
ab is new password ko humko file mei add karna he 
hamara jo structure banaay he usko access kar ke;
*/
void change_password(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    char old_password[MAX_BUFFER_SIZE];
    char new_password[MAX_BUFFER_SIZE];

    // Ask for old password
    send(client_socket, "Enter your old password: ", 25, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(old_password, buffer);

    // Validate the old password
    if (strcmp(customers[customerIndex].password, old_password) != 0) {
        send(client_socket, "Incorrect old password.\n", 25, 0);
        return;
    }

    // Ask for new password
    send(client_socket, "Enter your new password: ", 25, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(new_password, buffer);

    // Update the password
    strcpy(customers[customerIndex].password, new_password);
    saveCustomers();
    send(client_socket, "Password successfully changed.\n", 31, 0);
}

// Function to add feedback

/* buffer banaya 
    client ko ek prompt send karvaya
    read kara client ka response 
    file open karna hoga feedback ka append mode
    us feedback ko fprintf kar dena 
    file ko close kar dena
    
    ek end mei prompt send kar dena ki send ho gaya */
void add_feedback(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    
    // Ask for feedback
    send(client_socket, "Enter your feedback: ", 21, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);

    // Save feedback to a file
    FILE *file = fopen("feedback.txt", "a");
    if (!file) {
        send(client_socket, "Error: Unable to submit feedback.\n", 34, 0);
        return;
    }

    fprintf(file, "Customer ID: %d, Feedback: %s\n", customers[customerIndex].id, buffer);
    fclose(file);
    send(client_socket, "Thank you for your feedback!\n", 30, 0);
}

// Function to view transaction history
void view_transaction_history(int client_socket, int customerIndex) {
    send(client_socket, "Displaying transaction history is under development.\n", 55, 0);
}

// Function to display customer menu and handle customer functionalities

/*socket and index as the jo identify karvata he us user ko 
while(1) logout nhi karna jb tak vo chahta nahi
customer menu banaya aur send kara 
read kara client resposne 
convert karo int mei
switch case lagao 
us function ko call karo */
void customer_menu(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    int option;

    while (1) {
        char *customer_menu =
            "Customer Menu:\n"
            "1. View Account Balance\n"
            "2. Deposit Money\n"
            "3. Withdraw Money\n"
            "4. Transfer Funds\n"
            "5. Apply for a Loan\n"
            "6. Change Password\n"
            "7. Add Feedback\n"
            "8. View Transaction History\n"
            "9. Logout\n"
            "10. Exit\n"
            "Choose an option: ";
        
        send(client_socket, customer_menu, strlen(customer_menu), 0);
        read(client_socket, buffer, MAX_BUFFER_SIZE);
        option = atoi(buffer);

        switch (option) {
            case 1:
                sprintf(buffer, "Your current account balance is: $%.2f\n", customers[customerIndex].balance);
                send(client_socket, buffer, strlen(buffer), 0);
                break;
            case 2: {
                float deposit_amount;
                send(client_socket, "Enter amount to deposit: ", 25, 0);
                read(client_socket, buffer, MAX_BUFFER_SIZE);
                deposit_amount = atof(buffer);
                customers[customerIndex].balance += deposit_amount;
                sprintf(buffer, "You have successfully deposited: $%.2f\n", deposit_amount);
                send(client_socket, buffer, strlen(buffer), 0);
                saveCustomers();  
                break;
            }
            case 3: {
                float withdraw_amount;
                send(client_socket, "Enter amount to withdraw: ", 26, 0);
                read(client_socket, buffer, MAX_BUFFER_SIZE);
                withdraw_amount = atof(buffer);
                if (withdraw_amount > customers[customerIndex].balance) {
                    send(client_socket, "Insufficient balance for this withdrawal.\n", 43, 0);
                } else {
                    customers[customerIndex].balance -= withdraw_amount;
                    sprintf(buffer, "You have successfully withdrawn: $%.2f\n", withdraw_amount);
                    send(client_socket, buffer, strlen(buffer), 0);
                    saveCustomers();
                }
                break;
            }
            case 4:
                transfer_funds(client_socket, customerIndex);
                break;
            case 5:
                apply_loan(client_socket, customerIndex);
                break;
            case 6:
                change_password(client_socket, customerIndex);
                break;
            case 7:
                add_feedback(client_socket, customerIndex);
                break;
            case 8:
                view_transaction_history(client_socket, customerIndex);
                break;
            case 9:
                send(client_socket, "Logging out...\n", 15, 0);
                return;
            case 10:
                send(client_socket, "Exiting the system...\n", 22, 0);
                exit(0);
            default:
                send(client_socket, "Invalid option. Please try again.\n", 35, 0);
        }
    }
}
