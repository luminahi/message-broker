#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "socket.h"

int create_server(char* filepath) {
    struct sockaddr_un addr;
    int socket_fd;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, filepath, strlen(filepath));

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

int create_connection(char* filepath) {
    struct sockaddr_un addr;
    int socket_fd;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, filepath, strlen(filepath));

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
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
    int res = recv(socket_fd, buffer, buffer_length, 0);
    if (res == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    return res;
}

void send_message(int socket_fd, char* message, int message_length) {
    if (send(socket_fd, message, message_length, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}
