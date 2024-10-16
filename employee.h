#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
#define MAX_EMPLOYEES 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary; // Example field for employee data
} Employee;

extern Employee employees[MAX_EMPLOYEES];
extern int employeeCount;


// Declare functions for employee login and menu
int employee_login(int client_socket);       // Function to handle employee login
void employee_menu(int client_socket, int employee_index);  // Employee menu after login

// Declare functions for employee database loading and handling
void loadEmployees();   // Load employees from a file (e.g., employees.txt)
int findEmployeeByUsername(const char *username);  // Find employee by username in the database

void loadEmployees();
void saveEmployees();
void loadCustomers();
void saveCustomers();
int employee_login(int client_socket);
void add_new_customer(int client_socket);
void modify_customer(int client_socket);
void view_customer_transactions(int client_socket);
void process_loan_application(int client_socket);
void approve_reject_loans(int client_socket);
void change_employee_password(int client_socket, int employeeIndex);
void employee_menu(int client_socket, int employeeIndex);

#endif // EMPLOYEE_H

