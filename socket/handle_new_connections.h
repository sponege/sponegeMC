// socket listener functions

struct socket_listener;

void createSocketListener(struct socket_listener *sock, uint16_t port);
int getNewSocketConnection(struct socket_listener *sock);
void listen_for_incoming_connections();

// functions you define in your own script
void *handle_socket(int socket);
