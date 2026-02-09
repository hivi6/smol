#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/**
 * Tokenize the source code
 *
 * Parameters:
 * 	filepath	filepath the source code belongs to
 * 	src		source code that needs tokenization
 *
 * Returns:
 * 	list of tokens (Users responsibility for freeing memory)
 */
token_t *tokenize(const char *filepath, const char *src);

#endif // LEXER_H
