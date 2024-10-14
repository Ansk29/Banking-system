#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "admin.h"
#include "customer.h"
#include "employee.h"
#include "manager.h" // Include the manager header

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE] = {0};
    int valread;

    // Load data from text files for employees, customers, etc.
    loadCustomers();
    loadEmployees();
    loadManagers();

    // Sending a welcome message and role selection menu to the client
    char *welcome_message = "Welcome to the Banking System!\n";
    char *menu = "Please select your role:\n1. Admin\n2. Customer\n3. Employee\n4. Manager\n";

    send(client_socket, welcome_message, strlen(welcome_message), 0);
    send(client_socket, menu, strlen(menu), 0);

    // Reading the role selection from the client
    valread = read(client_socket, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string

    // Parse the user's role choice
    int user_choice = atoi(buffer);

    // Based on the user's role choice, perform the respective login and show menu
    switch (user_choice)
    {
    case 1: // Admin selected
        if (admin_login(client_socket))
        {
            admin_menu(client_socket); // Show admin menu on successful login
        }
        break;
    case 2: // Customer selected
    {
        int customer_index = customer_login(client_socket); // Attempt customer login
        if (customer_index != -1)
        {
            customer_menu(client_socket, customer_index); // Show customer menu on successful login
        }
        break;
    }
    case 3: // Employee selected
    {
        int employee_index = employee_login(client_socket); // Attempt employee login
        if (employee_index != -1)
        {
            employee_menu(client_socket, employee_index); // Show employee menu on successful login
        }
        break;
    }
    case 4: // Manager selected
    {
        int manager_index = manager_login(client_socket); // Attempt manager login
        if (manager_index != -1)
        {
            manager_menu(client_socket); // Show manager menu on successful login
        }
        break;
    }
        send(client_socket, "Invalid choice. Please try again.\n", 36, 0);
        break;
    }

    printf("Response sent to client.\n");
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating the server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting up the server address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the defined port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1)
    {
        // Accepting a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection established with client.\n");

        // Handling the client in the handle_client function
        handle_client(new_socket);

        // Close the connection with the current client
        close(new_socket);
    }

    return 0;
}
