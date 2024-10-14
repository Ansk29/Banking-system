#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_EMPLOYEES 100
#define MAX_LENGTH 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary; // Example field for employee data
} Employee;

Employee employees[MAX_EMPLOYEES]; // Array to hold employee data
int employeeCount = 0; // To keep track of the number of employees

// Function to load employees from the text file
void loadEmployees() {
    FILE *file = fopen("employees.txt", "r");
    if (!file) {
        printf("Error: Unable to open employees database.\n");
        return;
    }

    while (fscanf(file, "%d,%99[^,],%99[^,],%f\n",
                  &employees[employeeCount].id,
                  employees[employeeCount].username,
                  employees[employeeCount].password,
                  &employees[employeeCount].salary) == 4) {
        employeeCount++;
    }
    
    fclose(file);
}

// Function to save employees to the text file
void saveEmployees() {
    FILE *file = fopen("employees.txt", "w");
    if (!file) {
        printf("Error: Unable to open employees database for writing.\n");
        return;
    }

    for (int i = 0; i < employeeCount; i++) {
        fprintf(file, "%d,%s,%s,%.2f\n", employees[i].id, employees[i].username, employees[i].password,
                employees[i].salary);
    }
    fclose(file);
}

// Function to validate employee login
int employee_login(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char login_username[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};

    // Ask for employee login username
    char *login_message = "Enter Employee Username: ";
    send(client_socket, login_message, strlen(login_message), 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(login_username, buffer);

    // Ask for employee password
    char *password_message = "Enter Employee Password: ";
    send(client_socket, password_message, strlen(password_message), 0);
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    strcpy(password, buffer);

    // Print debug information
    printf("Login attempt: Username: '%s', Password: '%s'\n", login_username, password);

    // Validate credentials by checking the loaded data
    for (int i = 0; i < employeeCount; i++) {
        printf("Checking user: '%s' with password: '%s'\n", employees[i].username, employees[i].password);
        if (strcmp(employees[i].username, login_username) == 0 &&
            strcmp(employees[i].password, password) == 0) {
            send(client_socket, "Login successful. Welcome Employee!\n", 36, 0);
            return i;  // Return the index of the logged-in employee
        }
    }

    send(client_socket, "Invalid username or password.\n", 31, 0);
    return -1;  // Login failure
}
// Employee menu after successful login
void employee_menu(int client_socket, int employeeIndex) {
    char buffer[MAX_BUFFER_SIZE] = {0};

    while (1) {
        // Show employee menu
        char *menu = "Employee Menu:\n"
                     "1. Add New Customer\n"
                     "2. Modify Customer Details\n"
                     "3. Process Loan Applications\n"
                     "4. Approve/Reject Loans\n"
                     "5. View Assigned Loan Applications\n"
                     "6. View Customer Transactions\n"
                     "7. Change Password\n"
                     "8. Logout\n"
                     "Select an option: ";
        send(client_socket, menu, strlen(menu), 0);

        // Read employee's choice
        int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0';  // Null-terminate the string
        int choice = atoi(buffer);  // Convert input to an integer

        // Process the menu choice
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
