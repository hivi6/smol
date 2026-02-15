#ifndef ST_H
#define ST_H

typedef struct {
	int id;
	char *name;
	int type_id;
} name_t;

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
 * 	name_t type
 */
name_t st_create_type(const char *name);

/**
 * Check if the type exists
 *
 * Parameters:
 * 	name	Name of a type
 *
 * Returns:
 * 	name_t type (id = -1 if doesn't exists)
 */
name_t st_check_type(const char *name);

/**
 * Create a new label
 *
 * Parameters:
 *	name	Name of the label
 *
 * Returns:
 * 	name_t type 
 */
name_t st_create_label(const char *name);

/**
 * Check if label exists
 *
 * Parameters:
 * 	name	Name of the label
 *
 * Returns:
 * 	name_t type (id = -1 if doesn't exists)
 */
name_t st_check_label(const char *name);

/**
 * Create a new variable name
 *
 * Parameters:
 * 	name	Name of the variable
 * 	type_id	Type of the variable
 *
 * Returns:
 * 	name_t type
 */
name_t st_create_var(const char *name, int type_id);

/**
 * Check if variable name exists
 *
 * Parameters:
 * 	name	Name of the variable
 *
 * Returns:
 * 	name_t type (id = -1 if doesn't exists)
 */
name_t st_check_var(const char *name);

#endif // ST_H

