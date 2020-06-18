/**
 * @file main.c
 * Description.
 */

#include <common.h>
#include <io.h>
#include <lexing.h>


int main(int argc, const char *argv[], const char *envp[]) {
	set_input_stream(stdin);
	set_output_stream(stdout);

	while (read_cmdline() == ERR_NO_ERROR) {
		token_t tok;
		tokenize_input(cmdline_buffer);

		while (get_next_token(&tok) == ERR_NO_ERROR) {
			fprintf(stderr, "%.*s  :::  ", (int)tok.length, tok.begin);
		}
		fprintf(stderr, "\n");
	}

	return 0;
}
