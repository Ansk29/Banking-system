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

#define CUSTOMER_FILE "customers.txt"
#define EMPLOYEE_FILE "employees.txt"

void change_admin_password(int client_socket) {
    printf("Change the Hardcoded Crednetiails in the file");
}
 
 void add_new_employee(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    char employee_id[100], employee_name[100], employee_password[100];

    // Get employee details
    send(client_socket, "Enter Employee ID: ", 20, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(employee_id, buffer);
    
    send(client_socket, "Enter Employee Name: ", 21, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(employee_name, buffer);
    
    send(client_socket, "Enter Employee Password: ", 24, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(employee_password, buffer);

    // Save employee data to employee.txt
    FILE *file = fopen(EMPLOYEE_FILE, "a");
    if (file) {
        fprintf(file, "%s,%s,%s\n", employee_id, employee_name, employee_password);
        fclose(file);
        send(client_socket, "New employee added successfully.\n", 34, 0);
    } else {
        send(client_socket, "Failed to add new employee.\n", 30, 0);
    }
}

void modify_employee_details(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    char employee_id[100];
    char new_employee_name[100];
    char new_employee_password[100];

    // Ask for the employee ID to modify
    send(client_socket, "Enter Employee ID to modify: ", 30, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    strcpy(employee_id, buffer);
    
    // Open the employee file
    FILE *file = fopen(EMPLOYEE_FILE, "r");
    if (file == NULL) {
        send(client_socket, "Failed to open employee file.\n", 32, 0);
        return;
    }

    // Create a temporary file to write modified data
    FILE *temp_file = fopen("temp_employee.txt", "w");
    if (temp_file == NULL) {
        send(client_socket, "Failed to create temporary file.\n", 35, 0);
        fclose(file);
        return;
    }

    char line[MAX_BUFFER_SIZE];
    int found = 0;

    // Read through the employee file
    while (fgets(line, sizeof(line), file)) {
        char current_employee_id[100];
        char current_employee_name[100];
        char current_employee_password[100];

        // Split the line into ID, Name, and Password
        sscanf(line, "%99[^,],%99[^,],%99[^\n]", current_employee_id, current_employee_name, current_employee_password);

        // Check if the current ID matches the one to modify
        if (strcmp(current_employee_id, employee_id) == 0) {
            found = 1;

            // Ask for new employee details
            send(client_socket, "Enter new Employee Name: ", 25, 0);
            read(client_socket, buffer, MAX_BUFFER_SIZE);
            strcpy(new_employee_name, buffer);

            send(client_socket, "Enter new Employee Password: ", 29, 0);
            read(client_socket, buffer, MAX_BUFFER_SIZE);
            strcpy(new_employee_password, buffer);

            // Write the modified line to the temporary file
            fprintf(temp_file, "%s,%s,%s\n", employee_id, new_employee_name, new_employee_password);
            send(client_socket, "Employee details modified successfully.\n", 40, 0);
        } else {
            // Write the existing line to the temporary file
            fprintf(temp_file, "%s", line);
        }
    }

    // Close both files
    fclose(file);
    fclose(temp_file);

    // If the employee was not found, send a message to the client
    if (!found) {
        send(client_socket, "Employee ID not found.\n", 24, 0);
        remove("temp_employee.txt");  // Remove the temporary file if not needed
        return;
    }

    // Replace the original file with the temporary file
    remove(EMPLOYEE_FILE);
    rename("temp_employee.txt", EMPLOYEE_FILE);
}

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
            "2. Modify Employee Details\n"
            "3. Add New Manager \n"
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
                add_new_employee(client_socket);
                break;
            case 2:
                // Functionality for modifying customer/employee details
                modify_employee_details(client_socket);
                break;
            case 3:
                // Functionality for managing user roles
                send(client_socket, "Manage User Roles functionality not implemented yet.\n", 56, 0);
                break;
            case 4:
                // Functionality for changing password
               send(client_socket, "Change Password could be done by changing the hardcoded credentials in the file \n", 28, 0);
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