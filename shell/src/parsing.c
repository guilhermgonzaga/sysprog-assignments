#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <common.h>
#include <parsing.h>
#include <builtin.h>
#include <execution.h>


errcode_t parse(char *argv, char *argvs[]) {
	errcode_t ret = -1;

	if (strcmp(argv, "echo") == 0)
		ret = exec_builtin((command_t){ECHO});
	else if (strcmp(argv, "kill") == 0)
		ret = exec_builtin((command_t){KILL});
	else if (strcmp(argv, "cd") == 0)
		ret = exec_builtin((command_t){CD});
	else if (strcmp(argv, "fg") == 0)
		ret = exec_builtin((command_t){FG});
	else if (strcmp(argv, "export") == 0)
		ret = exec_builtin((command_t){EXPORT});
	else if (strcmp(argv, "history") == 0)
		ret = exec_builtin((command_t){HISTORY});
	else if (strcmp(argv, "jobs") == 0)
		ret = exec_builtin((command_t){JOBS});
	// else
	// 	ret = exec_external(argvs, 1, NULL);

	return ret;
}
