#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> // Add this for the isspace function
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
        // Ensure that we are not reading beyond the limits
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

    // Print debug information
    printf("Login attempt: Username: '%s', Password: '%s'\n", login_username, password);

    // Validate credentials by checking the loaded data
    for (int i = 0; i < customerCount; i++) {
        printf("Checking user: '%s' with password: '%s'\n", customers[i].username, customers[i].password);
        if (strcmp(customers[i].username, login_username) == 0 && 
            strcmp(customers[i].password, password) == 0) {
            send(client_socket, "Login successful. Welcome Customer!\n", 36, 0);
            return i;  // Return the index of the logged-in customer
        }
    }

    send(client_socket, "Invalid username or password.\n", 31, 0);
    return -1;  // Login failure
}

// Function to display customer menu and handle customer functionalities
void customer_menu(int client_socket, int customerIndex) {
    char buffer[MAX_BUFFER_SIZE];
    int option;

    while (1) {
        // Display customer menu
        char *customer_menu =
            "Customer Menu:\n"
            "1. View Account Balance\n"
            "2. Deposit Money\n"
            "3. Withdraw Money\n"
            "4. Logout\n"
            "5. Exit\n"
            "Choose an option: ";
        
        send(client_socket, customer_menu, strlen(customer_menu), 0);

        // Read user choice
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
                saveCustomers();  // Save the updated balance
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
                    saveCustomers();  // Save the updated balance
                }
                break;
            }
            case 4:
                send(client_socket, "Logging out...\n", 16, 0);
                return; // Exit the customer menu
            case 5:
                send(client_socket, "Exiting...\n", 12, 0);
                exit(0); // Exit the server
            default:
                send(client_socket, "Invalid option. Please try again.\n", 36, 0);
                break;
        }
    }
}
