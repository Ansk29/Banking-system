#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>  // Include for socket functions

#include "admin.h"  // Include the admin header

// Define the admin credentials
const char *admin_id = "admin123";
const char *admin_password = "pass123";

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

// Function to validate admin login
int admin_login(int client_socket) {
    char buffer[1024] = {0};
    char login_id[1024] = {0};
    char password[1024] = {0};

    // Ask for admin login ID
    char *login_message = "Enter Admin Login ID: ";
    send(client_socket, login_message, strlen(login_message), 0);
    int valread = read(client_socket, buffer, 1024);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(login_id, buffer);

    // Ask for admin password
    char *password_message = "Enter Admin Password: ";
    send(client_socket, password_message, strlen(password_message), 0);
    valread = read(client_socket, buffer, 1024);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(password, buffer);

    // Validate credentials
    if (strcmp(login_id, admin_id) == 0 && strcmp(password, admin_password) == 0) {
        send(client_socket, "Login successful. Welcome Admin!\n", 33, 0);
        return 1;  // Login success
    } else {
        send(client_socket, "Invalid login ID or password.\n", 31, 0);
        return 0;  // Login failure
    }
}

// Function to display admin menu and handle admin functionalities
void admin_menu(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int option;

    while (1) {
        // Display admin menu
        char *admin_menu = 
            "Admin Menu:\n"
            "1. Add New Bank Employee\n"
            "2. Modify Customer/Employee Details\n"
            "3. Manage User Roles\n"
            "4. Change Password\n"
            "5. Logout\n"
            "6. Exit\n"
            "Choose an option: ";
        
        send(client_socket, admin_menu, strlen(admin_menu), 0);

        // Read user choice
        read(client_socket, buffer, MAX_BUFFER_SIZE);
        option = atoi(buffer);

        switch (option) {
            case 1:
                // Functionality for adding new bank employee
                send(client_socket, "Add New Bank Employee functionality not implemented yet.\n", 56, 0);
                break;
            case 2:
                // Functionality for modifying customer/employee details
                send(client_socket, "Modify Customer/Employee Details functionality not implemented yet.\n", 68, 0);
                break;
            case 3:
                // Functionality for managing user roles
                send(client_socket, "Manage User Roles functionality not implemented yet.\n", 56, 0);
                break;
            case 4:
                // Functionality for changing password
                send(client_socket, "Change Password functionality not implemented yet.\n", 56, 0);
                break;
            case 5:
                send(client_socket, "Logging out...\n", 16, 0);
                return; // Exit the admin menu
            case 6:
                send(client_socket, "Exiting...\n", 12, 0);
                exit(0); // Exit the server
            default:
                send(client_socket, "Invalid option. Please try again.\n", 36, 0);
                break;
        }
    }
}