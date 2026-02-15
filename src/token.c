#include "token.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// token.h - definition
// ========================================

const char *token_type_str(token_t token) {
	if (token.type == TT_EOF) return "TT_EOF";
	if (token.type == TT_COLON) return "TT_COLON";
	if (token.type == TT_SEMICOLON) return "TT_SEMICOLON";
	if (token.type == TT_QUESTION) return "TT_QUESTION";
	if (token.type == TT_LPAREN) return "TT_LPAREN";
	if (token.type == TT_RPAREN) return "TT_RPAREN";
	if (token.type == TT_EQUAL) return "TT_EQUAL";
	if (token.type == TT_EQUAL_EQUAL) return "TT_EQUAL_EQUAL";
	if (token.type == TT_PIPE) return "TT_PIPE";
	if (token.type == TT_LOGICAL_OR) return "TT_LOGICAL_OR";
	if (token.type == TT_AMPERSAND) return "TT_AMPERSAND";
	if (token.type == TT_LOGICAL_AND) return "TT_LOGICAL_AND";
	if (token.type == TT_CARET) return "TT_CARET";
	if (token.type == TT_BANG) return "TT_BANG";
	if (token.type == TT_BANG_EQUAL) return "TT_BANG_EQUAL";
	if (token.type == TT_LESSER) return "TT_LESSER";
	if (token.type == TT_LESSER_EQUAL) return "TT_LESSER_EQUAL";
	if (token.type == TT_LSHIFT) return "TT_LSHIFT";
	if (token.type == TT_GREATER) return "TT_GREATER";
	if (token.type == TT_GREATER_EQUAL) return "TT_GREATER_EQUAL";
	if (token.type == TT_RSHIFT) return "TT_RSHIFT";
	if (token.type == TT_PLUS) return "TT_PLUS";
	if (token.type == TT_PLUS_PLUS) return "TT_PLUS_PLUS";
	if (token.type == TT_MINUS) return "TT_MINUS";
	if (token.type == TT_MINUS_MINUS) return "TT_MINUS_MINUS";
	if (token.type == TT_STAR) return "TT_STAR";
	if (token.type == TT_FSLASH) return "TT_FSLASH";
	if (token.type == TT_MOD) return "TT_MOD";
	if (token.type == TT_TILDE) return "TT_TILDE";
	if (token.type == TT_IDENTIFIER) return "TT_IDENTIFIER";
	if (token.type == TT_VAR_KEYWORD) return "TT_VAR_KEYWORD";
	if (token.type == TT_IF_KEYWORD) return "TT_IF_KEYWORD";
	if (token.type == TT_ELSE_KEYWORD) return "TT_ELSE_KEYWORD";
	if (token.type == TT_GOTO_KEYWORD) return "TT_GOTO_KEYWORD";
	if (token.type == TT_PRINT_KEYWORD) return "TT_PRINT_KEYWORD";
	if (token.type == TT_INT_LITERAL) return "TT_INT_LITERAL";
	return "UNKNOWN";
}

char *token_lexical(token_t token) {
	int lexical_len = token.end.index - token.start.index;
	char *lexical = malloc((lexical_len + 1) * sizeof(char));
	if (lexical == NULL) {
		perror("something went wrong with malloc in token_lexical");
		exit(1);
	}
	for (int i = token.start.index; i < token.end.index; i++) {
		lexical[i-token.start.index] = token.src[i];
	}
	lexical[lexical_len] = 0;
	return lexical;
}

