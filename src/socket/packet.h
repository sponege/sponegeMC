#pragma once
#include "main.h"
#include <stdbool.h>

char readByte(char** bufPtr);
int readVarInt(char** bufPtr);
char* readString(char** bufPtr);
void writeByte(int byte, int socket);
void writeVarInt(int value, int socket);
bool handle_packet(connection *connection);
