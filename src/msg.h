#ifndef MSG_H

#define MSG_H

#include <stdio.h>

int info(char *message) {
	char out_message[4096];
	snprintf(out_message, sizeof(out_message), "\x1b[0;35mINFO\x1b[0m: %s", message);
	printf(out_message);
}

int error(char *message) {
	char out_message[4096];
	snprintf(out_message, sizeof(out_message), "\x1b[0;31mINFO\x1b[0m: %s", message);
	printf(out_message);
}

#endif
