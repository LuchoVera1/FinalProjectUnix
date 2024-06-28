#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 1024

int socket_fd; 

void sigint_handler() {
    printf("\nCtrl+C detected. Closing connection and exiting.\n");
    close(socket_fd);
    exit(0);
}

void init_connection(char *hostname, int port) {
    struct sockaddr_in server_addr;
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "No such host\n");
        exit(1);
    }

    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting");
        exit(1);
    }

    printf("Connected to server.\n");
}

void communicate_with_server() {
    char buffer[BUF_SIZE];
    int n;

    signal(SIGINT, sigint_handler);

    while (1) {
        printf("Enter command: ");
        fgets(buffer, BUF_SIZE, stdin);

        if (send(socket_fd, buffer, strlen(buffer), 0) < 0) {
            perror("Error writing to socket");
            break;
        }


        memset(buffer, 0, BUF_SIZE);
        if ((n = recv(socket_fd, buffer, BUF_SIZE-1, 0)) < 0) {
            perror("Error reading from socket");
            break;
        }


        printf("Server response: %s", buffer);
    }

    close(socket_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    int port = atoi(argv[2]);

    init_connection(hostname, port);

    communicate_with_server();

    return 0;
}

