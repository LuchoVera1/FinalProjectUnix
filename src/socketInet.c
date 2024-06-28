#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define MAXCONN 30
#define BUF_SIZE 1024

int socket_fd;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nCtrl + C detected, closing socket and exiting the program\n");
        close(socket_fd);
        exit(0);
    }
}

void handle_client(int client_sock) {
    char buffer[BUF_SIZE];
    int n;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if ((n = recv(client_sock, buffer, sizeof(buffer)-1, 0)) < 0) {
            error("Error reading from socket");
        }
        if (n == 0) {
            printf("Client disconnected\n");
            break;
        }

        if (buffer[n-1] == '\n') {
            buffer[n-1] = '\0';
        }

        if (strcmp(buffer, "getInfo") == 0) {
            char *info_msg = "Luis Espinoza service, v0.1\n";
            send(client_sock, info_msg, strlen(info_msg), 0);
        } else if (strcmp(buffer, "getNumberOfPartitions") == 0) {
            FILE *fp = popen("lsblk -l | grep part | wc -l", "r");
            if (fp == NULL) {
                char *error_msg = "Error executing command\n";
                send(client_sock, error_msg, strlen(error_msg), 0);
            } else {
                while (fgets(buffer, sizeof(buffer)-1, fp) != NULL) {
                    send(client_sock, buffer, strlen(buffer), 0);
                }
                pclose(fp);
            }
        } else if (strcmp(buffer, "getCurrentKernelVersion") == 0) {
            FILE *fp = popen("uname -r", "r");
            if (fp == NULL) {
                char *error_msg = "Error executing command\n";
                send(client_sock, error_msg, strlen(error_msg), 0);
            } else {
                while (fgets(buffer, sizeof(buffer)-1, fp) != NULL) {
                    send(client_sock, buffer, strlen(buffer), 0);
                }
                pclose(fp);
            }
        } else if (strcmp(buffer, "sshdRunning") == 0) {
            FILE *fp = popen("ps ax | grep sshd | grep -v grep", "r");
            if (fp == NULL) {
                char *error_msg = "Error executing command\n";
                send(client_sock, error_msg, strlen(error_msg), 0);
            } else {
                if (fgets(buffer, sizeof(buffer)-1, fp) != NULL) {
                    send(client_sock, "true\n", 5, 0);
                } else {
                    send(client_sock, "false\n", 6, 0);
                }
                pclose(fp);
            }
        } else if (strcmp(buffer, "help") == 0) {
            char *help_msg = "\nAvailable commands:\n"
                             "  - getInfo\n"
                             "  - getNumberOfPartitions\n"
                             "  - getCurrentKernelVersion\n"
                             "  - sshdRunning\n"
                             "  - help\n";
            send(client_sock, help_msg, strlen(help_msg), 0);
        } else {
            char *error_msg = "Unknown command\n";
            send(client_sock, error_msg, strlen(error_msg), 0);
        }
    }

    close(client_sock);
}


int main(int argc, char *argv[]) {
    int port = 0;
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
                exit(1);
        }
    }


    if (port == 0) {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    int new_socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        error("Error opening socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        close(socket_fd);
        error("Error on binding");
    }

    if (listen(socket_fd, MAXCONN) < 0) {
        close(socket_fd);
        error("Error on listen");
    }

    printf("Server listening on port %d\n", port);

    sin_size = sizeof(client_addr);
    while ((new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sin_size)) >= 0) {
        printf("Connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));
        int pid = fork();
        if (pid < 0) {
            close(new_socket_fd);
            error("Error on fork");
        } else if (pid == 0) {
            close(socket_fd);
            handle_client(new_socket_fd);
        } else {
            close(new_socket_fd);
        }
    }

    close(socket_fd);
    return 0;
}
