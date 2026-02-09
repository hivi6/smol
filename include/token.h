#ifndef TOKEN_H
#define TOKEN_H

#include "pos.h"

enum {
	TT_EOF = 0,
	
	TT_COLON, 
	TT_SEMICOLON, 
	TT_QUESTION,
	TT_LPAREN, TT_RPAREN,
	TT_EQUAL, TT_EQUAL_EQUAL,
	TT_PIPE, TT_LOGICAL_OR,
	TT_AMPERSAND, TT_LOGICAL_AND,
	TT_CARET,
	TT_BANG, TT_BANG_EQUAL,
	TT_LESSER, TT_LESSER_EQUAL, TT_LSHIFT,
	TT_GREATER, TT_GREATER_EQUAL, TT_RSHIFT,
	TT_PLUS, TT_PLUS_PLUS,
	TT_MINUS, TT_MINUS_MINUS,
	TT_STAR,
	TT_FSLASH,
	TT_MOD,
	TT_TILDE,

	TT_IDENTIFIER,

	TT_VAR_KEYWORD,
	TT_IF_KEYWORD,
	TT_ELSE_KEYWORD,
	TT_GOTO_KEYWORD,
	TT_PRINT_KEYWORD,

	TT_INT_LITERAL,

	TOTAL_TOKENS,
};

typedef struct {
	int type;
	pos_t start;
	pos_t end;
	const char *filepath;
	const char *src;
} token_t;

/**
 * Get the string converter type of the token
 *
 * Parameter:
 * 	token	whose string mapped type needs to be provided
 *
 * Returns:
 * 	character array with string mapped type
 */
const char *token_type_str(token_t token);

#endif // TOKEN_H
