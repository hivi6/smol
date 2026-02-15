#include "st.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// helper declaration
// ========================================

static name_t *g_types;
static int g_types_len;
static name_t *g_labels;
static int g_labels_len;
static name_t *g_vars;
static int g_vars_len;

char *copy_str(const char *str);

// ========================================
// st.h - definition
// ========================================

void st_init() {
	g_types = NULL;
	g_types_len = 0;
	g_labels = NULL;
	g_labels_len = 0;
	g_vars = NULL;
	g_vars_len = 0;
}

void st_free() {
	for (int i = 0; i < g_types_len; i++) free(g_types[i].name);
	for (int i = 0; i < g_labels_len; i++) free(g_labels[i].name);
	for (int i = 0; i < g_vars_len; i++) free(g_vars[i].name);

	free(g_types);
	free(g_labels);
	free(g_vars);
}

name_t st_check_type(const char *name) {
	for (int i = 0; i < g_types_len; i++) {
		name_t n = g_types[i];
		if (strcmp(name, n.name) == 0) {
			return n;
		}
	}
	return (name_t) {.id=-1};
}

name_t st_create_type(const char *name) {
	g_types_len++;
	g_types = realloc(g_types, g_types_len * sizeof(name_t));
	if (g_types == NULL) {
		perror("something went wrong with realloc in st_create_type");
		exit(1);
	}

	char *name_copy = copy_str(name);
	g_types[g_types_len-1] = (name_t) {.id = g_types_len, .name = name_copy, .type_id=-1};
	return g_types[g_types_len-1];
}

name_t st_check_label(const char *name) {
	for (int i = 0; i < g_labels_len; i++) {
		name_t n = g_labels[i];
		if (strcmp(name, n.name) == 0) {
			return n;
		}
	}
	return (name_t) {.id=-1};
}

name_t st_create_label(const char *name) {
	g_labels_len++;
	g_labels = realloc(g_labels, g_labels_len * sizeof(name_t));
	if (g_types == NULL) {
		perror("something went wrong with realloc in st_create_label");
		exit(1);
	}

	char *name_copy = copy_str(name);
	g_labels[g_labels_len-1] = (name_t) {.id = g_labels_len, .name = name_copy, .type_id=-1};
	return g_labels[g_labels_len-1];
}

name_t st_check_var(const char *name) {
	for (int i = 0; i < g_vars_len; i++) {
		name_t n = g_vars[i];
		if (strcmp(name, n.name) == 0) {
			return n;
		}
	}
	return (name_t) {.id=-1};
}

name_t st_create_var(const char *name, int type_id) {
	g_vars_len++;
	g_vars = realloc(g_vars, g_vars_len * sizeof(name_t));
	if (g_types == NULL) {
		perror("something went wrong with realloc in st_create_var");
		exit(1);
	}

	char *name_copy = copy_str(name);
	g_vars[g_vars_len-1] = (name_t) {.id = g_vars_len, .name = name_copy, .type_id=type_id};
	return g_vars[g_vars_len-1];
}

// ========================================
// helper definition
// ========================================

char *copy_str(const char *str) {
	int len = strlen(str);
	char *res = malloc((len + 1) * sizeof(char));
	for (int i = 0; i < len; i++) res[i] = str[i];
	res[len] = 0;
	return res;
}

