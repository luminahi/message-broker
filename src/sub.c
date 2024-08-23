#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include "lib/queue.h"
#include "lib/socket.h"

void start_client(int socket_fd, int buffer_size) {
    struct pollfd sub;
    char buffer[buffer_size];

    sub.fd = socket_fd;
    sub.events = POLLIN;

    while (true) {
        int res = poll(&sub, 1, -1);
        if (res == -1) {
            perror("poll");
            break;
        }

        if (sub.revents & POLLIN) {
            int num_bytes = receive_message(sub.fd, buffer, buffer_size);
            if (num_bytes > 0) {
                buffer[num_bytes] = '\0';
                printf("[received]: %s\n", buffer);
            } else if (num_bytes == 0) {
                printf("server disconnected.");
                break;
            } else {
                perror("recv");
                break;
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
