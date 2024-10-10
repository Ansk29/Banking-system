// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "admin.h"  // Include the admin header

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    int valread;

    // Sending a welcome message and menu options to the client
    char *welcome_message = "Welcome to the Banking System!\n";
    char *menu = "Please select your role:\n1. Admin\n2. Customer\n3. Employee\n4. Manager\n";
    
    send(client_socket, welcome_message, strlen(welcome_message), 0);
    send(client_socket, menu, strlen(menu), 0);
    
    // Reading the role selection from the client
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string

    // Parse the user's choice
    int user_choice = atoi(buffer);
    switch (user_choice) {
        case 1:  // Admin selected
            if (admin_login(client_socket)) {
                // Admin functionality can be implemented here
                send(client_socket, "Admin functionalities will be added here.\n", 42, 0);
            }
            break;
        case 2:
            send(client_socket, "You have chosen Customer.\n", 27, 0);
            break;
        case 3:
            send(client_socket, "You have chosen Employee.\n", 27, 0);
            break;
        case 4:
            send(client_socket, "You have chosen Manager.\n", 26, 0);
            break;
        default:
            send(client_socket, "Invalid choice. Please try again.\n", 36, 0);
            break;
    }

    printf("Response sent to client.\n");
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Defining the address and port for the server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the defined port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept a new connection from a client
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection established with client.\n");

        // Handle client request in a function
        handle_client(new_socket);

        // Close the connection with the current client
        close(new_socket);
    }

    return 0;
}
