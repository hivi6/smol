#include "ir.h"
#include "ast.h"
#include "st.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

static ir_t *g_ir_list;
static int g_ir_list_len;
static int g_temp_len;

void ir_init();
ir_t *ir_list();
void ir_emit(int op, int res_id, int arg1_id, int arg2_id);

void print_ir_print1(const char *op_str, int res_id, const char *res_name);
void print_ir_print2(const char *op_str, int res_id, const char *res_name, int arg1_id, const char *arg1_name);
void print_ir_print3(const char *op_str, int res_id, const char *res_name, int arg1_id, const char *arg1_name,
	int arg2_id, const char *arg2_name);
void print_ir_op_binary(ir_t ir, const char *op_str);
void print_ir_op_unary(ir_t ir, const char *op_str);
void print_ir_op_label(ir_t ir);
void print_ir_op_copy(ir_t ir);

void ir_rule_prog(ast_t *ast);
void ir_rule_stmt(ast_t *ast);
void ir_rule_label_stmt(ast_t *ast);
void ir_rule_var_stmt(ast_t *ast);
int ir_rule_expr(ast_t *ast);
int ir_rule_literal(ast_t *ast);
int ir_rule_identifier(ast_t *ast);
int ir_rule_unary(ast_t *ast);
int ir_rule_binary(ast_t *ast);

int ir_generate_temp();
int ir_generate_literal(const char *literal);

// ========================================
// ir.h - definition
// ========================================

ir_t *generate_ir(ast_t *ast) {
	ir_init();

	ir_rule_prog(ast);

	return ir_list();
}

void print_ir(ir_t *ir_list) {
	ir_t *ir_ptr = ir_list;
	do {
		switch (ir_ptr->op) {
		case OP_ADD:
			print_ir_op_binary(*ir_ptr, "OP_ADD");
			break;
		case OP_SUB:
			print_ir_op_binary(*ir_ptr, "OP_SUB");
			break;
		case OP_MUL:
			print_ir_op_binary(*ir_ptr, "OP_MUL");
			break;
		case OP_DIV:
			print_ir_op_binary(*ir_ptr, "OP_DIV");
			break;
		case OP_MOD:
			print_ir_op_binary(*ir_ptr, "OP_MOD");
			break;
		case OP_LSHIFT:
			print_ir_op_binary(*ir_ptr, "OP_LSHIFT");
			break;
		case OP_RSHIFT:
			print_ir_op_binary(*ir_ptr, "OP_RSHIFT");
			break;
		case OP_EQUAL_EQUAL:
			print_ir_op_binary(*ir_ptr, "OP_EQUAL_EQUAL");
			break;
		case OP_NOT_EQUAL:
			print_ir_op_binary(*ir_ptr, "OP_NOT_EQUAL");
			break;
		case OP_LESSER:
			print_ir_op_binary(*ir_ptr, "OP_LESSER");
			break;
		case OP_LESSER_EQUAL:
			print_ir_op_binary(*ir_ptr, "OP_LESSER_EQUAL");
			break;
		case OP_GREATER:
			print_ir_op_binary(*ir_ptr, "OP_GREATER");
			break;
		case OP_GREATER_EQUAL:
			print_ir_op_binary(*ir_ptr, "OP_GREATER_EQUAL");
			break;
		case OP_LOGICAL_NOT:
			print_ir_op_unary(*ir_ptr, "OP_LOGICAL_NOT");
			break;
		case OP_BITWISE_NOT:
			print_ir_op_unary(*ir_ptr, "OP_BITWISE_NOT");
			break;
		case OP_LABEL:
			print_ir_op_label(*ir_ptr);
			break;
		case OP_COPY:
			print_ir_op_copy(*ir_ptr);
			break;
		case OP_END:
			printf("OP_END\n");
			break;
		default:
			fprintf(stderr, "T_T no idea man; what you sending\n");
			exit(1);
		}
		ir_ptr++;
	} while (ir_ptr->op != OP_END);
	printf("OP_END\n");
}

// ========================================
// helper definition
// ========================================

void ir_init() {
	g_ir_list = NULL;
	g_ir_list_len = 0;
	g_temp_len = 0;
}

ir_t *ir_list() {
	return g_ir_list;
}

void ir_emit(int op, int res_id, int arg1_id, int arg2_id) {
	g_ir_list_len++;
	g_ir_list = realloc(g_ir_list, g_ir_list_len * sizeof(ir_t));
	if (g_ir_list == NULL) {
		perror("something went wrong while realloc in ir_emit");
		exit(1);
	}
	g_ir_list[g_ir_list_len-1] = (ir_t) {.op=op, .res_id=res_id, .arg1_id=arg1_id, .arg2_id=arg2_id};
}

