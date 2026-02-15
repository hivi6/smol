#include "ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

ast_t *ast_malloc(int type, pos_t start, pos_t end, const char *filepath, const char *src);
void ast_print_helper(ast_t *ast, char *last, int depth);
void ast_print_token(token_t token);

// ========================================
// ast.h - definition
// ========================================

void ast_free(ast_t *ast) {
	if (ast == NULL) return;

	switch (ast->type) {
	case AST_LITERAL:
	case AST_IDENTIFIER:
		break;
	case AST_UNARY:
		ast_free(ast->unary.right);
		break;
	case AST_BINARY:
		ast_free(ast->binary.left);
		ast_free(ast->binary.right);
		break;
	case AST_TERNARY:
		ast_free(ast->ternary.left);
		ast_free(ast->ternary.mid);
		ast_free(ast->ternary.right);
		break;
	case AST_EXPR_STMT:
		ast_free(ast->expr_stmt.expr);
		break;
	case AST_PROG: {
		for (int i = 0; i < ast->prog.len; i++) {
			ast_free(ast->prog.stmts[i]);
		}
		free(ast->prog.stmts);
	}
	}

	free(ast);
}

ast_t *ast_literal(token_t token) {
	ast_t *res = ast_malloc(AST_LITERAL, token.start, token.end, token.filepath, token.src);
	res->literal.token = token;
	return res;
}

ast_t *ast_unary(token_t op, ast_t *right) {
	ast_t *res = ast_malloc(AST_UNARY, op.start, right->end, op.filepath, op.src);
	res->unary.op = op;
	res->unary.right = right;
	return res;
}

ast_t *ast_binary(ast_t *left, token_t op, ast_t *right) {
	ast_t *res = ast_malloc(AST_BINARY, left->start, right->end, left->filepath, left->src);
	res->binary.left = left;
	res->binary.op = op;
	res->binary.right = right;
	return res;
}

ast_t *ast_ternary(ast_t *left, ast_t *mid, ast_t *right) {
	ast_t *res = ast_malloc(AST_TERNARY, left->start, right->end, left->filepath, left->src);
	res->ternary.left = left;
	res->ternary.mid = mid;
	res->ternary.right = right;
	return res;
}

ast_t *ast_identifier(token_t token) {
	ast_t *res = ast_malloc(AST_IDENTIFIER, token.start, token.end, token.filepath, token.src);
	res->identifier.token = token;
	return res;
}

ast_t *ast_expr_stmt(ast_t *expr, token_t semicolon) {
	ast_t *res = ast_malloc(AST_EXPR_STMT, expr->start, semicolon.end, expr->filepath, expr->src);
	res->expr_stmt.expr = expr;
	res->expr_stmt.semicolon = semicolon;
	return res;
}

ast_t *ast_prog() {
	pos_t pos;
	ast_t *res = ast_malloc(AST_PROG, pos, pos, NULL, NULL);
	res->prog.stmts = NULL;
	res->prog.cap = res->prog.len = 0;
	return res;
}

void ast_prog_append(ast_t *prog, ast_t *stmt) {
	assert(prog->type == AST_PROG);

	if (prog->prog.len == 0) prog->start = stmt->start;
	prog->end = stmt->end;
	prog->filepath = stmt->filepath;
	prog->src = stmt->src;

	prog->prog.len++;
	if (prog->prog.cap <= prog->prog.len) {
		prog->prog.cap = (prog->prog.cap + 1) * 2;
		prog->prog.stmts = realloc(prog->prog.stmts, sizeof(ast_t *) * prog->prog.cap);
		if (prog->prog.stmts == NULL) {
			perror("Something went wrong while realloc in ast_prog_append");
			exit(1);
		}
	}
	prog->prog.stmts[prog->prog.len-1] = stmt;
}

void ast_print(ast_t *ast) {
	char last[AST_PRINT_DEPTH] = {};
	printf("AST\n");
	ast_print_helper(ast, last, 0);
}

// ========================================
// helper definition
// ========================================

ast_t *ast_malloc(int type, pos_t start, pos_t end, const char *filepath, const char *src) {
	ast_t *res = malloc(sizeof(ast_t));
	if (res == NULL) {
		perror("Error on malloc in ast_malloc");
		exit(1);
	}

	res->type = type;
	res->start = start;
	res->end = end;
	res->filepath = filepath;
	res->src = src;
	return res;
}

void ast_print_helper(ast_t *ast, char *last, int depth) {
	for (int i = 0; i < depth; i++) {
		if (last[i]) printf("|   ");
		else printf("    ");
	}

	last[depth+1] = 1;
	switch (ast->type) {
	case AST_LITERAL:
		printf("+-- AST_LITERAL(");
		ast_print_token(ast->literal.token);
		printf(")\n");

		last[depth+1] = 0;
		break;

	case AST_UNARY:
		printf("+-- AST_UNARY(");
		ast_print_token(ast->unary.op);
		printf(")\n");

		last[depth+1] = 0;
		ast_print_helper(ast->unary.right, last, depth+1);
		break;

	case AST_BINARY:
		printf("+-- AST_BINARY(");
		ast_print_token(ast->binary.op);
		printf(")\n");

		ast_print_helper(ast->binary.left, last, depth+1);

		last[depth+1] = 0;
		ast_print_helper(ast->binary.right, last, depth+1);
		break;

	case AST_TERNARY:
		printf("+-- AST_TERNARY\n");

		ast_print_helper(ast->ternary.left, last, depth+1);
		ast_print_helper(ast->ternary.mid, last, depth+1);

		last[depth+1] = 0;
		ast_print_helper(ast->ternary.right, last, depth+1);
		break;
	
	case AST_IDENTIFIER:
		printf("+-- AST_IDENTIFER(");
		ast_print_token(ast->identifier.token);
		printf(")\n");

		last[depth+1] = 0;
		break;

	case AST_EXPR_STMT:
		printf("+-- AST_EXPR_STMT\n");

		last[depth+1] = 0;
		ast_print_helper(ast->expr_stmt.expr, last, depth+1);
		break;
	
	case AST_PROG: {
		printf("+-- AST_PROG\n");

		for (int i = 0; i < ast->prog.len; i++) {
			if (i == ast->prog.len - 1) last[depth+1] = 0;
			ast_print_helper(ast->prog.stmts[i], last, depth+1);
		}
		last[depth+1] = 0;
		break;
	}
	}
}

void ast_print_token(token_t token) {
	printf("%s | '%.*s'", token_type_str(token), 
		token.end.index - token.start.index, token.src + token.start.index);
}

