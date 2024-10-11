// manager.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

// Dummy manager credentials
const char *manager_username = "manager";
const char *manager_password = "manager123";

// Manager login function
int manager_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    int valread;

    // Asking for manager credentials
    char *login_message = "Manager Login:\nUsername: ";
    send(client_socket, login_message, strlen(login_message), 0);

    // Read username
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string

    if (strcmp(buffer, manager_username) != 0) {
        send(client_socket, "Invalid Username.\n", 19, 0);
        return 0;
    }

    // Ask for password
    char *password_message = "Password: ";
    send(client_socket, password_message, strlen(password_message), 0);

    // Read password
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string

    if (strcmp(buffer, manager_password) != 0) {
        send(client_socket, "Invalid Password.\n", 19, 0);
        return 0;
    }

    // Login successful
    send(client_socket, "Login successful.\n", 19, 0);
    return 1;
}

// Manager menu function
void manager_menu(int client_socket) {
    char *menu = "Manager Menu:\n1. Activate/Deactivate Customer Accounts\n"
                 "2. Assign Loan Application Processes to Employees\n"
                 "3. Review Customer Feedback\n4. Change Password\n5. Logout\n";
    send(client_socket, menu, strlen(menu), 0);

    char buffer[MAX_BUFFER_SIZE] = {0};
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string

    int choice = atoi(buffer);
    switch (choice) {
        case 1:
            send(client_socket, "Activating/Deactivating Accounts...\n", 38, 0);
            break;
        case 2:
            send(client_socket, "Assigning Loan Applications...\n", 31, 0);
            break;
        case 3:
            send(client_socket, "Reviewing Feedback...\n", 23, 0);
            break;
        case 4:
            send(client_socket, "Changing Password...\n", 22, 0);
            break;
        case 5:
            send(client_socket, "Logging out...\n", 15, 0);
            return;
        default:
            send(client_socket, "Invalid choice.\n", 17, 0);
            break;
    }

    // Show menu again after action
    manager_menu(client_socket);
}
