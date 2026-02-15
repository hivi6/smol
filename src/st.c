#include "st.h"

#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

typedef struct {
	int id;
	const char *name;
	int len;
} name_t;

static name_t *g_types;
static int g_types_len;
static name_t *g_labels;
static int g_labels_len;

// ========================================
// st.h - definition
// ========================================

void st_init() {
	g_types = NULL;
	g_types_len = 0;
	g_labels = NULL;
	g_labels_len = 0;
}

void st_free() {
	free(g_types);
}

int st_check_type(const char *name, int len) {
	for (int i = 0; i < g_types_len; i++) {
		name_t n = g_types[i];
		if (n.len == len && strncmp(name, n.name, len) == 0) {
			return n.id;
		}
	}
	return -1;
}

int st_create_type(const char *name, int len) {
	g_types_len++;
	g_types = realloc(g_types, g_types_len * sizeof(name_t));
	g_types[g_types_len-1] = (name_t) {.id = g_types_len, .name = name, .len = len};
	return g_types_len;
}

int st_check_label(const char *name, int len) {
	for (int i = 0; i < g_labels_len; i++) {
		name_t n = g_labels[i];
		if (n.len == len && strncmp(name, n.name, len) == 0) {
			return n.id;
		}
	}
	return -1;
}

int st_create_label(const char *name, int len) {
	g_labels_len++;
	g_labels = realloc(g_labels, g_labels_len * sizeof(name_t));
	g_labels[g_labels_len-1] = (name_t) {.id = g_labels_len, .name = name, .len = len};
	return g_labels_len;
}

// ========================================
// helper definition
// ========================================

