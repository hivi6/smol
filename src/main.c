#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "analyzer.h"
#include "ir.h"
#include "st.h"
#include "vm.h"

// ========================================
// helper declaration
// ========================================

void usage(FILE *fd);
char *read_file(const char *filepath);

// ========================================
// main definition
// ========================================

int main(int argc, const char **argv) {
	int index = 1;
	int usage_flag = 0;
	const char *output_file = "a.out";
	int lexer_flag = 0, parser_flag = 0, ir_flag = 0;
	while (index < argc) {
		if (strcmp("--help", argv[index]) == 0 ||
			strcmp("-h", argv[index]) == 0) {
			usage_flag = 1;
		}
		else if (strcmp("--output", argv[index]) == 0) {
			index++;
			if (index >= argc) {
				fprintf(stderr, "ERROR: Expected filepath after --output flag\n");
				usage(stderr);
				return 1;
			}
			output_file = argv[index];
		}
		else if (strcmp("--only-lexer", argv[index]) == 0) {
			lexer_flag = 1;
		}
		else if (strcmp("--only-parser", argv[index]) == 0) {
			parser_flag = 1;
		}
		else if (strcmp("--only-ir", argv[index]) == 0) {
			ir_flag = 1;
		}
		else break;
		index++;
	}

	if (usage_flag) {
		usage(stdout);
		return 0;
	}

	if (index >= argc) {
		fprintf(stderr, "ERROR: Expected source files\n");
		usage(stderr);
		return 1;
	}

	const char *filepath = argv[index];
	char *src = read_file(filepath);

	token_t *tokens = tokenize(filepath, src);
	if (tokens == NULL) {
		exit(1);
	}

	if (lexer_flag) {
		for (token_t *cur = tokens; cur->type != TT_EOF; cur++) {
			printf("%s | '%.*s'\n", token_type_str(*cur), 
				cur->end.index - cur->start.index, cur->src + cur->start.index);
		}
		return 0;
	}

	ast_t *ast = parse(tokens);
	if (ast == NULL) {
		exit(1);
	}
	if (parser_flag) {
		ast_print(ast);
		return 0;
	}

	// initialize the symbol table
	st_init();
	st_create_type("int");

	int error = analyze(ast);
	if (error) {
		exit(1);
	}

	ir_t *ir_list = generate_ir(ast);
	if (ir_list == NULL) {
		exit(1);
	}
	if (ir_flag) {
		print_ir(ir_list);
		return 0;
	}

	vm_run(ir_list);

	free(ir_list);

	st_free();

	ast_free(ast);

	free(src);
	free(tokens);

	return 0;
}

// ========================================
// helper definition
// ========================================

void usage(FILE *fd) {
	fprintf(fd, "USAGE: ./smol [flags] <filename>\n");
	fprintf(fd, "\n");
	fprintf(fd, "FLAGS:\n");
	fprintf(fd, "        --help, -h                 This screen\n");
	fprintf(fd, "        --output <filename>        Change the output filepath\n");
	fprintf(fd, "        --only-lexer               Print only the output of lexer\n");
	fprintf(fd, "        --only-parser              Print only the output of parser\n");
	fprintf(fd, "        --only-ir                  Print only the output of ir generator\n");
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

