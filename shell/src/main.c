#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>
#include <history.h>
#include <io.h>
#include <lexing.h>
#include <parsing.h>


char *username;
char hostname[HOSTNAME_MAX+1];

int exec_extrn = 0;

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
		if(parse((*args)[i], *args) == -1)
			exec_extrn = 1;
		i++;
	}

	(*args)[i] = NULL;
}


int main(int argc, const char *argv[], const char *envp[]) {
	char **args = NULL;
	size_t capacity = 1;
	char MYPATH[PATH_MAX+1] = "MYPATH=";

	strcat(MYPATH, getenv("PATH"));
	putenv(MYPATH);

	set_input_stream(stdin);
	set_output_stream(stdout);

	username = getenv("LOGNAME");
	gethostname(hostname, HOSTNAME_MAX);
	getcwd(cwd, sizeof(cwd));

	printf("%s@%s:%s$ ", username, hostname, cwd);
	while (read_cmdline() == ERR_NO_ERROR) {
		history_append(cmdline_buffer);
		make_args(&args, &capacity);
		if(exec_extrn)
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
