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
 * 	len	Length of the name string
 *
 * Returns:
 * 	type id
 */
int st_create_type(const char *name, int len);

/**
 * Check if the type exists
 *
 * Parameters:
 * 	name	Name of a type
 * 	len	Length of the name string
 *
 * Returns:
 * 	type id (-1 if doesn't exists)
 */
int st_check_type(const char *name, int len);

/**
 * Create a new label
 *
 * Parameters:
 *	name	Name of the label
 *	len	Length of the name string
 *
 * Returns:
 * 	label id
 */
int st_create_label(const char *name, int len);

/**
 * Check if label exists
 *
 * Parameters:
 * 	name	Name of the label
 * 	len	Length of the name string
 *
 * Returns:
 * 	-1 (invalid label)
 * 	non-negative integer for valid label id
 */
int st_check_label(const char *name, int len);

#endif // ST_H

