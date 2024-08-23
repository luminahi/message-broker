#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include "lib/queue.h"
#include "lib/socket.h"

void start_client(int socket_fd, int buffer_size) {
    struct pollfd pub;
    char buffer[buffer_size];

    pub.fd = STDIN_FILENO;
    pub.events = POLLIN;

    while (true) {
        int res = poll(&pub, 1, -1);
        if (res == -1) {
            perror("poll");
            break;
        }

        // check if the user has typed something and send trough a socket
        if (pub.revents & POLLIN) {
            if (fgets(buffer, buffer_size, stdin) != NULL) {
                send_message(socket_fd, buffer, strlen(buffer) - 1);
            }
        }
    }

    close(socket_fd);
}

void handshake(int socket_fd, char* topic) {
    send_message(socket_fd, topic, strlen(topic));
}

int main(int argc, char* argv[]) {
    int socket_fd;
    char* filepath = "msg.sock";
    const int buffer_size = 128;

    socket_fd = create_connection(filepath);
    handshake(socket_fd, argv[1] ? argv[1] : "*");
    start_client(socket_fd, buffer_size);

    return 0;
}
