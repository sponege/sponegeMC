#include <sys/socket.h>

#include "main.h"
#include "packet.h"
#include "../logger/logger.h"

// unsigned char json[] = {
//   0x7b, 0x0a, 0x20, 0x20, 0x22, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e,
//   0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x61,
//   0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x31, 0x2e, 0x38, 0x2e, 0x37, 0x22,
//   0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x72, 0x6f, 0x74, 0x6f,
//   0x63, 0x6f, 0x6c, 0x22, 0x3a, 0x20, 0x34, 0x37, 0x0a, 0x20, 0x20, 0x7d,
//   0x2c, 0x0a, 0x20, 0x20, 0x22, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73,
//   0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x61,
//   0x78, 0x22, 0x3a, 0x20, 0x31, 0x30, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20,
//   0x20, 0x22, 0x6f, 0x6e, 0x6c, 0x69, 0x6e, 0x65, 0x22, 0x3a, 0x20, 0x35,
//   0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x61, 0x6d, 0x70, 0x6c,
//   0x65, 0x22, 0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
//   0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e,
//   0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x74, 0x68, 0x69, 0x6e, 0x6b,
//   0x6f, 0x66, 0x64, 0x65, 0x61, 0x74, 0x68, 0x22, 0x2c, 0x0a, 0x20, 0x20,
//   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x69, 0x64, 0x22, 0x3a, 0x20,
//   0x22, 0x34, 0x35, 0x36, 0x36, 0x65, 0x36, 0x39, 0x66, 0x2d, 0x63, 0x39,
//   0x30, 0x37, 0x2d, 0x34, 0x38, 0x65, 0x65, 0x2d, 0x38, 0x64, 0x37, 0x31,
//   0x2d, 0x64, 0x37, 0x62, 0x61, 0x35, 0x61, 0x61, 0x30, 0x30, 0x64, 0x32,
//   0x30, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20,
//   0x20, 0x20, 0x20, 0x5d, 0x0a, 0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20,
//   0x22, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e,
//   0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x74, 0x65,
//   0x78, 0x74, 0x22, 0x3a, 0x20, 0x22, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
//   0x77, 0x6f, 0x72, 0x6c, 0x64, 0x22, 0x0a, 0x20, 0x20, 0x7d, 0x2c, 0x0a,
//   0x20, 0x20, 0x22, 0x66, 0x61, 0x76, 0x69, 0x63, 0x6f, 0x6e, 0x22, 0x3a,
//   0x20, 0x22, 0x64, 0x61, 0x74, 0x61, 0x3a, 0x69, 0x6d, 0x61, 0x67, 0x65,
//   0x2f, 0x70, 0x6e, 0x67, 0x3b, 0x62, 0x61, 0x73, 0x65, 0x36, 0x34, 0x2c,
//   0x3c, 0x64, 0x61, 0x74, 0x61, 0x3e, 0x22, 0x0a, 0x7d, 0x0a
// };
// unsigned int json_len = 334;

#include <stdlib.h>
#include <stdio.h>

void handle_status_request(connection *connection, char **bufPtr) {
  FILE *file;
  int fs; // file size
  int stream_size;
  file = fopen("status.json", "r");
  
  if (file == NULL) {
    puts("Error opening file, this is either because the file doesn't exist or you don't have permission to read the requested file.");
    return; // error opening file
  }

  fseek(file, 0L, SEEK_END);
  fs = ftell(file);
  rewind(file);

  if (fs == -1) {
    puts("That's a directory lol");
    return; // a directory was provided instead of a file
  }

  char *json = malloc(fs * sizeof(char)); // string with file contents
  fread(json, sizeof(char), fs, file); // read file into string

  log_info("Writing json... %s", json);
  // https://wiki.vg/Protocol#Packet_format
  char *bp;
  size_t size;
  FILE *stream;

  stream = open_memstream(&bp, &size);

  writeVarInt(fs, stream); // Packet Length
  writeVarInt(0, stream); // Packet ID
  writeVarInt(fs, stream); // String Length (JSON)
	fprintf(stream, json);

  fseek(stream, 0L, SEEK_END);
  stream_size = ftell(stream);
  rewind(stream);

  send(connection->socket, bp, stream_size, 0);

  char *buf[1024] = {0};
  recv(connection->socket, buf, 10, 0);
  for (int i = 0; i < 10; i++) {
    log_warn("%02x", buf[i]);
  }
}
