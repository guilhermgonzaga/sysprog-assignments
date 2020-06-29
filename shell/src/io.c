#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <common.h>
#include <io.h>


char cmdline_buffer[ARG_MAX+1];

static FILE *input_stream;
static FILE *output_stream;


errcode_t set_input_stream(FILE *stream) {
	input_stream = stream;
	return ERR_NO_ERROR;
}

errcode_t set_output_stream(FILE *stream) {
	output_stream = stream;
	return ERR_NO_ERROR;
}

errcode_t read_cmdline() {
	if (fgets(cmdline_buffer, ARG_MAX + 1, input_stream) == NULL) {
		return ERR_INPUT;
	}

	int cmdline_len = strnlen(cmdline_buffer, ARG_MAX);

	// TODO handle trailing backslash for multiple input lines

	if (cmdline_buffer[cmdline_len-1] == '\n') {
		cmdline_buffer[cmdline_len-1] = '\0';
		// cmdline_len--;
	}

	return ERR_NO_ERROR;
}
