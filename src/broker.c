#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include "lib/socket.h"
#include "lib/queue.h"

#define FILEPATH "sock.msg"

bool file_exists(const char* file_path) {
    struct stat file_stat;
    return (stat(file_path, &file_stat) == 0);
}

void handle_sigint(int sig) {
    if (!file_exists(FILEPATH)) exit(EXIT_FAILURE);
    printf("unlinking %s and exiting.", FILEPATH);
    unlink(FILEPATH);
    exit(EXIT_SUCCESS);
}

void register_connection(int socket_fd, struct pollfd* clients, int* num_clients, int max_clients) {
    if (clients[0].revents & POLLIN) {
        int client_socket_fd = accept_connection(socket_fd);
        if (*num_clients < max_clients) {
            clients[(*num_clients) + 1].fd = client_socket_fd;
            clients[(*num_clients) + 1].events = POLLIN;
            (*num_clients)++;
            printf("new client connected - total clients: %d\n", *num_clients);
        } else {
            printf("party is full\n");
            close(client_socket_fd);
        }
    }
}

void redirect_messages(struct pollfd* clients, int* num_clients, char* buffer, int buffer_size) {
    for (int i = 1; i <= *num_clients; i++) {
        if (clients[i].revents && POLLIN) {
            int num_bytes = receive_message(clients[i].fd, buffer, buffer_size);
            if (num_bytes <= 0) {
                // client disconnect or error
                close(clients[i].fd);
                printf("client disconnected.\n");
                clients[i] = clients[*num_clients];
                (*num_clients)--;
                i--;
            } else {
                // forward the message to all other clients
                buffer[num_bytes] = '\0';
                for (int j = 1; j <= *num_clients; j++) {
                    if (j != i)
                        send_message(clients[j].fd, buffer, num_bytes);
                }
            }
        }
    }
}

void start_server(char* filepath, int buffer_size, int max_clients) {
    char buffer[buffer_size];
    int socket_fd;
    int num_clients = 0;
    struct pollfd clients[max_clients];

    if (file_exists(filepath)) unlink(filepath);

    socket_fd = create_server(filepath);

    printf("waiting for connections.\n");

    clients[0].fd = socket_fd;
    clients[0].events = POLLIN;

    for (int i = 1; i < max_clients; i++) clients[i].fd = -1;

    signal(SIGINT, handle_sigint);

    // main loop to handle requests
    while (true) {
        // wait for an event on one of the sockets
        int poll_count = poll(clients, num_clients + 1, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // check if there is a new connection
        register_connection(socket_fd, clients, &num_clients, max_clients);

        // check each client for incoming messages
        redirect_messages(clients, &num_clients, buffer, buffer_size);
    }
}

int main(int argc, char* argv[]) {
    start_server(FILEPATH, 128, 4);

    return 0;
}
