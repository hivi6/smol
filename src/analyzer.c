#include "analyzer.h"
#include "error.h"
#include "pos.h"
#include "st.h"

#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static int g_has_error;
static const char *g_error_filepath;
static const char *g_error_src;
static pos_t g_error_start, g_error_end;
static const char *g_error_message;

void analyzer_init();
void analyzer_free();

void analyzer_error_set(const char *filepath, const char *src, pos_t start, pos_t end, const char *message);
void analyzer_error_print();
void analyzer_error_clear();
int analyzer_error_check();

void analyzer_rule_prog(ast_t *ast);
void analyzer_rule_stmt(ast_t *stmt);
void analyzer_rule_label_stmt(ast_t *stmt);
void analyzer_rule_var_stmt(ast_t *stmt);
void analyzer_rule_expr(ast_t *expr);
void analyzer_rule_literal(ast_t *expr);
void analyzer_rule_identifier(ast_t *expr);
void analyzer_rule_unary(ast_t *expr);
void analyzer_rule_binary(ast_t *expr);
void analyzer_rule_ternary(ast_t *expr);

int bigger_type_id(int ltype_id, int rtype_id);
int is_numerical_type(int type_id);
int is_lhs(ast_t *expr);
int is_compatible_type(int ltype_id, int rtype_id);

// ========================================
// analyzer.h - definition
// ========================================

int analyze(ast_t *ast) {
	analyzer_init();

	analyzer_rule_prog(ast);
	if (analyzer_error_check()) {
		analyzer_error_print();
		analyzer_error_clear();
		return 1;
	}

	analyzer_free();

	return 0;
}

// ========================================
// helper definition
// ========================================

void analyzer_init() {
	g_has_error = 0;

	st_init();
	st_create_type("int");
}

void analyzer_free() {
	st_free();
}

void analyzer_error_set(const char *filepath, const char *src, pos_t start, pos_t end, const char *message) {
	g_has_error = 1;
	g_error_filepath = filepath;
	g_error_src = src;
	g_error_start = start;
	g_error_end = end;
	g_error_message = message;
}

void analyzer_error_print() {
	error_print(g_error_filepath, g_error_src, g_error_start, g_error_end, g_error_message);
}

void analyzer_error_clear() {
	g_has_error = 0;
}

int analyzer_error_check() {
	return g_has_error;
}

void analyzer_rule_prog(ast_t *ast) {
	if (ast->type != AST_PROG) {
		analyzer_error_set(ast->filepath, ast->src, ast->start, ast->end, "expected AST_PROG ast");
		return;
	}

	for (int i = 0; i < ast->prog.len; i++) {
		ast_t *stmt = ast->prog.stmts[i];
		analyzer_rule_stmt(stmt);
		if (analyzer_error_check()) {
			return;
		}
	}
}

void analyzer_rule_stmt(ast_t *stmt) {
	switch (stmt->type) {
	case AST_LABEL_STMT:
		analyzer_rule_label_stmt(stmt);
		break;
	case AST_VAR_STMT:
		analyzer_rule_var_stmt(stmt);
		break;
	default:
		analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end, "unexpected statement");
		break;
	}
}

void analyzer_rule_label_stmt(ast_t *stmt) {
	char *lexical = NULL;

	if (stmt->type != AST_LABEL_STMT) {
		analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end, 
			"expected AST_LABEL_STMT ast");
		goto cleanup;
	}

	token_t label_token = stmt->label_stmt.label;
	lexical = token_lexical(label_token);
	if (st_check_label(lexical).id != -1) {
		analyzer_error_set(label_token.filepath, label_token.src, label_token.start, label_token.end,
			"label already declared");
		goto cleanup;
	}
	st_create_label(lexical);

cleanup:
	free(lexical);
}

void analyzer_rule_var_stmt(ast_t *stmt) {
	char *lexical = NULL;

	if (stmt->type != AST_VAR_STMT) {
		analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end,
			"expected AST_VAR_STMT ast");
		goto cleanup;
	}

	token_t var_token = stmt->var_stmt.name;
	lexical = token_lexical(var_token);
	if (st_check_var(lexical).id != -1) {
		analyzer_error_set(var_token.filepath, var_token.src, var_token.start, var_token.end,
			"variable already declared");
		goto cleanup;
	}
	int type_id = st_check_type("int").id;

	if (stmt->var_stmt.expr) {
		analyzer_rule_expr(stmt->var_stmt.expr);
		if (analyzer_error_check()) {
			goto cleanup;
		}

		if (type_id != stmt->var_stmt.expr->type_id) {
			analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end,
				"variable and expression are of different type");
			goto cleanup;
		}
	}

	st_create_var(lexical, type_id);

cleanup:
	free(lexical);
}

void analyzer_rule_expr(ast_t *expr) {
	switch (expr->type) {
	case AST_LITERAL:
		analyzer_rule_literal(expr);
		break;
	case AST_IDENTIFIER:
		analyzer_rule_identifier(expr);
		break;
	case AST_UNARY:
		analyzer_rule_unary(expr);
		break;
	case AST_BINARY:
		analyzer_rule_binary(expr);
		break;
	case AST_TERNARY:
		analyzer_rule_ternary(expr);
		break;
	default:
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"unexpected expression");
	}
}

