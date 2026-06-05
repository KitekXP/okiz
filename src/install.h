#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include "metadata.h"
#include "extract.h"
#include "dircp.h"
#include "config.h"

int gen_file_list(const char *path, const char *outfile) {
    char abs_base[PATH_MAX];

    if (!realpath(path, abs_base)) {
        return -1;
    }

    FILE *out = fopen(outfile, "w");
    if (!out) return -1;

    DIR *dir = opendir(abs_base);
    if (!dir) {
        fclose(out);
        return -1;
    }

    struct dirent *ent;
    char full[PATH_MAX];

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.' &&
            (ent->d_name[1] == '\0' ||
            (ent->d_name[1] == '.' && ent->d_name[2] == '\0')))
            continue;

        snprintf(full, sizeof(full), "%s/%s", abs_base, ent->d_name);
        fprintf(out, "%s\n", full);
    }

    closedir(dir);
    fclose(out);
    return 0;
}

int package_list(char *package_location) {
	char metadata_loc[PATH_MAX];
	snprintf(metadata_loc, sizeof(metadata_loc), "%s/package/metadata.conf", package_location);
	char * package_name = parse_metadata(metadata_loc, "Name");
	char list_message[4096];
	snprintf(list_message, sizeof(list_message), "\x1b[0;35mINFO\x1b[0m: Generating a file list of %s...", package_name);
	printf(list_message);
	char extract_src[PATH_MAX];
	snprintf(extract_src, sizeof(extract_src), "%s/package/files", package_location);
	char file_list[PATH_MAX];
	snprintf(file_list, sizeof(file_list), "%s/%s", config_folder, package_name);
	if(gen_file_list(extract_src, file_list) != 0) {
		printf("\x1b[0;31mERROR\x1b[0m\n");
		return 1;
	}

	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int package_install(char *package_location) {
	char metadata_loc[PATH_MAX];
	snprintf(metadata_loc, sizeof(metadata_loc), "%s/package/metadata.conf", package_location);
	char * package_name = parse_metadata(metadata_loc, "Name");
	char install_message[4096];
	snprintf(install_message, sizeof(install_message), "\x1b[0;35mINFO\x1b[0m: Copying files for %s...", package_name);
	printf(install_message);
	char extract_src[PATH_MAX];
	snprintf(extract_src, sizeof(extract_src), "%s/package/files", package_location);
	if(copy_dir(extract_src, "/") != 0) {
		printf("\x1b[0;31mERROR\x1b[0m\n");
		return 1;
	}
	
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int package_postinstall(char *package_location) {
	char metadata_loc[PATH_MAX];
	snprintf(metadata_loc, sizeof(metadata_loc), "%s/package/metadata.conf", package_location);
	char * package_name = parse_metadata(metadata_loc, "Name");
	char post_message[4096];
	snprintf(post_message, sizeof(post_message), "\x1b[0;35mINFO\x1b[0m: Executing postinstall for %s...", package_name);
	printf(post_message);
	char postinstall_path[PATH_MAX];
	snprintf(postinstall_path, sizeof(postinstall_path), "%s/package/postinstall", package_location);
	if(system(postinstall_path) != 0) {
		printf("\x1b[0;31mERROR\x1b[0m\n");
		return 1;
	}
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int package_preinstall(char *package_location) {
	char metadata_loc[PATH_MAX];
	snprintf(metadata_loc, sizeof(metadata_loc), "%s/package/metadata.conf", package_location);
	char * package_name = parse_metadata(metadata_loc, "Name");
	char pre_message[4096];
	snprintf(pre_message, sizeof(pre_message), "\x1b[0;35mINFO\x1b[0m: Executing preinstall for %s...", package_name);
	printf(pre_message);
	char preinstall_path[PATH_MAX];
	snprintf(preinstall_path, sizeof(preinstall_path), "%s/package/preinstall", package_location);
	if(system(preinstall_path) != 0) {
		printf("\x1b[0;31mERROR\x1b[0m\n");
		return 1;
	}
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int install_packages(char **package_locations) {
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		if(package_preinstall(package_locations[i]) != 0) {
			printf("\x1b[0;31mERROR\x1b[0m: An error occurred while executing preinstall\n");
			return 1;
		}
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done executing preinstall scripts\n\n");
	
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		if(package_install(package_locations[i]) != 0) {
			printf("\x1b[0;31mERROR\x1b[0m: An error occurred while generating a list of files\n");
			return 2;
		}
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done generating a list of files\n\n");
	
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		if(package_install(package_locations[i]) != 0) {
			printf("\x1b[0;31mERROR\x1b[0m: An error occurred while copying files\n");
			return 3;
		}
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done copying files for packages\n\n");
	
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		if(package_postinstall(package_locations[i]) != 0) {
			printf("\x1b[0;31mERROR\x1b[0m: An error occurred while executing postinstall\n");
			return 4;
		}
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done postinstall scripts\n\n");
	return 0;
}
