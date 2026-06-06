#ifndef METADATA_H

#define METADATA_H

#include <stdio.h>
#include <string.h>

char *parse_metadata(const char *file, const char *keyinput) {
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

#endif
