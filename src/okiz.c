#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <archive.h>
#include <archive_entry.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "install.h"
#include "metadata.h"
#include "file-tools.h"
#include "extract.h"
#include "config.h"

int print_packages_data(char **configs) {
    for (size_t i = 0; configs[i] != NULL; i++) {
        char *packagename = parse_metadata(configs[i], "Name");
        printf("%s ", packagename);
    }
    printf("\n");
    return 0;
}


int main(int argc, char **argv) {
	if(argc < 2) {
		printf("\x1b[0;31mERROR\x1b[0m: Too few arguments\n");
		// Add print_help() here
		return 1;
	}

	char **dirs = extract_packages(argc, argv);
	install_packages(dirs);
	
	return 0;
}
