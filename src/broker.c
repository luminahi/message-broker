#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include "lib/socket.h"
#include "lib/queue.h"

#define FILEPATH "msg.sock"

struct Client {
    int fd;
    char topic[32];
};

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

void handshake(int client_socket_fd, int* num_clients, struct pollfd* connections, struct Client* clients) {
    int index = (*num_clients) + 1;
    char buffer[32];

    connections[index].fd = client_socket_fd;
    connections[index].events = POLLIN;
    
    int num_bytes = receive_message(client_socket_fd, buffer, sizeof(buffer));
    buffer[num_bytes] = '\0';
    
    clients[index].fd = client_socket_fd;
    strncpy(clients[index].topic, buffer, num_bytes);

    (*num_clients)++;
}

void register_connection(int socket_fd, struct pollfd* connections, int* num_clients, int max_clients, struct Client* clients) {
    if (connections[0].revents & POLLIN) {
        int client_socket_fd = accept_connection(socket_fd);
        if (*num_clients < max_clients) {
            handshake(client_socket_fd, num_clients, connections, clients);
            printf("new client connected - total connections: %d\n", *num_clients);
        } else {
            printf("party is full\n");
            close(client_socket_fd);
        }
    }
}

void redirect_messages(struct pollfd* connections, int* num_clients, char* buffer, int buffer_size, struct Client* clients) {
    for (int i = 1; i <= *num_clients; i++) {
        if (connections[i].revents && POLLIN) {
            int num_bytes = receive_message(connections[i].fd, buffer, buffer_size);
            if (num_bytes <= 0) {
                // client disconnect or error
                close(connections[i].fd);
                printf("client disconnected.\n");
                connections[i] = connections[*num_clients];
                clients[i] = clients[*num_clients];
                (*num_clients)--;
                i--;
            } else {
                // forward the message to all other connections
                buffer[num_bytes] = '\0';
                for (int j = 1; j <= *num_clients; j++) {
                    if (
                        j != i && 
                        ((strcmp(clients[i].topic, clients[j].topic) == 0) || 
                        '*' == clients[j].topic[0])
                    ) {
                        send_message(connections[j].fd, buffer, num_bytes);
                    }
                }
            }
        }
    }
}

void start_server(char* filepath, int buffer_size, int max_clients) {
    char buffer[buffer_size];
    int socket_fd;
    int num_clients = 0;
    struct Client clients[max_clients];
    struct pollfd connections[max_clients];

    if (file_exists(filepath)) unlink(filepath);

    socket_fd = create_server(filepath);

    printf("waiting for connections.\n");

    connections[0].fd = socket_fd;
    connections[0].events = POLLIN;

    for (int i = 1; i < max_clients; i++) connections[i].fd = -1;

    signal(SIGINT, handle_sigint);

    // main loop to handle requests
    while (true) {
        // wait for an event on one of the sockets
        int poll_count = poll(connections, num_clients + 1, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // check if there is a new connection
        register_connection(socket_fd, connections, &num_clients, max_clients, clients);

        // check each client for incoming messages
        redirect_messages(connections, &num_clients, buffer, buffer_size, clients);
    }
}

int main(int argc, char* argv[]) {
    start_server(FILEPATH, 128, 4);

    return 0;
}
