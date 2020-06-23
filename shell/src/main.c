/**
 * @file main.c
 * Description.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>
#include <io.h>
#include <lexing.h>


char *username;
char hostname[HOSTNAME_MAX+1];


void make_args(char ***args, size_t *capacity) {
	size_t i = 0;
	token_t token;
	tokenize_input(cmdline_buffer);

	while (get_next_token(&token) == ERR_NO_ERROR) {
		if (i + 1 >= *capacity) {
			*capacity *= 2;
			*args = realloc(*args, *capacity * sizeof(char *));  // TODO handle NULL return
		}
		else {
			free((*args)[i]);
		}

		(*args)[i] = strndup(token.begin, token.length);

		i++;
	}
	(*args)[i] = NULL;

	for (size_t j = 0; j < *capacity; j++) {
		fprintf(stderr, "%p\t%s\n", (*args)[j], (*args)[j]);
	}
}


int main(int argc, const char *argv[], const char *envp[]) {
	char **args = NULL;
	size_t capacity = 1;

	set_input_stream(stdin);
	set_output_stream(stdout);

	//getlogin_r(username, USERNAME_MAX);
	username = getenv("LOGNAME");
	gethostname(hostname, HOSTNAME_MAX);
	getcwd(cwd, sizeof(cwd));

	printf("%s@%s:%s$ ", username, hostname, cwd);
	while (read_cmdline() == ERR_NO_ERROR) {
		make_args(&args, &capacity);

		exec_external(args, 1, NULL);

		printf("%s@%s:%s$ ", username, hostname, cwd);
	}
	printf("\n");

	for (size_t i = 0; i < capacity; i++) {
		free(args[i]);
	}
	free(args);

	return EXIT_SUCCESS;
}
