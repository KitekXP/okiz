#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <archive.h>
#include <archive_entry.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dircp.h"

char *temp_extract(char * archive) {
	char dir[] = "/tmp/okiz-XXXXXX";
	
	if (mkdtemp(dir) == NULL) {
	    perror("mkdtemp");
	    return NULL;
	}

	struct archive *a = archive_read_new();
	archive_read_support_format_tar(a);
	archive_read_support_filter_all(a);

	archive_read_open_filename(a, archive, 10240);

	struct archive_entry *entry;

	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
	    const char *path = archive_entry_pathname(entry);

 	   char newpath[PATH_MAX];
 	   snprintf(newpath, sizeof(newpath), "%s/%s", dir, path);

	    archive_entry_set_pathname(entry, newpath);

	    archive_read_extract(a, entry,
	        ARCHIVE_EXTRACT_TIME |
 	       ARCHIVE_EXTRACT_PERM);
	}

	archive_read_free(a);

	return strdup(dir);
}

char *parse_metadata(char *file, char *keyinput) {
	FILE *f = fopen(file, "r");
	if (!f) return NULL;

	char line[512];

	while (fgets(line, sizeof(line), f)) {
	    char *eq = strchr(line, ':');
	    if (!eq) continue;

	    *eq = '\0';

	    char *key = line;
	    char *val = eq + 1;

	    key[strcspn(key, "\n")] = 0;
	    val[strcspn(val, "\n")] = 0;
	    
	    if(!strcmp(key, keyinput)) {
	    	fclose(f);
	    	return strdup(val);
	    }
	}

	fclose(f);
	return NULL;
}



int print_packages_data(char **configs) {
    for (size_t i = 0; configs[i] != NULL; i++) {
        char *packagename = parse_metadata(configs[i], "Name");
        printf("%s ", packagename);
    }
    printf("\n");
    return 0;
}

int package_install(char *package_location) {
	char install_message[4096];
	snprintf(install_message, sizeof(install_message), "\x1b[0;35mINFO\x1b[0m: Copying files for %s...", package_location);
	printf(install_message);
	char extract_src[PATH_MAX];
	snprintf(extract_src, sizeof(extract_src), "%s/package/files", package_location);
	copy_dir(extract_src, "/");
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int package_postinstall(char *package_location) {
	char post_message[4096];
	snprintf(post_message, sizeof(post_message), "\x1b[0;35mINFO\x1b[0m: Executing postinstall for %s...", package_location);
	printf(post_message);
	char postinstall_path[PATH_MAX];
	snprintf(postinstall_path, sizeof(postinstall_path), "%s/package/postinstall", package_location);
	system(postinstall_path);
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int package_preinstall(char *package_location) {
	char pre_message[4096];
	snprintf(pre_message, sizeof(pre_message), "\x1b[0;35mINFO\x1b[0m: Executing preinstall for %s...", package_location);
	printf(pre_message);
	char preinstall_path[PATH_MAX];
	snprintf(preinstall_path, sizeof(preinstall_path), "%s/package/preinstall", package_location);
	system(preinstall_path);
	printf("\x1b[0;32mDONE\x1b[0m\n");
	return 0;
}

int install_packages(char **package_locations) {
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		package_preinstall(package_locations[i]);
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done executing preinstall scripts\n\n");
	
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		package_install(package_locations[i]);
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done copying files for packages\n\n");
	
	for (size_t i = 0; package_locations[i] != NULL; i++) {
		package_postinstall(package_locations[i]);
		printf("\n");
	}
	printf("\x1b[0;35mINFO\x1b[0m: Done postinstall scripts\n\n");
	return 0;
}

char **extract_packages(int argc, char **argv) {
    int count = argc - 2;

    char **dirs = malloc((count + 1) * sizeof(char *));

    for (int i = 0; i < count; i++) {
        dirs[i] = temp_extract(argv[i + 2]);
    }

    dirs[count] = NULL;

    return dirs;
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
