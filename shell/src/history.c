#include <stdlib.h>
#include <string.h>

#include <common.h>
#include <history.h>


char *history_buffer[HISTORY_SIZE];
static size_t latest = 0;


static errcode_t reserve_space(char **entry, size_t new_capacity);


size_t history_latest(void) {
	return latest;
}


errcode_t history_append(const char *cmdline) {
	size_t length = strlen(cmdline);

	if (length == 0) {
		return ERR_GENERIC;
	}

	latest = (latest + 1) % HISTORY_SIZE;
	if (reserve_space(&history_buffer[latest], length + 1) != ERR_MEMORY)
		strcpy(history_buffer[latest], cmdline);

	return ERR_NO_ERROR;
}


errcode_t history_free() {
	for (size_t i = 0; i < HISTORY_SIZE; ++i) {
		free(history_buffer[i]);
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
