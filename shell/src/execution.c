/**
 * @file execution.c
 * Description.
 */

#include <stdlib.h>

#include <common.h>
#include <execution.h>
#include <builtin.h>


char cwd[PATH_MAX+1];


errcode_t exec_external(char *argv[], _Bool wait_child, int *return_code) {
	errcode_t ret = ERR_NO_ERROR;
	int status;
	pid_t pid = fork();

	if (pid == 0) {
		execvp(argv[0], argv);
		exit(EXIT_SUCCESS);
	}
	else if (wait_child) {
		waitpid(pid, &status, 0);

		if (!WIFEXITED(status)) {
			ret = ERR_GENERIC;
		}

		if (return_code != NULL) {
			*return_code = WEXITSTATUS(status);
		}
	}

	return ret;
}


errcode_t exec_builtin(enum cmd command) {
	switch (command) {
	case CMD_CD:
		return cd();
	case CMD_ECHO:
		return echo();
	case CMD_HISTORY:
		return history();
	case CMD_KILL:
		return kill();
	case CMD_JOBS:
		return jobs();
	case CMD_FG:
		return fg();
	case CMD_BG:
		return bg();
	case CMD_SET:
		return set();
	default:
		// Error
		break;
	}

	return ERR_NO_ERROR;
}


// TODO search executables in evironment variables
