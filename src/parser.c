#include "parser.h"
#include "ast.h"
#include "token.h"
#include "pos.h"
#include "error.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static token_t *g_tokens;
static int g_index;
static int g_has_error;
static const char *g_error_filepath;
static const char *g_error_src;
static pos_t g_error_start, g_error_end;
static const char *g_error_message;

void parser_init(token_t *token);
token_t parser_current_token();
token_t parser_next_token();
void parser_next();

char parser_error_check();
void parser_error_print();
void parser_error_clear();
void parser_error_set(const char *filepath, const char *src, pos_t start, pos_t end, const char *message);

ast_t *parser_rule_prog();
ast_t *parser_rule_stmt();
ast_t *parser_rule_label_stmt();
ast_t *parser_rule_var_stmt();
ast_t *parser_rule_expr_stmt();
ast_t *parser_rule_expr();
ast_t *parser_rule_assign();
ast_t *parser_rule_ternary();
ast_t *parser_rule_logical_or();
ast_t *parser_rule_logical_and();
ast_t *parser_rule_bitwise_or();
ast_t *parser_rule_bitwise_xor();
ast_t *parser_rule_bitwise_and();
ast_t *parser_rule_equality();
ast_t *parser_rule_relation();
ast_t *parser_rule_shift();
ast_t *parser_rule_add();
ast_t *parser_rule_term();
ast_t *parser_rule_unary();
ast_t *parser_rule_group();
ast_t *parser_rule_primary();

// ========================================
// parser.h - definition
// ========================================

ast_t *parse(token_t *tokens) {
	parser_init(tokens);

	ast_t *res = parser_rule_prog();
	if (parser_error_check()) {
		parser_error_print();
		parser_error_clear();
		return NULL;
	}

	token_t token = parser_current_token();
	if (token.type != TT_EOF) {
		parser_error_set(token.filepath, token.src, token.start, token.end, "Expected EOF");
		parser_error_print();
		parser_error_clear();
		ast_free(res);
		return NULL;
	}

	return res;
}

// ========================================
// helper definition
// ========================================

void parser_init(token_t *tokens) {
	g_tokens = tokens;
	g_index = 0;
	g_has_error = 0;
	g_error_filepath = NULL;
	g_error_src = NULL;
	g_error_start = g_error_end = (pos_t) {.line=1, .column=1, .index=0};
	g_error_message = NULL;
}

token_t parser_current_token() {
	return g_tokens[g_index];
}

token_t parser_next_token() {
	assert(parser_current_token().type != TT_EOF);
	return g_tokens[g_index+1];
}

void parser_next() {
	token_t cur = parser_current_token();
	if (cur.type != TT_EOF) {
		g_index++;
	}
}

char parser_error_check() {
	return g_has_error != 0;
}

void parser_error_print() {
	error_print(g_error_filepath, g_error_src, g_error_start, g_error_end, g_error_message);
}

void parser_error_clear() {
	g_has_error = 0;
}

void parser_error_set(const char *filepath, const char *src, pos_t start, pos_t end, const char *message) {
	g_has_error = 1;
	g_error_filepath = filepath;
	g_error_src = src;
	g_error_start = start;
	g_error_end = end;
	g_error_message = message;
}

ast_t *parser_rule_prog() {
	ast_t *prog = ast_prog();

	while (parser_current_token().type != TT_EOF) {
		ast_t *stmt = parser_rule_stmt();
		if (parser_error_check()) {
			ast_free(stmt);
			ast_free(prog);
			return NULL;
		}
		ast_prog_append(prog, stmt);
	}

	return prog;
}

ast_t *parser_rule_stmt() {
	ast_t *stmt = NULL;

	if (parser_current_token().type == TT_IDENTIFIER && parser_next_token().type == TT_COLON)
		stmt = parser_rule_label_stmt();
	else if (parser_current_token().type == TT_VAR_KEYWORD)
		stmt = parser_rule_var_stmt();
	else
		stmt = parser_rule_expr_stmt();

	if (parser_error_check()) {
		ast_free(stmt);
		return NULL;
	}

	return stmt;
}

ast_t *parser_rule_label_stmt() {
	token_t label = parser_current_token();
	parser_next();

	token_t colon = parser_current_token();
	parser_next();

	return ast_label_stmt(label, colon);
}

ast_t *parser_rule_var_stmt() {
	token_t var_keyword = parser_current_token();
	parser_next();

	token_t name = parser_current_token();
	if (name.type != TT_IDENTIFIER) {
		parser_error_set(var_keyword.filepath, var_keyword.src, var_keyword.start, name.end,
			"Expected an identifier after 'var' keyword");
		return NULL;
	}
	parser_next();

	ast_t *expr = NULL;
	if (parser_current_token().type == TT_EQUAL) {
		parser_next(); // pass next '='
		
		expr = parser_rule_expr();
		if (parser_error_check()) {
			return NULL;
		}
	}

	token_t semicolon = parser_current_token();
	if (semicolon.type != TT_SEMICOLON) {
		ast_free(expr);
		parser_error_set(var_keyword.filepath, var_keyword.src, var_keyword.start, 
			semicolon.end, "Expected ';' at the end of 'var' statement");
	}
	parser_next();

	return ast_var_stmt(var_keyword, name, expr, semicolon);
}

