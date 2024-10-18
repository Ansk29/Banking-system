#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "customer.h"

#define MAX_EMPLOYEES 100
#define MAX_LENGTH 100

typedef struct
{
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary; // Example field for employee data
} Employee;

#define MAX_BUFFER_SIZE 1024
Employee employees[MAX_EMPLOYEES]; // Array to hold employee data
int employeeCount = 0;             // To keep track of the number of employees

// Load the customer database
extern void loadCustomers(); // This will be from customer.c
extern void saveCustomers(); // To save updated customer data

// Function to add a new customer
void add_new_customer(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    Customer new_customer;

    // Get new customer details
    send(client_socket, "Enter new customer ID: ", 23, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    new_customer.id = atoi(buffer);

    send(client_socket, "Enter new customer username: ", 29, 0);
    read(client_socket, new_customer.username, MAX_BUFFER_SIZE);

    send(client_socket, "Enter new customer password: ", 29, 0);
    read(client_socket, new_customer.password, MAX_BUFFER_SIZE);

    send(client_socket, "Enter initial balance: ", 23, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    new_customer.balance = atof(buffer);

    // Add new customer to the array
    customers[customerCount] = new_customer;
    customerCount++;

    // Save updated customer data
    saveCustomers();
    send(client_socket, "New customer added successfully.\n", 33, 0);
}

// Function to modify customer details
void modify_customer_details(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    int customer_id, i;
    float new_balance;
    char new_username[MAX_BUFFER_SIZE];
    char new_password[MAX_BUFFER_SIZE];

    send(client_socket, "Enter customer ID to modify: ", 29, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    customer_id = atoi(buffer);

    // Find the customer by ID
    for (i = 0; i < customerCount; i++)
    {
        if (customers[i].id == customer_id)
        {
            send(client_socket, "Enter new username: ", 21, 0);
            read(client_socket, new_username, MAX_BUFFER_SIZE);
            strcpy(customers[i].username, new_username);

            send(client_socket, "Enter new password: ", 21, 0);
            read(client_socket, new_password, MAX_BUFFER_SIZE);
            strcpy(customers[i].password, new_password);

            send(client_socket, "Enter new balance: ", 20, 0);
            read(client_socket, buffer, MAX_BUFFER_SIZE);
            new_balance = atof(buffer);
            customers[i].balance = new_balance;

            saveCustomers();
            send(client_socket, "Customer details updated successfully.\n", 40, 0);
            return;
        }
    }
    send(client_socket, "Customer ID not found.\n", 24, 0);
}

// Function to process loan applications
void process_loan_applications(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    int customer_id;

    send(client_socket, "Enter customer ID to process loan: ", 35, 0);
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    customer_id = atoi(buffer);

    // Find the customer by ID and confirm loan
    for (int i = 0; i < customerCount; i++)
    {
        if (customers[i].id == customer_id)
        {
            // Assuming a loan field in Customer struct (could be added in future)
            send(client_socket, "Loan application processed.\n", 29, 0);
            return;
        }
    }
    send(client_socket, "Customer ID not found.\n", 24, 0);
}

// Function to approve/reject loans
void approve_or_reject_loans(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    int loan_id;
    char decision;

    // Prompt for loan ID
    send(client_socket, "Enter Loan ID to approve/reject: ", 33, 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    loan_id = atoi(buffer);

    // Open the loan file for reading
    FILE *loan_file = fopen("loan_assigned.txt", "r");
    if (!loan_file)
    {
        send(client_socket, "Error: Could not open loan file.\n", 34, 0);
        return;
    }

    // Open a temporary file to write updated information
    FILE *temp_file = fopen("temp_loan.txt", "w");
    if (!temp_file)
    {
        send(client_socket, "Error: Could not create temporary file.\n", 40, 0);
        fclose(loan_file);
        return;
    }

    // Open the file to log processed loans
    FILE *processed_loan_file = fopen("processed_loans.txt", "a");
    if (!processed_loan_file)
    {
        send(client_socket, "Error: Could not open processed loans file.\n", 44, 0);
        fclose(loan_file);
        fclose(temp_file);
        return;
    }

    int found = 0;
    char line[256];

    // Read each line from the loan file
    while (fgets(line, sizeof(line), loan_file))
    {
        int current_loan_id, customer_id, employee_id;
        float loan_amount;
        sscanf(line, "%d,%d,%f,%d", &current_loan_id, &customer_id, &loan_amount, &employee_id);

        // If the loan ID matches, process the loan
        if (current_loan_id == loan_id)
        {
            found = 1;

            // Prompt for approval or rejection
            send(client_socket, "Approve (A) or Reject (R) the loan? ", 36, 0);
            valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
            buffer[valread] = '\0'; // Null-terminate the string
            decision = buffer[0];

            if (decision == 'A' || decision == 'a')
            {
                // Write "Approved" to processed loan file
                fprintf(processed_loan_file, "%d,%d,%.2f,%d,Approved\n", current_loan_id, customer_id, loan_amount, employee_id);
                send(client_socket, "Loan approved.\n", 15, 0);
            }
            else if (decision == 'R' || decision == 'r')
            {
                // Write "Rejected" to processed loan file
                fprintf(processed_loan_file, "%d,%d,%.2f,%d,Rejected\n", current_loan_id, customer_id, loan_amount, employee_id);
                send(client_socket, "Loan rejected.\n", 15, 0);
            }
            else
            {
                // If invalid decision, write the original line back
                fprintf(temp_file, "%s", line);
                send(client_socket, "Invalid choice. Loan decision canceled.\n", 41, 0);
            }
        }
        else
        {
            // Copy the original line to the temp file for non-matching loans
            fprintf(temp_file, "%s", line);
        }
    }

    if (!found)
    {
        send(client_socket, "Loan ID not found.\n", 20, 0);
    }

    // Close all files
    fclose(loan_file);
    fclose(temp_file);
    fclose(processed_loan_file);

    // Replace the old loan file with the updated temp file
    if (found)
    {
        remove("loan_assigned.txt");
        rename("temp_loan.txt", "loan_assigned.txt");
    }
    else
    {
        // If loan ID was not found, delete the temp file
        remove("temp_loan.txt");
    }
}

// assigned loans
void view_assigned_loan_applications(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    char line[256];
    int employee_id, found = 0;

    // Ask for employee ID input
    send(client_socket, "Enter your Employee ID: ", 24, 0);
    int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0';     // Null-terminate the string
    employee_id = atoi(buffer); // Convert employee ID to integer

    // Open the loan file to read assigned loans
    FILE *loan_file = fopen("loan_assigned.txt", "r");
    if (!loan_file)
    {
        send(client_socket, "Error: Could not open loan file.\n", 34, 0);
        return;
    }

    // Send the header for the assigned loans
    send(client_socket, "Assigned Loan Applications:\nLoan ID | Customer ID | Amount \n", 57, 0);

    // Read each line from the loan file
    while (fgets(line, sizeof(line), loan_file))
    {
        int current_loan_id, customer_id, assigned_employee_id;
        float loan_amount;

        // Parse the line (loan ID, customer ID, loan amount, employee ID)
        sscanf(line, "%d,%d,%f,%d", &current_loan_id, &customer_id, &loan_amount, &assigned_employee_id);

        // If the employee ID matches, show the loan details
        if (assigned_employee_id == employee_id)
        {
            found = 1;
            // Format the loan details to send to the employee
            snprintf(buffer, sizeof(buffer), "%d | %d | %.2f\n", current_loan_id, customer_id, loan_amount);
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }

    if (!found)
    {
        send(client_socket, "No loans assigned to you.\n", 26, 0);
    }

    // Close the loan file
    fclose(loan_file);
}

// Function to view customer transactions
void view_customer_transactions(int client_socket)
{
    send(client_socket, "Displaying customer transactions is under development.\n", 56, 0);
}

// Function to change employee password
// Function to change employee password
void change_employee_password(int client_socket, int employeeIndex)
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
    if (strcmp(employees[employeeIndex].password, old_password) != 0)
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

    // Update the employee's password
    strcpy(employees[employeeIndex].password, new_password);

    // Save updated employees
    saveEmployees(); // Assuming a save function for employees exists
    send(client_socket, "Password successfully changed.\n", 32, 0);
}

// Logout function for employees
void employee_logout(int client_socket)
{
    send(client_socket, "Logging out...\n", 16, 0);
}

// Function to load employees from the text file
void loadEmployees()
{
    FILE *file = fopen("employees.txt", "r");
    if (!file)
    {
        printf("Error: Unable to open employees database.\n");
        return;
    }

    while (fscanf(file, "%d,%99[^,],%99[^,],%f\n",
                  &employees[employeeCount].id,
                  employees[employeeCount].username,
                  employees[employeeCount].password,
                  &employees[employeeCount].salary) == 4)
    {
        employeeCount++;
    }

    fclose(file);
}

// Function to save employees to the text file
void saveEmployees()
{
    FILE *file = fopen("employees.txt", "w");
    if (!file)
    {
        printf("Error: Unable to open employees database for writing.\n");
        return;
    }

    for (int i = 0; i < employeeCount; i++)
    {
        fprintf(file, "%d,%s,%s,%.2f\n", employees[i].id, employees[i].username, employees[i].password,
                employees[i].salary);
    }
    fclose(file);
}

// Function to validate employee login
int employee_login(int client_socket)
{
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
    for (int i = 0; i < employeeCount; i++)
    {
        printf("Checking user: '%s' with password: '%s'\n", employees[i].username, employees[i].password);
        if (strcmp(employees[i].username, login_username) == 0 &&
            strcmp(employees[i].password, password) == 0)
        {
            send(client_socket, "Login successful. Welcome Employee!\n", 36, 0);
            return i; // Return the index of the logged-in employee
        }
    }

    send(client_socket, "Invalid username or password.\n", 31, 0);
    return -1; // Login failure
}
// Employee menu after successful login
// Employee menu after successful login
void employee_menu(int client_socket, int employeeIndex)
{
    char buffer[MAX_BUFFER_SIZE] = {0};

    while (1)
    {
        // Show employee menu
        char *menu = "Employee Menu:\n"
                     "1. Add New Customer\n"
                     "2. Modify Customer Details\n"

                     "3. Approve/Reject Loans\n"
                     "4. View Assigned Loan Applications\n"
                     "5. View Customer Transactions\n"
                     "6. Change Password\n"
                     "7. Logout\n"
                     "Select an option: ";
        send(client_socket, menu, strlen(menu), 0);

        // Read employee's choice
        int valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0';    // Null-terminate the string
        int choice = atoi(buffer); // Convert input to an integer

        // Process the menu choice
        switch (choice)
        {
        case 1:
            add_new_customer(client_socket);
            break;
        case 2:
            modify_customer_details(client_socket);
            break;
        /*case 3:
            process_loan_applications(client_socket);
            break;*/
        case 3:
            approve_or_reject_loans(client_socket);
            break;
        case 4:
            view_assigned_loan_applications(client_socket);
            break;
        case 5:
            view_customer_transactions(client_socket);
            break;
        case 6:
            change_employee_password(client_socket, employeeIndex);
            break;
        case 7:
            employee_logout(client_socket);
            return; // Logout and return to main menu
        default:
            send(client_socket, "Invalid choice. Please try again.\n", 36, 0);
        }
    }
}
