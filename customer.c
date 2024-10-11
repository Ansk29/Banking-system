// customer.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/socket.h>
#include "customer.h" // Include the customer header

// Define the customer credentials
const char *customer_id = "customer123";
const char *customer_password = "custpass";

#define MAX_BUFFER_SIZE 1024

// Function to validate customer login
int customer_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char login_id[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};

    // Ask for customer login ID
    char *login_message = "Enter Customer Login ID: ";
    send(client_socket, login_message, strlen(login_message), 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(login_id, buffer);

    // Ask for customer password
    char *password_message = "Enter Customer Password: ";
    send(client_socket, password_message, strlen(password_message), 0);
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(password, buffer);

    // Validate credentials
    if (strcmp(login_id, customer_id) == 0 && strcmp(password, customer_password) == 0) {
        send(client_socket, "Login successful. Welcome Customer!\n", 36, 0);
        return 1;  // Login success
    } else {
        send(client_socket, "Invalid login ID or password.\n", 31, 0);
        return 0;  // Login failure
    }
}

// Function to display customer menu and handle customer functionalities
void customer_menu(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int option;
    float account_balance = 1000.00; // Example initial balance

    while (1) {
        // Display customer menu
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

        // Read user choice
        read(client_socket, buffer, MAX_BUFFER_SIZE);
        option = atoi(buffer);

        switch (option) {
            case 1:
                sprintf(buffer, "Your current account balance is: $%.2f\n", account_balance);
                send(client_socket, buffer, strlen(buffer), 0);
                break;
            case 2: {
                float deposit_amount;
                send(client_socket, "Enter amount to deposit: ", 25, 0);
                read(client_socket, buffer, MAX_BUFFER_SIZE);
                deposit_amount = atof(buffer);
                account_balance += deposit_amount;
                sprintf(buffer, "You have successfully deposited: $%.2f\n", deposit_amount);
                send(client_socket, buffer, strlen(buffer), 0);
                break;
            }
            case 3: {
                float withdraw_amount;
                send(client_socket, "Enter amount to withdraw: ", 26, 0);
                read(client_socket, buffer, MAX_BUFFER_SIZE);
                withdraw_amount = atof(buffer);
                if (withdraw_amount > account_balance) {
                    send(client_socket, "Insufficient balance for this withdrawal.\n", 43, 0);
                } else {
                    account_balance -= withdraw_amount;
                    sprintf(buffer, "You have successfully withdrawn: $%.2f\n", withdraw_amount);
                    send(client_socket, buffer, strlen(buffer), 0);
                }
                break;
            }
            case 4:
                send(client_socket, "Transfer Funds functionality not implemented yet.\n", 51, 0);
                break;
            case 5:
                send(client_socket, "Apply for a Loan functionality not implemented yet.\n", 50, 0);
                break;
            case 6:
                send(client_socket, "Change Password functionality not implemented yet.\n", 50, 0);
                break;
            case 7:
                send(client_socket, "Add Feedback functionality not implemented yet.\n", 49, 0);
                break;
            case 8:
                send(client_socket, "View Transaction History functionality not implemented yet.\n", 60, 0);
                break;
            case 9:
                send(client_socket, "Logging out...\n", 16, 0);
                return; // Exit the customer menu
            case 10:
                send(client_socket, "Exiting...\n", 12, 0);
                exit(0); // Exit the server
            default:
                send(client_socket, "Invalid option. Please try again.\n", 36, 0);
                break;
        }
    }
}
