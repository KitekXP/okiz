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
#include "remove.h"
#include "metadata.h"
#include "file-tools.h"
#include "extract.h"
#include "config.h"

int print_packages_data(char **configs) {
    for (size_t i = 0; configs[i] != NULL; i++) {
        char *package_name = parse_metadata(configs[i], "Name");
        printf("%s ", package_name);
        free(package_name);
    }
    printf("\n");
    return 0;
}

int print_help() {
	printf("Usage:\n");
	printf("   okiz [action] [package/s]\n");
	printf("\nActions:\n");
	printf("   install - Installs a package\n");
	printf("   remove  - Removes a package\n");
	printf("   help    - Prints this message\n");
	return 0;
}

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("\x1b[0;31mERROR\x1b[0m: Too few arguments\n");
		print_help();
		return 1;
	}

	if(!strcmp(argv[1], "install")) {
		if(argc < 3) {
			printf("\x1b[0;31mERROR\x1b[0m: Too few arguments\n");
			print_help();
			return 1;
		}
		char **dirs = extract_packages(argc, argv);
		if(install_packages(dirs) != 0) { return 1; }
		return 0;
	} if(!strcmp(argv[1], "remove")) {
		if(argc < 3) {
			printf("\x1b[0;31mERROR\x1b[0m: Too few arguments\n");
			print_help();
			return 1;
		}
		if(remove_packages(argv + 2) != 0) { return 1; }
		return 0;
	} else {
		print_help();
		return 0;
	}
	
	return 0;
}
