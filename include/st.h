#ifndef ST_H
#define ST_H

/**
 * initialize the symbol table
 */
void st_init();

/**
 * Free the symbol table
 */
void st_free();

/**
 * Create a new type
 *
 * Parameters:
 * 	name	Name of a type
 *
 * Returns:
 * 	type id
 */
int st_create_type(const char *name);

#endif // ST_H

