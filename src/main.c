#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

void usage(FILE *fd);
char *read_file(const char *filepath);

// ========================================
// main definition
// ========================================

int main(int argc, const char **argv) {
	if (argc <= 1) {
		usage(stderr);
		return 0;
	}

	const char *filepath = argv[1];
	char *src = read_file(filepath);

	printf("%s:\n%s\n", filepath, src);

	return 0;
}

// ========================================
// helper definition
// ========================================

void usage(FILE *fd) {
	fprintf(fd, "USAGE: ./smol [flags] <filename>\n");
	fprintf(fd, "\n");
	fprintf(fd, "FLAGS:\n");
	fprintf(fd, "\n");
	fprintf(fd, "MORE INFO:\n");
	fprintf(fd, "        - To read from stdin run as follows './smol -'\n");
	fprintf(fd, "\n");
}

char *read_file(const char *filepath) {
	FILE *fd = stdin;
	if (strcmp(filepath, "-") != 0) fd = fopen(filepath, "r");
	if (fd == NULL) {
		char buffer[1024] = {};
		snprintf(buffer, 1024, "Error opening '%s'", filepath);
		perror(buffer);
		exit(1);
	}

	int cap = 1024, len = 0;
	char *buffer = (char *) malloc(cap * sizeof(char));
	if (buffer == NULL) {
		perror("Error with malloc in read_file");
		exit(1);
	}

	for (;;) {
		int n = fread(buffer, 1, cap - len, fd);
		if (n == 0) {
			break;
		}

		len += n;
		if (len == cap) {
			cap *= 2;
			buffer = realloc(buffer, cap * sizeof(char));
			if (buffer == NULL) {
				perror("Error with realloc in read_file");
				exit(1);
			}
		}
	}

	buffer = realloc(buffer, len + 1);
	if (buffer == NULL) {
		perror("Error with realloc in read_file");
		exit(1);
	}
	buffer[len] = '\0';

	if (strcmp(filepath, "-") != 0) fclose(fd);
	return buffer;
}

