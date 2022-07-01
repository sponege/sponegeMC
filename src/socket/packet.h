#pragma once
#include "main.h"
#include <stdbool.h>
#include <stdio.h>

char readByte(char** bufPtr);
int readVarInt(char** bufPtr);
char* readString(char** bufPtr);
void writeByte(int byte, FILE* fd);
void writeVarInt(int value, FILE* fd);
bool handle_packet(connection *connection);
