#include <stdio.h>

#include "packet.h"
#include "main.h"

#include "../logger/logger.h"

void handle_handshake(connection *connection, char **bufPtr) {
	unsigned int version = readVarInt(bufPtr);
	char* domain = readString(bufPtr);
	unsigned short port = (*(*bufPtr) << 8) + *(*bufPtr + 1);
	*bufPtr += 2;
	unsigned int next_state = readVarInt(bufPtr);

	log_info("Protocol Version: %u", version);
	log_info("Domain: %s", domain);
	log_info("Port: %hu", port);
	log_info("Next State: %u", next_state);

	switch (next_state) {
		case 1: connection->state = STATUS; break;
		//case 2: connection->state = STATE_LOGIN; break;
		default: log_error("Unknown next state: %u", next_state); break;
	}
}
