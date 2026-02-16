#include "vm.h"
#include "st.h"

#include <stdio.h>
#include <stdlib.h>

// ========================================
// helper definition
// ========================================

static int *g_vars;
static int g_vars_len;
static ir_t **g_labels;
static int g_labels_len;

void vm_init(ir_t *ir_list);
void vm_free();

void vm_set_var(int id, int value);
int vm_get_var(int id);
void vm_set_label(int id, ir_t *ir_ptr);
ir_t *vm_get_label(int id);

// ========================================
// vm.h - definition
// ========================================

void vm_run(ir_t *ir_list) {
	vm_init(ir_list);

	ir_t *ip = ir_list;
	int running = 1;
	while (running) {
		switch (ip->op) {
		case OP_ADD: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left + right);
			break;
		}
		case OP_SUB: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left - right);
			break;
		}
		case OP_MUL: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left * right);
			break;
		}
		case OP_DIV: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left / right);
			break;
		}
		case OP_MOD: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left % right);
			break;
		}
		case OP_LSHIFT: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left << right);
			break;
		}
		case OP_RSHIFT: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left >> right);
			break;
		}
		case OP_EQUAL_EQUAL: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left == right);
			break;
		}
		case OP_NOT_EQUAL: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left != right);
			break;
		}
		case OP_LESSER: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left < right);
			break;
		}
		case OP_LESSER_EQUAL: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left <= right);
			break;
		}
		case OP_GREATER: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left > right);
			break;
		}
		case OP_GREATER_EQUAL: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left >= right);
			break;
		}
		case OP_BITWISE_AND: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left & right);
			break;
		}
		case OP_BITWISE_OR: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left | right);
			break;
		}
		case OP_BITWISE_XOR: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left ^ right);
			break;
		}
		case OP_LOGICAL_AND: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left && right);
			break;
		}
		case OP_LOGICAL_OR: {
			int left = vm_get_var(ip->arg1_id);
			int right = vm_get_var(ip->arg2_id);
			vm_set_var(ip->res_id, left || right);
			break;
		}
		case OP_LOGICAL_NOT: {
			int left = vm_get_var(ip->arg1_id);
			vm_set_var(ip->res_id, !left);
			break;
		}
		case OP_BITWISE_NOT: {
			int left = vm_get_var(ip->arg1_id);
			vm_set_var(ip->res_id, ~left);
			break;
		}
		case OP_LABEL:
			break;
		case OP_JMP: {
			ip = vm_get_label(ip->res_id);
			continue;
		}
		case OP_JMP_TRUE: {
			int left = vm_get_var(ip->arg1_id);
			if (left) ip = vm_get_label(ip->res_id);
			else ip++;
			continue;
		}
		case OP_JMP_FALSE: {
			int left = vm_get_var(ip->arg1_id);
			if (!left) ip = vm_get_label(ip->res_id);
			else ip++;
			continue;
		}
		case OP_COPY: {
			int left = vm_get_var(ip->arg1_id);
			vm_set_var(ip->res_id, left);
			break;
		}
		case OP_PRINT: {
			int res = vm_get_var(ip->res_id);
			printf("%d\n", res);
			break;
		}
		case OP_END:
			running = 0;
			break;
		default:
			fprintf(stderr, "shouldn't reach here >.>\n");
			exit(1);
		}
		ip++;
	}

	vm_free();
}

// ========================================
// helper declaration
// ========================================

void vm_init(ir_t *ir_list) {
	g_vars = NULL;
	g_vars_len = 0;

	for (int i = 1; st_check_var_by_id(i).id != -1; i++) {
		name_t n = st_check_var_by_id(i);
		int literal = -1;
		sscanf(n.name, ".LITERAL_%d", &literal);
		if (literal != -1) {
			vm_set_var(n.id, literal);
		}
		else {
			vm_set_var(n.id, 0);
		}
	}

	for (ir_t *ir_ptr = ir_list; ir_ptr->op != OP_END; ir_ptr++) {
		if (ir_ptr->op == OP_LABEL) {
			vm_set_label(ir_ptr->res_id, ir_ptr);
		}
	}
}

void vm_free() {
	free(g_vars);
}

void vm_set_var(int id, int value) {
	if (id >= g_vars_len) {
		g_vars_len = (id + 1) * 2;
		g_vars = realloc(g_vars, g_vars_len * sizeof(int));
	}
	g_vars[id] = value;
}

int vm_get_var(int id) {
	return g_vars[id];
}

void vm_set_label(int id, ir_t *ir_ptr) {
	if (id >= g_labels_len) {
		g_labels_len = (id + 1) * 2;
		g_labels = realloc(g_labels, g_labels_len * sizeof(ir_t*));
	}
	g_labels[id] = ir_ptr;
}

ir_t *vm_get_label(int id) {
	return g_labels[id];
}
