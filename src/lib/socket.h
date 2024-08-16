#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/un.h>

void create_socket(int*, struct sockaddr_un*, char*);

void connect_socket(int, struct sockaddr_un*);

void bind_socket(int, struct sockaddr_un*);

void listen_socket(int);

int accept_connection(int);

int receive_message(int, char*, int);

void send_message(int, char*, int);

#endif
