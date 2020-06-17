/**
 * @file history.c
 * Description.
 */

#include <stdlib.h>
#include <string.h>

#include <common.h>
#include <history.h>


char *history[HISTORY_SIZE];
static size_t latest = 0;


static errcode_t reserve_space(char **entry, size_t new_capacity);


size_t history_latest(void) {
	return latest;
}


errcode_t history_append(const char *cmdline) {
	size_t length = strlen(cmdline);

	latest = (latest + 1) % HISTORY_SIZE;
	reserve_space(&history[latest], length + 1);
	strcpy(history[HISTORY_SIZE], cmdline);

	return ERR_NO_ERROR;
}


errcode_t history_free() {
	for (size_t i = 0; i < HISTORY_SIZE; ++i) {
		free(history[i]);
	}

	return ERR_NO_ERROR;
}


static errcode_t reserve_space(char **entry, size_t new_capacity) {
	char *new_entry = realloc(*entry, new_capacity);

	if (new_entry == NULL) {
		free(*entry);
		*entry = NULL;

		return ERR_MEMORY;
	}

	*entry = new_entry;

	return ERR_NO_ERROR;
}
