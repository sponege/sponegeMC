#include "./daniiMC.h"
#include "./main.h"

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
		log_error("setsockopt");
		exit(EXIT_FAILURE);
	}

	sock->address.sin_family = AF_INET;
	sock->address.sin_addr.s_addr = INADDR_ANY;
	sock->address.sin_port = htons(port);

	// Forcefully attaching socket to the port 8080
	if (bind(sock->server_fd, (struct sockaddr*)&sock->address,
						sizeof(sock->address))
			< 0) {
			log_error("bind failed");
			exit(EXIT_FAILURE);
	}
	if (listen(sock->server_fd, 3) < 0) {
			log_error("listen");
			exit(EXIT_FAILURE);
	}
}

void getNewSocketConnection(struct socket_listener *sock) {
		sock->new_socket
				= accept(sock->server_fd, (struct sockaddr*)&sock->address,
								(socklen_t*)&sock->addrlen);
}

void *handle_connection(void *arg) {
	int socket_fd = *((int*) arg);
	log_info("New connection! Socket descriptor: %i", socket_fd);

	while (handle_packet(socket_fd)) {}

	log_info("Closing connection... Socket descriptor: %i", socket_fd);
	// close(connection.socket);
}

void* listen_for_incoming_connections() {
	struct socket_listener sock;
	pthread_t tid;

	createSocketListener(&sock, 1337);


	while (true) {
		getNewSocketConnection(&sock);

		if (sock.new_socket) {
			pthread_create(&tid, NULL, handle_connection, &sock.new_socket);
		} else if (sock.new_socket < 0) {
			log_error("error lol");
		}

	}

	shutdown(sock.server_fd, SHUT_RDWR); // never reached but good to know
}

int main(int argc, char const* argv[]) {
	log_info("Starting socket listener...");
	listen_for_incoming_connections();
}

bool handle_packet(int socket_fd) {
	
}