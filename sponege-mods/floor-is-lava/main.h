// socket listener functions
#pragma once
#include <stdint.h>

// Creates new threads for incoming connections (gets piped into handle_packet)
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "./config.h"
#include "../logger/logger.c"

bool handle_packet(int socket_fd);

struct socket_listener {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt;
	int addrlen;
};

enum state {
	HANDSHAKE,
	STATUS,
} typedef state;

struct socket_listener;

void createSocketListener(struct socket_listener *sock, uint16_t port);
void getNewSocketConnection(struct socket_listener *sock);
void* listen_for_incoming_connections();