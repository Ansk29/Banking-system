
#ifndef CUSTOMER_H
#define CUSTOMER_H

#define MAX_LENGTH 100
#define MAX_CUSTOMERS 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float balance;  // Example field for customer balance
} Customer;

#define MAX_LENGTH 100

typedef struct {
    int loanID;               // Unique loan ID for each loan request
    int customerId;           // Customer ID who requested the loan
    float loanAmount;         // The amount of the loan requested
    char status[MAX_LENGTH];  // Status of the loan (e.g., "Pending", "Assigned", "Approved", "Rejected")
    char details[MAX_LENGTH]; // Additional details or description of the loan
} LoanRequest;


// Use extern to declare the variables so that they are not defined multiple times
extern Customer customers[MAX_CUSTOMERS];
extern int customerCount;

// Function declarations for customer-related functions
void loadCustomers();
void saveCustomers();
void add_new_customer(int client_socket);
void modify_customer(int client_socket);
void view_customer_transactions(int client_socket);

#endif // CUSTOMER_H
