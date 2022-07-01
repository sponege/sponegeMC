#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	char *buf; // buffer pointer
	size_t size;
	FILE *stream;

	stream = open_memstream(&buf, &size);

	fprintf(stream, "Hello there! I'm Jordan!\n");
	fprintf(stream, "This is a file stream test, where my buffer is essentually limitless!\n");
	
	int a, b;
	printf("Provide me two numbers to add please!\n");

	scanf("%i", &a);
	scanf("%i", &b);

	fprintf(stream, "%i + %i is %i\n", a, b, a + b);

	fflush(stream);
	fclose(stream);

	printf("Printing buffered stream...\n");

	printf(buf);

	printf("Buffer size: %lu", size);

	free(buf);
}
