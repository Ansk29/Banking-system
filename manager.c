#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include <unistd.h>
#include "manager.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_MANAGERS 100
#define MAX_LENGTH 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary; // Example field for employee data
} Employee;

Manager managers[MAX_MANAGERS]; // Array to hold manager data
int managerCount = 0; // To keep track of the number of managers

// Function to load managers from the text file
void loadManagers() {
    FILE *file = fopen("managers.txt", "r");
    if (!file) {
        printf("Error: Unable to open managers database.\n");
        return;
    }

    while (fscanf(file, "%d,%99[^,],%99[^,\n]", 
                  &managers[managerCount].id, 
                  managers[managerCount].username, 
                  managers[managerCount].password) == 3) {
        managerCount++;
    }
    
    fclose(file);
}

// Function to save managers to the text file
void saveManagers() {
    FILE *file = fopen("managers.txt", "w");
    if (!file) {
        printf("Error: Unable to open managers database for writing.\n");
        return;
    }

    for (int i = 0; i < managerCount; i++) {
        fprintf(file, "%d,%s,%s,%.2f\n", managers[i].id, managers[i].username, managers[i].password,
                managers[i].salary);
    }
    fclose(file);
}
//manager

int manager_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char login_username[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};

    // Ask for manager login username
    char *login_message = "Enter Manager Username: ";
    send(client_socket, login_message, strlen(login_message), 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(login_username, buffer);

    // Ask for manager password
    char *password_message = "Enter Manager Password: ";
    send(client_socket, password_message, strlen(password_message), 0);
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(password, buffer);

    // Debugging output
    printf("Login attempt: Username: '%s', Password: '%s'\n", login_username, password);

    // Validate credentials
    for (int i = 0; i < managerCount; i++) {
        printf("Checking manager: Username: '%s', Password: '%s'\n", 
               managers[i].username, 
               managers[i].password); // Debugging line
        if (strcmp(managers[i].username, login_username) == 0 && 
            strcmp(managers[i].password, password) == 0) {
            send(client_socket, "Login successful. Welcome Manager!\n", 36, 0);
            return i;  // Return the index of the logged-in manager
        }
    }

    send(client_socket, "Invalid username or password.\n", 31, 0);
    return -1;  // Login failure
}

// Function to display manager menu and handle functionalities
void manager_menu(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    int option;

    while (1) {
        // Display manager menu
        char *manager_menu =
            "Manager Menu:\n"
            "1. Activate/Deactivate Customer Accounts\n"
            "2. Assign Loan Applications to Employees\n"
            "3. Review Customer Feedback\n"
            "4. Change Password\n"
            "5. Logout\n"
            "6. Exit\n"
            "Choose an option: ";
        
        send(client_socket, manager_menu, strlen(manager_menu), 0);

        // Read user choice
        read(client_socket, buffer, MAX_BUFFER_SIZE);
        option = atoi(buffer);

        switch (option) {
            case 1:
                send(client_socket, "Activating/Deactivating customer accounts...\n", 45, 0);
                break;
            case 2:
                send(client_socket, "Assigning loan applications to employees...\n", 45, 0);
                break;
            case 3:
                send(client_socket, "Reviewing customer feedback...\n", 33, 0);
                break;
            case 4:
                send(client_socket, "Changing password...\n", 21, 0);
                // Implement password change functionality if needed
                break;
            case 5:
                send(client_socket, "Logging out...\n", 16, 0);
                return; // Exit the manager menu
            case 6:
                send(client_socket, "Exiting...\n", 12, 0);
                exit(0); // Exit the server
            default:
                send(client_socket, "Invalid option. Please try again.\n", 36, 0);
                break;
        }
    }
}
