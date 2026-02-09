#ifndef ERROR_H
#define ERROR_H

#include "pos.h"

/**
 * Print error based on information provided
 *
 * Parameters:
 * 	filepath	filepath where the error occurred
 * 	src		Source code of where the error occurred
 * 	start		Start position of the error (included)
 * 	end		end position of the error (excluded)
 * 	message		Error message that needs to be printed
 */
void error_print(const char *filepath, const char *src, pos_t start, pos_t end, 
	const char *message);

#endif // ERROR_H
