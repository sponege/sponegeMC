#include <time.h>
#include <stdio.h>
#include "handle_new_connections.c"

void *handle_socket(int socket) {
	char buffer[1024] = {0};

	time_t rawtime = time(0);
	struct tm *ptm = localtime(&rawtime);
	strftime(buffer, 1024, "%Y-%m-%d %r", ptm);

	printf("Socket created! Socket ID: %d", socket);
	sprintf(buffer, "%s\nPress enter for the time!\n", buffer);
	send(socket, buffer, strlen(buffer), 0);

	while (recv(socket, buffer, 1, 0) > 0) {
		rawtime = time(0);
		ptm = localtime(&rawtime);
		strftime(buffer, 1024, "%Y-%m-%d %r", ptm);

		sprintf(buffer, "%s\nPress enter for the time!\n", buffer);
		send(socket, buffer, strlen(buffer), 0);
	}
}

int main(int argc, char const* argv[]) {
	pthread_t tid;

	pthread_create(&tid, NULL, listen_for_incoming_connections, 0);

	while (1) {}
}
