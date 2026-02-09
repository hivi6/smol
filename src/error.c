#include <stdio.h>

#include "error.h"

// ========================================
// error.h - definition
// ========================================

void error_print(const char *filepath, const char *src, pos_t start, pos_t end, 
	const char *message) {

	fprintf(stderr, "%s:%d:%d: %s\n", filepath, start.line, start.column, 
			message);

	// Where is the start of the line?
	int index = start.index;
	while (index - 1 >= 0 && src[index - 1] != '\n') index--;

	// Show where the error exists in the top of the line
	fprintf(stderr, "\t|\t");
	for (int i = index; src[i] && src[i] != '\n'; i++) {
		char ch = src[i];
		char is_error = (start.index <= i && i < end.index ? 'v' : ' ');

		char buffer[1024] = {};
		sprintf(buffer, "%c", is_error);
		if (ch == '\t') {
			for (int i = 0; i < 8; i++) buffer[i] = is_error;
			buffer[8] = 0;
		}

		fprintf(stderr, "%s", buffer);
	}
	fprintf(stderr, "\n");

	// Show the lines with error
	for (int line = start.line; line <= end.line; line++) {
		fprintf(stderr, "%d\t>\t", line);

		while (src[index] && src[index] != '\n') {
			char ch = src[index];
			if (ch == '\t') {
				for (int i = 0; i < 8; i++) {
					fprintf(stderr, " ");
				}
			}
			else fprintf(stderr, "%c", ch);
			index++;
		}

		if (src[index]) {
			fprintf(stderr, "\n");
			index++;
		}

		if (line == end.line && src[end.index] == 0) {
			fprintf(stderr, "\n");
			index++;
		}
	}
	fprintf(stderr, "\t|\n");
}