void print_ir_print1(const char *op_str, int res_id, const char *res_name) {
	printf("%-20s | %-5d %-10s\n", op_str, res_id, res_name);
}

void print_ir_print2(const char *op_str, int res_id, const char *res_name, int arg1_id, const char *arg1_name) {
	printf("%-20s | %-5d %-10s | %-5d %-10s\n", op_str, res_id, res_name, arg1_id, arg1_name);
}

void print_ir_print3(const char *op_str, int res_id, const char *res_name, int arg1_id, const char *arg1_name,
	int arg2_id, const char *arg2_name) {
	printf("%-20s | %-5d %-10s | %-5d %-10s | %-5d %-10s\n", op_str, res_id, res_name, arg1_id, arg1_name,
		arg2_id, arg2_name);
}

void print_ir_op_binary(ir_t ir, const char *op_str) {
	name_t res_name = st_check_var_by_id(ir.res_id);
	name_t arg1_name = st_check_var_by_id(ir.arg1_id);
	name_t arg2_name = st_check_var_by_id(ir.arg2_id);
	print_ir_print3(op_str, res_name.id, res_name.name, arg1_name.id, arg1_name.name, arg2_name.id, arg2_name.name);
}

void print_ir_op_unary(ir_t ir, const char *op_str) {
	name_t res_name = st_check_var_by_id(ir.res_id);
	name_t arg1_name = st_check_var_by_id(ir.arg1_id);
	print_ir_print2(op_str, res_name.id, res_name.name, arg1_name.id, arg1_name.name);
}

void print_ir_op_label(ir_t ir) {
	name_t res_name = st_check_label_by_id(ir.res_id);
	print_ir_print1("OP_LABEL", res_name.id, res_name.name);
}

void print_ir_op_copy(ir_t ir) {
	name_t res_name = st_check_var_by_id(ir.res_id);
	name_t arg1_name = st_check_var_by_id(ir.arg1_id);
	print_ir_print2("OP_COPY", res_name.id, res_name.name, arg1_name.id, arg1_name.name);
}

void ir_rule_prog(ast_t *ast) {
	for (int i = 0; i < ast->prog.len; i++) {
		ir_rule_stmt(ast->prog.stmts[i]);
	}

	ir_emit(OP_END, 0, 0, 0);
}

void ir_rule_stmt(ast_t *ast) {
	switch (ast->type) {
	case AST_LABEL_STMT:
		ir_rule_label_stmt(ast);
		break;
	case AST_VAR_STMT:
		ir_rule_var_stmt(ast);
		break;
	default:
		fprintf(stderr, "bruhhh, you shouldn't be here!\n");
		exit(1);
	}
}

void ir_rule_label_stmt(ast_t *ast) {
	char *lexical = token_lexical(ast->label_stmt.label);
	name_t n = st_check_label(lexical);
	ir_emit(OP_LABEL, n.id, 0, 0);
	free(lexical);
}

void ir_rule_var_stmt(ast_t *ast) {
	char *lexical = token_lexical(ast->var_stmt.name);

	name_t n = st_check_var(lexical);
	if (ast->var_stmt.expr) {
		int arg_id = ir_rule_expr(ast->var_stmt.expr);
		ir_emit(OP_COPY, n.id, arg_id, 0);
	}

	free(lexical);
}

int ir_rule_expr(ast_t *ast) {
	switch (ast->type) {
	case AST_LITERAL:
		return ir_rule_literal(ast);
	case AST_IDENTIFIER:
		return ir_rule_identifier(ast);
	case AST_UNARY:
		return ir_rule_unary(ast);
	case AST_BINARY:
		return ir_rule_binary(ast);
	default:
		fprintf(stderr, "yooo, how you here?\n");
		exit(1);
	}
}

int ir_rule_literal(ast_t *ast) {
	char *lexical = token_lexical(ast->literal.token);
	int id = ir_generate_literal(lexical);
	free(lexical);
	return id;
}

int ir_rule_identifier(ast_t *ast) {
	char *lexical = token_lexical(ast->identifier.token);
	int id = st_check_var(lexical).id;
	free(lexical);
	return id;
}

