#ifndef MANAGER_H
#define MANAGER_H

#define MAX_LENGTH 100
#define MAX_MANAGERS 100

typedef struct {
    int id;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    float salary; 
} Manager;

void loadManagers();
int manager_login(int client_socket);
void manager_menu(int client_socket);

#endif
