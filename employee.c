// employee.cempllo
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "employee.h"

#define MAX_BUFFER_SIZE 1024

// Hardcoded employee credentials
const char *employee_username = "employee";
const char *employee_password = "emp123";

// Function for employee login
int employee_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char input_username[MAX_BUFFER_SIZE];
    char input_password[MAX_BUFFER_SIZE];

    // Ask for employee username
    send(client_socket, "Enter Employee Username: ", 25, 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0';  // Null-terminate the string
    strcpy(input_username, buffer);  // Copy the received username

    // Ask for employee password
    send(client_socket, "Enter Employee Password: ", 25, 0);
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0';  // Null-terminate the string
    strcpy(input_password, buffer);  // Copy the received password

    // Validate credentials
    if (strcmp(input_username, employee_username) == 0 && strcmp(input_password, employee_password) == 0) {
        send(client_socket, "Login successful!\n", 18, 0);
        return 1;  // Successful login
    } else {
        send(client_socket, "Invalid credentials. Login failed.\n", 35, 0);
        return 0;  // Failed login
    }
}

// Employee menu after successful login
void employee_menu(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};

    while (1) {
        // Show employee menu
        char *menu = "Employee Menu:\n1. Add New Customer\n2. Modify Customer Details\n3. Process Loan Applications\n4. Approve/Reject Loans\n5. View Assigned Loan Applications\n6. View Customer Transactions\n7. Change Password\n8. Logout\n";
        send(client_socket, menu, strlen(menu), 0);

        // Read employee's choice
        int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0';  // Null-terminate the string
        int choice = atoi(buffer);  // Convert input to an integer

        switch (choice) {
            case 1:
                send(client_socket, "Add New Customer selected.\n", 27, 0);
                break;
            case 2:
                send(client_socket, "Modify Customer Details selected.\n", 35, 0);
                break;
            case 3:
                send(client_socket, "Process Loan Applications selected.\n", 36, 0);
                break;
            case 4:
                send(client_socket, "Approve/Reject Loans selected.\n", 31, 0);
                break;
            case 5:
                send(client_socket, "View Assigned Loan Applications selected.\n", 42, 0);
                break;
            case 6:
                send(client_socket, "View Customer Transactions selected.\n", 37, 0);
                break;
            case 7:
                send(client_socket, "Change Password selected.\n", 27, 0);
                break;
            case 8:
                send(client_socket, "Logging out...\n", 16, 0);
                return;  // Logout and return to main menu
            default:
                send(client_socket, "Invalid choice. Please try again.\n", 36, 0);
        }
    }
}
