#include "lexer.h"
#include "pos.h"
#include "token.h"
#include "error.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

static const char *g_filepath;
static const char *g_src;
static int g_src_len;
static pos_t g_start, g_end;
static token_t *g_tokens;
static int g_tokens_cap, g_tokens_len;
static int g_has_error;
static const char *g_error_message;

void lexer_init(const char *filepath, const char *src);
char lexer_eof();
char lexer_current();
void lexer_next();
int lexer_get_token();
int lexer_add_token(int token_type);
int lexer_keyword_type();

int lexer_error_check();
void lexer_error_set(const char *message);
void lexer_error_print();
void lexer_error_clear();

// ========================================
// lexer.h - definition
// ========================================

token_t *tokenize(const char *filepath, const char *src) {
	lexer_init(filepath, src);

	int has_error = 0;
	while (!lexer_eof()) {
		lexer_get_token();

		if (lexer_error_check()) {
			lexer_error_print();
			lexer_error_clear();
			has_error = 1;
			break;
		}
	}
	lexer_add_token(TT_EOF);

	if (has_error) {
		free(g_tokens);
		return NULL;
	}

	return g_tokens;
}

// ========================================
// helper definition
// ========================================

void lexer_init(const char *filepath, const char *src) {
	g_filepath = filepath;
	g_src = src;
	g_src_len = strlen(src);
	g_start = g_end = (pos_t) {.line = 1, .column = 1, .index = 0};
	g_tokens = NULL;
	g_tokens_cap = g_tokens_len = 0;
	g_has_error = 0;
	g_error_message = NULL;
}

char lexer_eof() {
	return lexer_current() == 0;
}

char lexer_current() {
	if (g_end.index >= g_src_len) return 0;
	return g_src[g_end.index];
}

char lexer_match(char ch) {
	if (lexer_current() == ch) {
		lexer_next();
		return ch;
	}
	return 0;
}

void lexer_next() {
	char ch = lexer_current();
	if (ch) {
		if (ch == '\n') {
			g_end.column = 0;
			g_end.line++;
		}
		g_end.column++;
		g_end.index++;
	}
}

int lexer_get_token() {
	g_start = g_end;

	char ch = lexer_current();
	lexer_next();
	if (isspace(ch)) {
		return TT_EOF;
	}

	if (ch == ':') {
		return lexer_add_token(TT_COLON);
	}
	else if (ch == ';') {
		return lexer_add_token(TT_SEMICOLON);
	}
	else if (ch == '?') {
		return lexer_add_token(TT_QUESTION);
	}
	else if (ch == '(') {
		return lexer_add_token(TT_LPAREN);
	}
	else if (ch == ')') {
		return lexer_add_token(TT_RPAREN);
	}
	else if (ch == '=') {
		int token_type = TT_EQUAL;
		if (lexer_match('=')) token_type = TT_EQUAL_EQUAL;
		return lexer_add_token(token_type);
	}
	else if (ch == '|') {
		int token_type = TT_PIPE;
		if (lexer_match('|')) token_type = TT_LOGICAL_OR;
		return lexer_add_token(token_type);
	}
	else if (ch == '&') {
		int token_type = TT_AMPERSAND;
		if (lexer_match('&')) token_type = TT_LOGICAL_AND;
		return lexer_add_token(token_type);
	}
	else if (ch == '^') {
		return lexer_add_token(TT_CARET);
	}
	else if (ch == '!') {
		int token_type = TT_BANG;
		if (lexer_match('=')) token_type = TT_BANG_EQUAL;
		return lexer_add_token(token_type);
	}
	else if (ch == '<') {
		int token_type = TT_LESSER;
		if (lexer_match('=')) token_type = TT_LESSER_EQUAL;
		else if (lexer_match('<')) token_type = TT_LSHIFT;
		return lexer_add_token(token_type);
	}
	else if (ch == '>') {
		int token_type = TT_GREATER;
		if (lexer_match('=')) token_type = TT_GREATER_EQUAL;
		else if (lexer_match('>')) token_type = TT_RSHIFT;
		return lexer_add_token(token_type);
	}
	else if (ch == '+') {
		int token_type = TT_PLUS;
		if (lexer_match('+')) token_type = TT_PLUS_PLUS;
		return lexer_add_token(token_type);
	}
	else if (ch == '-') {
		int token_type = TT_MINUS;
		if (lexer_match('-')) token_type = TT_MINUS_MINUS;
		return lexer_add_token(token_type);
	}
	else if (ch == '*') {
		return lexer_add_token(TT_STAR);
	}
	else if (ch == '/') {
		return lexer_add_token(TT_FSLASH);
	}
	else if (ch == '%') {
		return lexer_add_token(TT_MOD);
	}
	else if (ch == '~') {
		return lexer_add_token(TT_TILDE);
	}
	else if (isalpha(ch) || ch == '_') {
		while (!lexer_eof() && (ch = lexer_current())) {
			if (isalnum(ch) || ch == '_') lexer_next();
			else break;
		}

		int token_type = lexer_keyword_type();
		return lexer_add_token(token_type);
	}
	else if (isdigit(ch)) {
		while (!lexer_eof() && (ch = lexer_current()) && isdigit(ch)) {
			lexer_next();
		}

		return lexer_add_token(TT_INT_LITERAL);
	}

	lexer_error_set("unexpected token");
	return TT_EOF;
}

int lexer_add_token(int token_type) {
	if (g_tokens_cap <= g_tokens_len) {
		g_tokens_cap = (g_tokens_cap + 1) * 2;
		g_tokens = realloc(g_tokens, g_tokens_cap * sizeof(token_t));
		if (g_tokens == NULL) {
			perror("Error while realloc in lexer_add_token");
			exit(1);
		}
	}
	g_tokens[g_tokens_len] = (token_t) {
		.type = token_type,
		.start = g_start,
		.end = g_end,
		.filepath = g_filepath,
		.src = g_src,
	};
	g_tokens_len++;
	return token_type;
}

int lexer_keyword_type() {
	const char *start = g_src + g_start.index;
	int len = g_end.index - g_start.index;

	if (strlen("var") == len && strncmp(start, "var", len) == 0) return TT_VAR_KEYWORD;
	if (strlen("if") == len && strncmp(start, "if", len) == 0) return TT_IF_KEYWORD;
	if (strlen("else") == len && strncmp(start, "else", len) == 0) return TT_ELSE_KEYWORD;
	if (strlen("goto") == len && strncmp(start, "goto", len) == 0) return TT_GOTO_KEYWORD;
	if (strlen("print") == len && strncmp(start, "print", len) == 0) return TT_PRINT_KEYWORD;

	return TT_IDENTIFIER;
}

int lexer_error_check() {
	return g_has_error;
}

void lexer_error_set(const char *message) {
	g_has_error = 1;
	g_error_message = message;
}

void lexer_error_print() {
	error_print(g_filepath, g_src, g_start, g_end, g_error_message);
}

void lexer_error_clear() {
	g_has_error = 0;
}

