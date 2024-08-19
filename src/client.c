#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "lib/queue.h"
#include "lib/socket.h"

void start_client(int socket_fd, int buffer_size) {
    struct pollfd fds[2];
    char buffer[buffer_size];

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
            int num_bytes = receive_message(socket_fd, buffer, buffer_size);
            if (num_bytes > 0) {
                buffer[num_bytes] = '\0';
                printf("[received]: %s\n", buffer);
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
            if (fgets(buffer, buffer_size, stdin) != NULL) {
                send_message(socket_fd, buffer, strlen(buffer) - 1);
            }
        }
    }

    close(socket_fd);
}

int main(int argc, char* argv[]) {
    int socket_fd;
    char* filepath = "msg.sock";

    socket_fd = create_connection(filepath);
    start_client(socket_fd, 128);

    return 0;
}
