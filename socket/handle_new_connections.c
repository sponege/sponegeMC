// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "../config.h"
#include "handle_new_connections.h"

#define true 1
#define false 0

struct socket_listener {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt;
	int addrlen;
};

void createSocketListener(struct socket_listener *sock, uint16_t port) {
	// Creating socket file descriptor

	sock->server_fd = 0;
	sock->new_socket = 0;
	sock->opt = 1;
	sock->addrlen = sizeof(sock->address);


	if ((sock->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		exit(1);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(sock->server_fd, SOL_SOCKET,
								SO_REUSEADDR | SO_REUSEPORT, &sock->opt,
								sizeof(sock->opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	sock->address.sin_family = AF_INET;
	sock->address.sin_addr.s_addr = INADDR_ANY;
	sock->address.sin_port = htons(port);

	// Forcefully attaching socket to the port 8080
	if (bind(sock->server_fd, (struct sockaddr*)&sock->address,
						sizeof(sock->address))
			< 0) {
			perror("bind failed");
			exit(EXIT_FAILURE);
	}
	if (listen(sock->server_fd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
	}
}

int getNewSocketConnection(struct socket_listener *sock) {
		sock->new_socket
				= accept(sock->server_fd, (struct sockaddr*)&sock->address,
								(socklen_t*)&sock->addrlen);
}

void listen_for_incoming_connections() {
	struct socket_listener sock;
	pthread_t tid;

	createSocketListener(&sock, 1337);


	while (true) {
		getNewSocketConnection(&sock);

		if (sock.new_socket) {
			pthread_create(&tid, NULL, handle_socket, sock.new_socket);
		} else if (sock.new_socket < 0) {
			puts("error lol");
		}

	}

	shutdown(sock.server_fd, SHUT_RDWR); // never reached but good to know
}
