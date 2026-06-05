#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <limits.h>

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

char **extract_packages(int argc, char **argv) {
    int count = argc - 2;

    char **dirs = malloc((count + 1) * sizeof(char *));

    for (int i = 0; i < count; i++) {
        dirs[i] = temp_extract(argv[i + 2]);
    }

    dirs[count] = NULL;

    return dirs;
}
