#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

/**
 * Create the ast using the stream of tokens
 *
 * Parameters:
 * 	tokens	List of tokens
 *
 * Returns:
 * 	ast memory
 */
ast_t *parse(token_t *tokens);

#endif // PARSER_H
