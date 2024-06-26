#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXCON 30

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }

    int port;
    if (strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    } else {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }

    int sockfd, new_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        error("Error on binding");
    }

    if (listen(sockfd, MAXCON) < 0) {
        close(sockfd);
        error("Error on listen");
    }

    printf("Server listening on port %d\n", port);

    sin_size = sizeof(client_addr);
    while ((new_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size)) >= 0) {
        printf("Connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));
        close(new_sockfd);
    }

    if (new_sockfd < 0) {
        close(sockfd);
        error("Error on accept");
    }

    close(sockfd);
    return 0;
}
