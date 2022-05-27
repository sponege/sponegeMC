// Prettifies your terminal output!

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "logger.h"
#include "../colors.h"

void log_print(const char* format, va_list args) {
	char buffer[1024] = {0};

	time_t rawtime = time(0);
	struct tm *ptm = localtime(&rawtime);
	strftime(buffer, 1024, "[%Y-%m-%d %r]", ptm);

	printf("%s: ", buffer);
	vprintf(format, args);
	printf("\n");
}

void log_info(const char* format, ...) {
	va_list args;
	va_start(args, format);

	log_print(format, args);

	va_end(args);
}

void log_error(const char* format, ...) {
	va_list args;
	va_start(args, format);

	printf("%s", RED);
	log_print(format, args);
	printf("%s", RESET);

	va_end(args);
}

void log_warn(const char* format, ...) {
	va_list args;
	va_start(args, format);

	printf("%s", YEL); // yellow
	log_print(format, args);
	printf("%s", RESET);

	va_end(args);
}