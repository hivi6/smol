#ifndef ANALYZER_H
#define ANALYZER_H

#include "ast.h"

/**
 * Analyze the ast
 *
 * Returns:
 * 	0 (if no error)
 * 	1 (if error)
 */
int analyze(ast_t *ast);

#endif // ANALYZER_H
