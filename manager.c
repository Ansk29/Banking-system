#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "manager.h"
#include "customer.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_MANAGERS 100

Manager managers[MAX_MANAGERS]; // Array to hold manager data
int managerCount = 0; // This is the only definition of managerCount

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

void displayLoanRequests(int client_socket) {
    FILE *file = fopen("loan_request.txt", "r");
    if (!file) {
        send(client_socket, "Error: Unable to open loan requests file.\n", 43, 0);
        return;
    }

    char line[MAX_BUFFER_SIZE];
    int hasRequests = 0;
    char message[MAX_BUFFER_SIZE];

    // Prepare message header
    strcpy(message, "Available Loan Requests (LoanID, CustomerID, Amount, Status):\n");

    // Read loan requests from the file
    while (fgets(line, sizeof(line), file)) {
        LoanRequest request;
        // Assuming loan request format: loanID,customerID,loanAmount,status
        sscanf(line, "%d,%d,%f,%[^\n]", &request.loanID, &request.customerId, &request.loanAmount, request.status);
        
        // Append the request details to the message
        char requestDetails[MAX_BUFFER_SIZE];
        snprintf(requestDetails, sizeof(requestDetails), "LoanID: %d, CustomerID: %d, Amount: %.2f, Status: %s\n",
                 request.loanID, request.customerId, request.loanAmount, request.status);
        
        strcat(message, requestDetails); // Concatenate request details
        hasRequests = 1; // Mark that at least one request is found
    }

    fclose(file);

    if (!hasRequests) {
        strcat(message, "No pending loan requests available.\n");
    }

    // Send the message back to the client
    send(client_socket, message, strlen(message), 0);
}

void change_manager_password(int client_socket, int managerIndex)
{
    char buffer[MAX_BUFFER_SIZE];
    char old_password[MAX_BUFFER_SIZE], new_password[MAX_BUFFER_SIZE];

    // Get old password
    send(client_socket, "Enter old password: ", 20, 0);
    int valread = read(client_socket, old_password, MAX_BUFFER_SIZE);
    old_password[valread] = '\0'; // Null-terminate the string
    // Remove any trailing newline character
    old_password[strcspn(old_password, "\n")] = '\0';

    // Compare old password
    if (strcmp(managers[managerIndex].password, old_password) != 0)
    {
        send(client_socket, "Incorrect old password.\n", 25, 0);
        return;
    }

    // Get new password
    send(client_socket, "Enter new password: ", 20, 0);
    valread = read(client_socket, new_password, MAX_BUFFER_SIZE);
    new_password[valread] = '\0'; // Null-terminate the string
    // Remove any trailing newline character
    new_password[strcspn(new_password, "\n")] = '\0';

    // Update 
    strcpy(managers[managerIndex].password, new_password);

    // Save 
    saveManagers();
    send(client_socket, "Password successfully changed.\n", 32, 0);
}


void assignLoanToEmployee(int client_socket) {
    FILE *requestFile = fopen("loan_request.txt", "r");
    if (!requestFile) {
        send(client_socket, "Error: Unable to open loan requests file.\n", 43, 0);
        return;
    }

    char line[MAX_BUFFER_SIZE];
    int loanID, employeeID;
    int found = 0;

    // Create a temporary file for updating loan requests
    FILE *tempFile = fopen("temp_loan_requests.txt", "w");
    if (!tempFile) {
        send(client_socket, "Error: Unable to create temporary file.\n", 41, 0);
        fclose(requestFile);
        return;
    }

    // Display available loan requests
    displayLoanRequests(client_socket);

    // Request employee ID to assign the loan
    send(client_socket, "Enter Employee ID to assign the loan: ", 38, 0);
    read(client_socket, line, sizeof(line));
    employeeID = atoi(line);

    // Ask for the Loan ID to assign
    send(client_socket, "Enter Loan ID to assign: ", 25, 0);
    read(client_socket, line, sizeof(line));
    loanID = atoi(line);

    // Open loan assigned file for writing
    FILE *assignedFile = fopen("loan_assigned.txt", "a");
    if (!assignedFile) {
        send(client_socket, "Error: Unable to open loan assigned file.\n", 44, 0);
        fclose(tempFile);
        fclose(requestFile);
        return;
    }

    // Process loans
    while (fgets(line, sizeof(line), requestFile)) {
        LoanRequest request;
        // Read loan request
        sscanf(line, "%d,%d,%f,%[^\n]", &request.loanID, &request.customerId, &request.loanAmount, request.status);
        
        if (request.loanID == loanID) {
            // Loan matches the one to assign
            fprintf(assignedFile, "%d,%d,%.2f,%d\n", request.loanID, request.customerId, request.loanAmount, employeeID);
            found = 1; // Mark that we found and assigned this loan
        } else {
            // Keep the loan in requests
            fprintf(tempFile, "%d,%d,%.2f,%s\n", request.loanID, request.customerId, request.loanAmount, request.status);
        }
    }

    fclose(requestFile);
    fclose(tempFile);
    fclose(assignedFile);

    // Replace old request file with the updated one
    remove("loan_request.txt");
    rename("temp_loan_requests.txt", "loan_request.txt");

    // Inform the user
    if (found) {
        snprintf(line, sizeof(line), "Loan ID %d assigned to Employee ID %d.\n", loanID, employeeID);
    } else {
        snprintf(line, sizeof(line), "Loan ID %d not found in requests.\n", loanID);
    }
    send(client_socket, line, strlen(line), 0);
}
// Manager login function
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
            "3. Change Password\n"
            "4. Logout\n"
            "5. Exit\n"
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
                assignLoanToEmployee(client_socket);
                break;
            case 3:

                send(client_socket, "Changing password...\n", 21, 0);
                // Implement password change functionality if needed
                break;
            case 4:
                send(client_socket, "Logging out...\n", 16, 0);
                return; // Exit the manager menu
            case 5:
                send(client_socket, "Exiting...\n", 12, 0);
                exit(0); // Exit the server
            default:
                send(client_socket, "Invalid option. Please try again.\n", 36, 0);
                break;
        }
    }
}
