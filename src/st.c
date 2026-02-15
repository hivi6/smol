#include "st.h"

#include <stdlib.h>

// ========================================
// helper declaration
// ========================================

typedef struct {
	int id;
	const char *name;
} type_t;

static type_t *g_types;
int g_types_len;

// ========================================
// st.h - definition
// ========================================

void st_init() {
	g_types = NULL;
	g_types_len = 0;
}

void st_free() {
	free(g_types);
}

int st_create_type(const char *name) {
	g_types_len++;
	g_types = realloc(g_types, g_types_len * sizeof(type_t));
	g_types[g_types_len] = (type_t) {.id = g_types_len, .name = name};
	return g_types_len;
}

// ========================================
// helper definition
// ========================================

