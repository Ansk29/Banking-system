// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

// Function to remove newline character from user input
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';  // Removes the first newline character if present
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE] = {0};
    char input[1024];

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Configuring server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connecting to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Reading welcome message from server
    int valread = read(sock, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    printf("%s", buffer);

    // Reading menu options from server
    valread = read(sock, buffer, MAX_BUFFER_SIZE);
    buffer[valread] = '\0'; // Null-terminate the string
    printf("%s", buffer);

    // User input for role selection
    printf("Enter your choice (1-4): ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Error reading input");
        close(sock);
        return -1;
    }

    // Remove the newline character from role choice
    remove_newline(input);

    // Send the role choice to the server
    send(sock, input, strlen(input), 0);

    // Admin login workflow
    if (atoi(input) == 1) {
        // Reading server's prompt for Admin Login ID
        valread = read(sock, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0'; // Null-terminate the string
        printf("%s", buffer);

        // Input login ID
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading login ID");
            close(sock);
            return -1;
        }

        // Remove the newline character from login ID
        remove_newline(input);

        // Send login ID to server
        send(sock, input, strlen(input), 0);

        // Reading server's prompt for Admin Password
        valread = read(sock, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0'; // Null-terminate the string
        printf("%s", buffer);

        // Input password
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading password");
            close(sock);
            return -1;
        }

        // Remove the newline character from password
        remove_newline(input);

        // Send password to server
        send(sock, input, strlen(input), 0);

        // Reading the server's response
        valread = read(sock, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0'; // Null-terminate the string
        printf("%s", buffer);

        // Admin functionalities (after successful login)
        while (1) {
            // Read admin menu options from server
            valread = read(sock, buffer, MAX_BUFFER_SIZE);
            buffer[valread] = '\0'; // Null-terminate the string
            printf("%s", buffer);

            // Input admin choice
            if (fgets(input, sizeof(input), stdin) == NULL) {
                perror("Error reading admin input");
                close(sock);
                return -1;
            }

            // Remove the newline character from admin choice
            remove_newline(input);

            // Send admin choice to server
            send(sock, input, strlen(input), 0);

            // Reading the server's response
            valread = read(sock, buffer, MAX_BUFFER_SIZE);
            buffer[valread] = '\0'; // Null-terminate the string
            printf("%s", buffer);

            // Break the loop if the user logged out or exited
            if (atoi(input) == 5 || atoi(input) == 6) {
                break;
            }
        }
    }

    // Close the socket
    close(sock);
    return 0;
}
