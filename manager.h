#ifndef MANAGER_H
#define MANAGER_H

#define MAX_LENGTH 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary;
} Manager;

extern int managerCount; // Declare managerCount as extern
extern Manager managers[]; // Declare managers as extern

// Function prototypes
void loadManagers();
void saveManagers();
void assignLoanToEmployee(int client_socket);
int manager_login(int client_socket);
void manager_menu(int client_socket);

#endif // MANAGER_H
