#ifndef IR_H
#define IR_H

#include "ast.h"

enum {
	OP_ADD = 0,	// Arguments are variable id; Result is a variable id
	OP_SUB,		// Arguments are variable id; Result is a variable id
	OP_MUL,		// Arguments are variable id; Result is a variable id
	OP_DIV,		// Arguments are variable id; Result is a variable id
	OP_MOD,		// Arguments are variable id; Result is a variable id
	OP_LSHIFT,	// Arguments are variable id; Result is a variable id
	OP_RSHIFT,	// Arguments are variable id; Result is a variable id
	OP_LOGICAL_NOT,	// 1st argument is variable id; Result is a variable id
	OP_BITWISE_NOT,	// 1st argument is variable id; Result is a variable id


	OP_LABEL,	// No arguments; Result is a label id
	OP_JMP,		// No arguments; Result is a label id
	OP_JMP_EQ,	// Arguments are variable ids; Result is a label id
	OP_JMP_NE,	// Arguments are variable ids; Result is a label id
	OP_JMP_LT,	// Arguments are variable ids; Result is a label id

	OP_COPY,	// 1st argument is variable id; Result is a variable id

	OP_END,		// End of the instructions
};

typedef struct {
	int op;		// Operation
	int res_id;	// Result; (variable id or label id)
	int arg1_id;	// 1st argument; (variable id)
	int arg2_id;	// 2nd argument; (variable id)
} ir_t;

/**
 * Generate the Intermediate Representation
 *
 * Parameters:
 * 	ast	The ast
 * 
 * Returns:
 * 	Array of intermediate representation
 */
ir_t *generate_ir(ast_t *ast);

/**
 * Print the list of intermediate representation
 * 
 * Parameters:
 * 	ir_list	List of ir
 */
void print_ir(ir_t *ir_list);

#endif // IR_H
