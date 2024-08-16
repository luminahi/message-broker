#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"

void create_socket(int* socket_fd, struct sockaddr_un* address, char* path) {
    *socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (*socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(address, 0, sizeof(struct sockaddr_un));
    address->sun_family = AF_UNIX;
    strncpy(address->sun_path, path, sizeof(address->sun_path) - 1);
}

void connect_socket(int socket_fd, struct sockaddr_un* address) {
    if (connect(socket_fd, (struct sockaddr*)address, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

void bind_socket(int socket_fd, struct sockaddr_un* address) {
    if (bind(socket_fd, (struct sockaddr*)address, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int socket_fd) {
    if (listen(socket_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

int accept_connection(int server_socket_fd) {
    int client_socket_fd = accept(server_socket_fd, NULL, NULL);
    if (client_socket_fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return client_socket_fd;
}

int receive_message(int socket_fd, char* buffer, int buffer_length) {
    return recv(socket_fd, buffer, buffer_length, 0);
}

void send_message(int socket_fd, char* message, int message_length) {
    if (send(socket_fd, message, message_length, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}
