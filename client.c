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
    str[strcspn(str, "\n")] = '\0';  // Removes the first newline character if found
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;  // IPv4
    serv_addr.sin_port = htons(PORT); // Port

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Loop to receive messages from the server
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, MAX_BUFFER_SIZE);
        buffer[valread] = '\0'; // Null-terminate the string
        printf("%s", buffer); // Print the received message

        // Input to send back to the server
        if (strcmp(buffer, "Invalid choice. Please try again.\n") == 0 ||
            strcmp(buffer, "You have chosen Employee.\n") == 0 ||
            strcmp(buffer, "You have chosen Manager.\n") == 0) {
            continue; // Skip if the last message was an error or confirmation
        }

        char input[MAX_BUFFER_SIZE];
        fgets(input, sizeof(input), stdin);
        remove_newline(input); // Clean the input

        // Send user input back to the server
        send(sock, input, strlen(input), 0);
    }

    return 0;
}
