#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/un.h>

int create_server(char* filepath);

int create_connection(char* filepath);

int accept_connection(int server_socket_fd);

int receive_message(int socket_fd, char* buffer, int buffer_length);

void send_message(int socket_fd, char* message, int message_length);

#endif
