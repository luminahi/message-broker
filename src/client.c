#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include "lib/queue.h"
#include "lib/socket.h"
#include <sys/socket.h>
#include <sys/un.h>

#define FILEPATH "msg.sock"
#define BUFFER_SIZE 64

int main(int argc, char* argv[]) {
    int socket_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[2];

    create_socket(&socket_fd, &address, FILEPATH);
    connect_socket(socket_fd, &address);

    printf("connected to the message broker.\n");

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    while (true) {
        int ret = poll(fds, 2, -1);
        if (ret == -1) {
            perror("poll");
            break;
        }

        // check if there is data from broker
        if (fds[0].revents & POLLIN) {
            int num_bytes = receive_message(socket_fd, buffer, BUFFER_SIZE);
            if (num_bytes > 0) {
                buffer[num_bytes] = '\0';
                printf("received: %s\n", buffer);
            } else if (num_bytes == 0) {
                printf("server disconnected.\n");
                break;
            } else {
                perror("recv");
                break;
            }
        }

        // check if the user has typed something
        if (fds[1].revents & POLLIN) {
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
                send_message(socket_fd, buffer, strlen(buffer) - 1);
            }
        }
    }

    close(socket_fd);
    return 0;
}
