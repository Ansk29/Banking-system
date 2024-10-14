#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Employee structure
typedef struct {
    int id;
    char username[50];
    char password[50];
    // Add other fields if necessary (e.g., role, assigned tasks, etc.)
} Employee;

// Declare functions for employee login and menu
int employee_login(int client_socket);       // Function to handle employee login
void employee_menu(int client_socket, int employee_index);  // Employee menu after login

// Declare functions for employee database loading and handling
void loadEmployees();   // Load employees from a file (e.g., employees.txt)
int findEmployeeByUsername(const char *username);  // Find employee by username in the database

#endif