ast_t *parser_rule_expr_stmt() {
	ast_t *expr = parser_rule_expr();
	if (expr == NULL) {
		return NULL;
	}

	token_t semicolon = parser_current_token();
	if (semicolon.type != TT_SEMICOLON) {
		parser_error_set(expr->filepath, expr->src, expr->start, semicolon.end, 
			"Expected ';' after expression");
		ast_free(expr);
		return NULL;
	}
	parser_next();

	return ast_expr_stmt(expr, semicolon);
}

ast_t *parser_rule_expr() {
	return parser_rule_assign();
}

ast_t *parser_rule_assign() {
	ast_t *left = parser_rule_ternary();
	if (left == NULL) {
		return left;
	}

	if (parser_current_token().type == TT_EQUAL) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_assign();
		if (right == NULL) {
			ast_free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_ternary() {
	ast_t *left = parser_rule_logical_or();
	if (left == NULL) {
		return NULL;
	}

	if (parser_current_token().type == TT_QUESTION) {
		parser_next();

		ast_t *mid = parser_rule_ternary();
		if (mid == NULL) {
			ast_free(left);
			return NULL;
		}

		token_t colon = parser_current_token();
		if (colon.type != TT_COLON) {
			parser_error_set(colon.filepath, colon.src, left->start, colon.end, 
				"Expected ':' for ternary operator");
			ast_free(left);
			ast_free(mid);
			return NULL;
		}
		parser_next();

		ast_t *right = parser_rule_ternary();
		if (right == NULL) {
			ast_free(left);
			ast_free(mid);
			return NULL;
		}

		printf("Here\n");
		left = ast_ternary(left, mid, right);
	}

	return left;
}

ast_t *parser_rule_logical_or() {
	ast_t *left = parser_rule_logical_and();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_LOGICAL_OR) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_logical_and();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_logical_and() {
	ast_t *left = parser_rule_bitwise_or();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_LOGICAL_AND) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_bitwise_or();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_bitwise_or() {
	ast_t *left = parser_rule_bitwise_xor();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_PIPE) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_bitwise_xor();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_bitwise_xor() {
	ast_t *left = parser_rule_bitwise_and();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_CARET) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_bitwise_and();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_bitwise_and() {
	ast_t *left = parser_rule_equality();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_AMPERSAND) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_equality();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_equality() {
	ast_t *left = parser_rule_relation();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_EQUAL_EQUAL || 
		parser_current_token().type == TT_BANG_EQUAL) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_relation();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_relation() {
	ast_t *left = parser_rule_shift();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_LESSER ||
		parser_current_token().type == TT_LESSER_EQUAL ||
		parser_current_token().type == TT_GREATER ||
		parser_current_token().type == TT_GREATER_EQUAL) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_shift();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_shift() {
	ast_t *left = parser_rule_add();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_LSHIFT ||
		parser_current_token().type == TT_RSHIFT) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_add();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_add() {
	ast_t *left = parser_rule_term();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_PLUS || 
		parser_current_token().type == TT_MINUS) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_term();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_term() {
	ast_t *left = parser_rule_unary();
	if (left == NULL) {
		return NULL;
	}

	while (parser_current_token().type == TT_STAR ||
		parser_current_token().type == TT_FSLASH ||
		parser_current_token().type == TT_MOD) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_unary();
		if (right == NULL) {
			free(left);
			return NULL;
		}

		left = ast_binary(left, op, right);
	}

	return left;
}

ast_t *parser_rule_unary() {
	if (parser_current_token().type == TT_BANG ||
		parser_current_token().type == TT_TILDE ||
		parser_current_token().type == TT_MINUS ||
		parser_current_token().type == TT_PLUS ||
		parser_current_token().type == TT_MINUS_MINUS ||
		parser_current_token().type == TT_PLUS_PLUS) {
		token_t op = parser_current_token();
		parser_next();

		ast_t *right = parser_rule_unary();
		if (right == NULL) {
			return NULL;
		}

		return ast_unary(op, right);
	}

	return parser_rule_group();
}

ast_t *parser_rule_group() {
	if (parser_current_token().type == TT_LPAREN) {
		token_t lparen = parser_current_token();
		parser_next();

		ast_t *res = parser_rule_expr();
		if (res == NULL) {
			return NULL;
		}

		token_t token = parser_current_token();
		if (token.type != TT_RPAREN) {
			parser_error_set(token.filepath, token.src, lparen.start, token.end, 
				"Expected ')' for grouping");
			ast_free(res);
			return NULL;
		}
		parser_next();

		return res;
	}

	return parser_rule_primary();
}

ast_t *parser_rule_primary() {
	token_t token = parser_current_token();
	if (token.type == TT_INT_LITERAL) {
		parser_next();
		return ast_literal(token);
	}

	if (token.type == TT_IDENTIFIER) {
		parser_next();
		return ast_identifier(token);
	}

	const char *message = "Unexpected token";
	if (token.type == TT_EOF) {
		message = "Expecting some tokens, reached EOF instead";
	}

	parser_error_set(token.filepath, token.src, token.start, token.end, message);
	return NULL;
}

