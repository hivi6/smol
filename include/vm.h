#ifndef VM_H
#define VM_H

#include "ir.h"

/**
 * Run the instruction list in the vm
 * 
 * Parameters:
 * 	ir_list	list of ir
 */
void vm_run(ir_t *ir_list);

#endif // VM_H