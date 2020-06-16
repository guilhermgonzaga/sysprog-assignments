/**
 * @file history.c
 * Description.
 */

#include <stdlib.h>
#include <string.h>

#include <common.h>
#include <history.h>


static struct history {
	char *cmdline;
	size_t capacity;
} history[HISTORY_SIZE];


static errcode_t reserve_space(struct history *entry, size_t new_capacity);


errcode_t history_append(const char *cmdline) {
	static size_t latest = 0;
	size_t length = strlen(cmdline);

	latest = (latest + 1) % HISTORY_SIZE;
	reserve_space(&history[latest], length + 1);
	strncpy(history[HISTORY_SIZE].cmdline, cmdline, length + 1);

	return ERR_NO_ERROR;
}


errcode_t history_free() {
	for (size_t i = 0; i < HISTORY_SIZE; ++i) {
		if (history[i].capacity > 0)
			free(history[i].cmdline);
	}

	return ERR_NO_ERROR;
}


static errcode_t reserve_space(struct history *entry, size_t new_capacity) {
	if (new_capacity > entry->capacity) {
		char *new_entry = realloc(entry->cmdline, new_capacity);

		if (new_entry == NULL) {
			free(entry->cmdline);
			entry->cmdline = NULL;
			entry->capacity = 0;

			return ERR_MEMORY;
		}

		entry->cmdline = new_entry;
		entry->capacity = new_capacity;
	}

	return ERR_NO_ERROR;
}
