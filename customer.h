// customer.h
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <stdio.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_CUSTOMERS 100
#define MAX_LENGTH 100

// Struct definition for Customer
typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float balance;
} Customer;

// Function prototypes
void loadCustomers();
void saveCustomers();
int customer_login(int client_socket);  // Ensure this prototype is declared
void customer_menu(int client_socket, int customerIndex);

#endif // CUSTOMER_H
