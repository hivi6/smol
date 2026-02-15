#include "analyzer.h"
#include "error.h"
#include "pos.h"
#include "st.h"

// ========================================
// helper declaration
// ========================================

static int g_has_error;
static const char *g_error_filepath;
static const char *g_error_src;
static pos_t g_error_start, g_error_end;
static const char *g_error_message;

void analyzer_init();

void analyzer_error_set(const char *filepath, const char *src, pos_t start, pos_t end, const char *message);
void analyzer_error_print();
void analyzer_error_clear();
int analyzer_error_check();

void analyzer_rule_prog(ast_t *ast);
void analyzer_rule_stmt(ast_t *stmt);
void analyzer_rule_label_stmt(ast_t *stmt);

// ========================================
// analyzer.h - definition
// ========================================

int analyze(ast_t *ast) {
	analyzer_rule_prog(ast);
	if (analyzer_error_check()) {
		analyzer_error_print();
		analyzer_error_clear();
		return 1;
	}

	return 0;
}

// ========================================
// helper definition
// ========================================

void analyzer_init() {
	g_has_error = 0;

	st_init();
	st_create_type("int", 3);
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
	default:
		analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end, "unexpected statement");
		break;
	}
}

void analyzer_rule_label_stmt(ast_t *stmt) {
	if (stmt->type != AST_LABEL_STMT) {
		analyzer_error_set(stmt->filepath, stmt->src, stmt->start, stmt->end, 
			"expeceted AST_LABEL_STMT ast");
		return;
	}

	token_t label_token = stmt->label_stmt.label;
	const char *label_start = label_token.src + label_token.start.index;
	int label_len = label_token.end.index - label_token.start.index;
	if (st_check_label(label_start, label_len) != -1) {
		analyzer_error_set(label_token.filepath, label_token.src, label_token.start, label_token.end,
			"label already exists");
		return;
	}
	st_create_label(label_start, label_len);
}

