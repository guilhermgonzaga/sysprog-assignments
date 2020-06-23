/**
 * @file execution.c
 * Description.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>


char cwd[PATH_MAX+1];


errcode_t exec_external(char *argv[], _Bool wait_completion, int *return_code) {
	errcode_t ret = ERR_NO_ERROR;
	pid_t pid = fork();

	if (pid == 0) {
		execvp(argv[0], argv);
		// TODO print error message
		exit(EXIT_FAILURE);  // Error: exec* should not return
	}
	else if (wait_completion) {
		int status;
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


errcode_t exec_builtin(command_t cmd) {
	switch (cmd.name) {
	case CD:
		return cd(cmd);
	case ECHO:
		return echo(cmd);
	case FG:
		return fg(cmd);
	case BG:
		return bg(cmd);
	case HISTORY:
		return history(cmd);
	case JOBS:
		return jobs(cmd);
	case KILL:
		return send_signal(cmd);
	case SET:
		return set(cmd);
	default:
		return ERR_GENERIC;
		break;
	}
}
