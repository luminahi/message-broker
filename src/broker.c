#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include "lib/socket.h"
#include "lib/queue.h"

#define FILEPATH "msg.sock"
#define BUFFER_SIZE 64
#define MAX_CLIENTS 4

int file_exists(const char* file_path) {
    struct stat file_stat;
    return (stat(file_path, &file_stat) == 0);
}

void handle_sigint(int sig) {
    printf("\ncleaning up...\n");
    unlink(FILEPATH);
    exit(EXIT_SUCCESS);
}

void start_server(char* file_path, int buffer_size) {
    char buffer[buffer_size];
    
    int server_socket_fd;
    int client_socket_fd;
    int num_clients = 0;
    struct sockaddr_un address;
    struct pollfd clients[MAX_CLIENTS];

    if (file_exists(file_path)) unlink(file_path);

    create_socket(&server_socket_fd, &address, file_path);
    bind_socket(server_socket_fd, &address);
    listen_socket(server_socket_fd);

    printf("server bound to %s\nwaiting for connections...\n", file_path);

    signal(SIGINT, handle_sigint);

    clients[0].fd = server_socket_fd;
    clients[0].events = POLLIN;

    for (int i = 1; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
    }

    // main loop to handle requests
    while (true) {
        // wait for an event on one of the sockets
        int poll_count = poll(clients, num_clients + 1, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // check if there is a new connection
        if (clients[0].revents & POLLIN) {
            client_socket_fd = accept_connection(server_socket_fd);
            if (num_clients < MAX_CLIENTS) {
                clients[num_clients + 1].fd = client_socket_fd;
                clients[num_clients + 1].events = POLLIN;
                num_clients++;
                printf("new client connected. Total clients: %d\n", num_clients);
            } else {
                printf("party is full\n");
                close(client_socket_fd);
            }
        }

        // check each client for incoming messages
        for (int i = 1; i <= num_clients; i++) {
            if (clients[i].revents && POLLIN) {
                int num_bytes = receive_message(clients[i].fd, buffer, BUFFER_SIZE);
                if (num_bytes <= 0) {
                    // client disconnect or error
                    close(clients[i].fd);
                    printf("client disconnected.\n");
                    clients[i] = clients[num_clients];
                    num_clients--;
                    i--;
                } else {
                    // forward the message to all other clients
                    buffer[num_bytes] = '\0';
                    for (int j = 1; j <= num_clients; j++) {
                        if (j != i)
                            send_message(clients[j].fd, buffer, num_bytes);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    start_server(FILEPATH, BUFFER_SIZE);

    return 0;
}