void analyzer_rule_literal(ast_t *expr) {
	if (expr->type != AST_LITERAL) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected AST_LITERAL ast");
		return;
	}

	token_t token = expr->literal.token;
	if (token.type == TT_INT_LITERAL) {
		expr->type_id = st_check_type("int").id;
	}
	else {
		analyzer_error_set(token.filepath, token.src, token.start, token.end,
			"unexpected literal token");
		return;
	}
}

void analyzer_rule_identifier(ast_t *expr) {
	char *lexical = NULL;

	if (expr->type != AST_IDENTIFIER) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected AST_IDENTIFIER ast");
		goto cleanup;
	}

	token_t token = expr->identifier.token;
	lexical = token_lexical(token);
	if (token.type == TT_IDENTIFIER) {
		name_t name = st_check_var(lexical);
		if (name.id == -1) {
			analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
				"variable undefined");
			goto cleanup;
		}
		expr->type_id = name.type_id;
	}
	else {
		analyzer_error_set(token.filepath, token.src, token.start, token.end,
			"unexpected identifier token");
		goto cleanup;
	}

cleanup:
	free(lexical);
}

void analyzer_rule_unary(ast_t *expr) {
	if (expr->type != AST_UNARY) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected AST_UNARY ast");
		return;
	}

	switch (expr->unary.op.type) {
	case TT_BANG:
	case TT_TILDE:
	case TT_MINUS:
	case TT_PLUS:
		analyzer_rule_expr(expr->unary.right);
		if (analyzer_error_check()) {
			return;
		}

		if (!is_numerical_type(expr->unary.right->type_id)) {
			analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
				"expected numerical type in unary expression");
		}

		expr->type_id = expr->unary.right->type_id;
		break;
	
	case TT_MINUS_MINUS:
	case TT_PLUS_PLUS:
		if (!is_lhs(expr->unary.right)) {
			analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
				"expected lhs");
		}

		analyzer_rule_expr(expr->unary.right);
		if (analyzer_error_check()) {
			return;
		}
	
		expr->type_id = expr->unary.right->type_id;
		break;
	
	default:
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"unexpected unary operation");
	}
}

void analyzer_rule_binary(ast_t *expr) {
	if (expr->type != AST_BINARY) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected AST_BINARY ast");
		return;
	}

	switch (expr->binary.op.type) {
	case TT_STAR:
	case TT_FSLASH:
	case TT_MOD:
	case TT_PLUS:
	case TT_MINUS:
	case TT_LSHIFT:
	case TT_RSHIFT:
	case TT_LESSER:
	case TT_LESSER_EQUAL:
	case TT_GREATER:
	case TT_GREATER_EQUAL:
	case TT_EQUAL_EQUAL:
	case TT_BANG_EQUAL:
	case TT_AMPERSAND:
	case TT_CARET:
	case TT_PIPE:
	case TT_LOGICAL_AND:
	case TT_LOGICAL_OR:
	case TT_EQUAL:
		if (expr->binary.op.type == TT_EQUAL && !is_lhs(expr->binary.left)) {
			analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
				"expected lhs");
			return;
		}

		analyzer_rule_expr(expr->binary.left);
		if (analyzer_error_check()) {
			return;
		}

		analyzer_rule_expr(expr->binary.right);
		if (analyzer_error_check()) {
			return;
		}

		if (!is_compatible_type(expr->binary.left->type_id, expr->binary.right->type_id)) {
			analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
				"left side of operation is uncompatible with right side");
			return;
		}

		expr->type_id = bigger_type_id(expr->binary.left->type_id, expr->binary.right->type_id);
		break;

	default:
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"unsupported binary operation");
	}
}

void analyzer_rule_ternary(ast_t *expr) {
	if (expr->type != AST_TERNARY) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected AST_BINARY ast");
		return;
	}

	analyzer_rule_expr(expr->ternary.left);
	if (analyzer_error_check()) {
		return;
	}

	if (!is_numerical_type(expr->ternary.left->type_id)) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"expected numeric type in ternary condition");
		return;
	}

	analyzer_rule_expr(expr->ternary.mid);
	if (analyzer_error_check()) {
		return;
	}

	analyzer_rule_expr(expr->ternary.right);
	if (analyzer_error_check()) {
		return;
	}

	if (!is_compatible_type(expr->ternary.mid->type_id, expr->ternary.right->type_id)) {
		analyzer_error_set(expr->filepath, expr->src, expr->start, expr->end,
			"uncompatible mid and right block of ternary operator");
		return;
	}

	expr->type_id = bigger_type_id(expr->ternary.mid->type_id, expr->ternary.right->type_id);
}

int is_numerical_type(int type_id) {
	return st_check_type("int").id == type_id;
}

int is_lhs(ast_t *expr) {
	return expr->type == AST_IDENTIFIER;
}

int bigger_type_id(int ltype_id, int rtype_id) {
	return ltype_id;
}

int is_compatible_type(int ltype_id, int rtype_id) {
	return ltype_id == rtype_id;
}

