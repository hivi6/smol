#ifndef AST_H
#define AST_H

#include "pos.h"
#include "token.h"

#define AST_PRINT_DEPTH 1024

enum {
	AST_LITERAL,
	AST_UNARY,
	AST_BINARY,
	AST_TERNARY,
	AST_IDENTIFIER,
	AST_EXPR_STMT,
	AST_PROG,
};

struct ast_t {
	int type;
	pos_t start;
	pos_t end;
	const char *filepath;
	const char *src;

	struct {
		token_t token;
	} literal;

	struct {
		token_t op;
		struct ast_t *right;
	} unary;

	struct {
		struct ast_t *left;
		token_t op;
		struct ast_t *right;
	} binary;

	struct {
		struct ast_t *left;
		struct ast_t *mid;
		struct ast_t *right;
	} ternary;

	struct {
		token_t token;
	} identifier;

	struct {
		struct ast_t *expr;
		token_t semicolon;
	} expr_stmt;

	struct {
		struct ast_t **stmts;
		int cap;
		int len;
	} prog;
};

typedef struct ast_t ast_t;

/**
 * Free the ast allocated memory
 *
 * Parameter:
 * 	ast	The memory that needs freeing
 */
void ast_free(ast_t *ast);

/**
 * Create a literal ast
 *
 * Parameter:
 * 	token	the literal token required
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_literal(token_t token);

/**
 * Create an unary ast
 *
 * Parameter:
 * 	op	unary operator token
 * 	right	ast to the right of the operator
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_unary(token_t op, ast_t *right);

/**
 * Create a binary ast
 *
 * Parameter:
 * 	left	ast to the left of the infix operator
 * 	op	binary operator token
 * 	right	ast to the right of the infix operator
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_binary(ast_t *left, token_t op, ast_t *right);

/**
 * Create a ternary ast (conditional operator)
 *
 * Parameter:
 * 	left	First ast in the conditional operator
 * 	mid	Second ast in the conditional operator
 * 	right	Third ast in the conditional operator
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_ternary(ast_t *left, ast_t *mid, ast_t *right);

/**
 * Create an identifier ast
 *
 * Parameter:
 * 	token	the identifier token required
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_identifier(token_t token);

/**
 * Create a expr stmt ast
 *
 * Parameter:
 * 	expr		the expression ast
 * 	semicolon	the semicolon token
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon);

/**
 * Create a prog ast
 *
 * Returns:
 * 	ast memory
 */
ast_t *ast_prog();

/**
 * Append stmt to the prog ast
 *
 * Parameters:
 * 	prog	The prog ast where stmt is appended
 * 	stmt	Statement that is appended
 */
void ast_prog_append(ast_t *prog, ast_t *stmt);

/**
 * Print the given ast
 *
 * Parameters:
 * 	ast	The ast that will be printed
 */
void ast_print(ast_t *ast);

#endif // AST_H

