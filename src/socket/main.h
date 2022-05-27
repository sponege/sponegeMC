// socket listener functions
#pragma once
#include <stdint.h>

enum state {
	HANDSHAKE,
	STATUS,
} typedef state;

struct connection {
	int socket;
	state state;
} typedef connection;

struct socket_listener;

void createSocketListener(struct socket_listener *sock, uint16_t port);
int getNewSocketConnection(struct socket_listener *sock);
void* listen_for_incoming_connections();
