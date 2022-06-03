// Handles an incoming socket conn

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include "handshake.h"
#include "status.h"

#include "../logger/logger.h"

#define SEGMENT_BITS 0b01111111
#define CONTINUE_BIT 0b10000000

#define MAX_VARINT_LEN 5

char readByte(char** bufPtr) {
	char byte = **bufPtr;
	*bufPtr += 1;
	return byte;
}

int readVarInt(char** bufPtr) {
	int value = 0;
  int position = 0;
  char currentByte;

  while (true) {
	  currentByte = readByte(bufPtr);

	  value |= (currentByte & SEGMENT_BITS) << position;

		if ((currentByte & CONTINUE_BIT) == 0) break;

	  position += 7;

	  if (position >= 32) {
			log_error("Invalid VarInt sent! Disconnecting socket...");
			pthread_exit(0);
		};
  }

  return value;
}

char* readString(char** bufPtr) {
	unsigned int length = readVarInt(bufPtr);
	char* str = malloc(length);
	memcpy(str, *bufPtr, length);
	*bufPtr += length;
	return str;
}

void writeByte(int byte, int fd) {
	fputc(fd, byte);
}

void writeVarInt(int value, int fd) {
	while (true) {
		if ((value & ~SEGMENT_BITS) == 0) {
			writeByte(value, socket);
			return;
		}

		writeByte((value & SEGMENT_BITS) | CONTINUE_BIT, fd);

		// Note: >>> means that the sign bit is shifted with the rest of the number rather than being left alone
		value = value >> 7;
	}
}

bool handle_packet(connection* conn) {
	char* buffer = malloc(MAX_VARINT_LEN);
	char* bufPtr = buffer;
	bool success = true;

	if (recv(conn->socket, bufPtr, MAX_VARINT_LEN, 0) == MAX_VARINT_LEN) {
		int packet_length = readVarInt(&bufPtr);
		int packet_length_length = bufPtr - buffer;
		buffer = realloc(buffer, packet_length_length + packet_length);
		bufPtr = buffer + packet_length_length;

		if (recv(conn->socket, buffer + MAX_VARINT_LEN, packet_length, 0) <= 0) {
			log_error("bruh aint no wayy");
			success = false;
			goto end;
		}

		unsigned int packet_id = readVarInt(&bufPtr);
		log_info("Packet Length: %u", packet_length);
		log_info("Packet ID: %u", packet_id);
		switch (conn->state) {
			case HANDSHAKE:
				switch (packet_id) {
					case 0: handle_handshake(conn, &bufPtr); handle_status_request(conn, &bufPtr); break;
					default:
						log_error("Invalid packet ID: %i", packet_id);
						success = false;
						goto end;
				}
				break;
			case STATUS:
				switch (packet_id) {
					case 0: handle_status_request(conn, &bufPtr); break;
					default:
						log_error("Invalid packet ID: %i", packet_id);
						success = false;
						goto end;
				}
				break;
		}
	} else {
		success = false;
	}

end:
	free(buffer);
	return success;
}
