#include "logger.h"

int main() {
	log_info("Hello, world!");
	log_info("1 + 1 = %i", 1 + 1);
	log_warn("WARNING: YOU ARE ON THE VERGE OF BECOMING A CATBOY");
	log_error("ERROR: CATBOY ALERT");
}