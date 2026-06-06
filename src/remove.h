#ifndef REMOVE_H

#define REMOVE_H

#include <stddef.h>
#include <stdio.h>
#include "file-tools.h"
#include "msg.h"
#include "config.h"

int package_remove(char *package_name) {
	char remove_list_path[4096];
	snprintf(remove_list_path, sizeof(remove_list_path), "%s/%s.list",config_folder , package_name);
	char info_out_message[4096];
	snprintf(info_out_message, sizeof(info_out_message), "Removing package %s...", package_name);
	info(info_out_message);
	if(remove_list(remove_list_path) != 0) {
		printf("\x1b[0;31mERROR\x1b[0m\n");
		return 1;
	}
	if (unlink(remove_list_path) != 0) {
	    printf("\x1b[0;31mERROR\x1b[0m\n");
	    return 1;
	}
	printf("\x1b[0;32mDONE\x1b[0m\n");
	
	return 0;
}

int remove_packages(char **packages) {
	for (size_t i = 0; packages[i] != NULL; i++) {
		if(package_remove(packages[i]) != 0) {
			error("An error occurred while removing packages\n");
			return 1;
		}
		printf("\n");
	}
	info("Done removing packages\n\n");
}

#endif
