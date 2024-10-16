#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_CUSTOMERS 100
#define MAX_LENGTH 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float balance;
    
} Customer;

Customer customers[MAX_CUSTOMERS]; // Array to hold customer data
int customerCount = 0; // To keep track of the number of customers

// Function to load customers from the text file
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
void transfer_funds(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    int recipient_id;
    float transfer_amount;

    // Get recipient customer ID
    send(client_socket, "Enter recipient customer ID: ", 29, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    recipient_id = atoi(buffer);

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
        return;
    }

    // Get transfer amount
    send(client_socket, "Enter transfer amount: ", 23, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    transfer_amount = atof(buffer);

    if (transfer_amount > customers[customerIndex].balance) {
        send(client_socket, "Insufficient balance for transfer.\n", 35, 0);
    } else {
        customers[customerIndex].balance -= transfer_amount;
        customers[recipientIndex].balance += transfer_amount;
        sprintf(buffer, "Transfer successful! $%.2f has been sent to customer ID %d.\n", transfer_amount, recipient_id);
        send(client_socket, buffer, strlen(buffer), 0);
        saveCustomers();
    }
}

// Function to apply for a loan
void apply_loan(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    float loan_amount;

    // Get loan amount from the customer
    send(client_socket, "Enter loan amount: ", 19, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    loan_amount = atof(buffer);

    sprintf(buffer, "Loan application for $%.2f has been submitted.\n", loan_amount);
    send(client_socket, buffer, strlen(buffer), 0);
}

// Function to change password
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