int ir_rule_unary(ast_t *ast) {
	int expr_id = ir_rule_expr(ast->unary.right);

	switch (ast->unary.op.type) {
	case TT_PLUS:
		return expr_id;
	case TT_MINUS: {
		int res_id = ir_generate_temp();
		int zero_literal = ir_generate_literal("0");
		ir_emit(OP_SUB, res_id, zero_literal, expr_id);
		return res_id;
	}
	case TT_PLUS_PLUS: {
		int res_id = ir_generate_temp();
		int one_literal = ir_generate_literal("1");
		ir_emit(OP_ADD, res_id, expr_id, one_literal);
		ir_emit(OP_COPY, expr_id, res_id, 0);
		return res_id;
	}
	case TT_MINUS_MINUS: {
		int res_id = ir_generate_temp();
		int one_literal = ir_generate_literal("1");
		ir_emit(OP_SUB, res_id, expr_id, one_literal);
		ir_emit(OP_COPY, expr_id, res_id, 0);
		return res_id;
	}
	case TT_BANG: {
		int res_id = ir_generate_temp();
		ir_emit(OP_LOGICAL_NOT, res_id, expr_id, 0);
		return res_id;
	}
	case TT_TILDE: {
		int res_id = ir_generate_temp();
		ir_emit(OP_BITWISE_NOT, res_id, expr_id, 0);
		return res_id;
	}
	default:
		fprintf(stderr, ">_<; don't sniff around!\n");
		exit(1);
	}
}

int ir_rule_binary(ast_t *ast) {
	int left_id = ir_rule_expr(ast->binary.left);
	int right_id = ir_rule_expr(ast->binary.right);
	switch (ast->binary.op.type) {
	case TT_STAR: {
		int res_id = ir_generate_temp();
		ir_emit(OP_MUL, res_id, left_id, right_id);
		return res_id;
	}
	case TT_FSLASH: {
		int res_id = ir_generate_temp();
		ir_emit(OP_DIV, res_id, left_id, right_id);
		return res_id;
	}
	case TT_MOD: {
		int res_id = ir_generate_temp();
		ir_emit(OP_MOD, res_id, left_id, right_id);
		return res_id;
	}
	case TT_PLUS: {
		int res_id = ir_generate_temp();
		ir_emit(OP_ADD, res_id, left_id, right_id);
		return res_id;
	}
	case TT_MINUS: {
		int res_id = ir_generate_temp();
		ir_emit(OP_SUB, res_id, left_id, right_id);
		return res_id;
	}
	case TT_LSHIFT: {
		int res_id = ir_generate_temp();
		ir_emit(OP_LSHIFT, res_id, left_id, right_id);
		return res_id;
	}
	case TT_RSHIFT: {
		int res_id = ir_generate_temp();
		ir_emit(OP_RSHIFT, res_id, left_id, right_id);
		return res_id;
	}
	case TT_EQUAL_EQUAL: {
		int res_id = ir_generate_temp();
		ir_emit(OP_EQUAL_EQUAL, res_id, left_id, right_id);
		return res_id;
	}
	case TT_BANG_EQUAL: {
		int res_id = ir_generate_temp();
		ir_emit(OP_NOT_EQUAL, res_id, left_id, right_id);
		return res_id;
	}
	case TT_LESSER: {
		int res_id = ir_generate_temp();
		ir_emit(OP_LESSER, res_id, left_id, right_id);
		return res_id;
	}
	case TT_LESSER_EQUAL: {
		int res_id = ir_generate_temp();
		ir_emit(OP_LESSER_EQUAL, res_id, left_id, right_id);
		return res_id;
	}
	case TT_GREATER: {
		int res_id = ir_generate_temp();
		ir_emit(OP_GREATER, res_id, left_id, right_id);
		return res_id;
	}
	case TT_GREATER_EQUAL: {
		int res_id = ir_generate_temp();
		ir_emit(OP_GREATER_EQUAL, res_id, left_id, right_id);
		return res_id;
	}
	default:
		fprintf(stderr, "invalidated ~.~\n");
		exit(1);
	}
}

int ir_generate_temp() {
	char buffer[1024];
	sprintf(buffer, ".TEMP_%d", ++g_temp_len);
	int id = st_create_var(buffer, st_check_type("int").id).id;
	return id;
}

int ir_generate_literal(const char *lexical) {
	const char *format = ".LITERAL_%s";

	int length = snprintf(NULL, 0, format, lexical);
	char *postfix_lexical = malloc((length + 1) * sizeof(char));
	sprintf(postfix_lexical, format, lexical);

	if (st_check_var(postfix_lexical).id == -1) {
		int type_id = st_check_type("int").id;
		st_create_var(postfix_lexical, type_id);
	}
	int id = st_check_var(postfix_lexical).id;

	free(postfix_lexical);

	return id;
}

