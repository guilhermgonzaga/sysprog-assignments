#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>
#include <history.h>
#include <lexing.h>


errcode_t cd(command_t cmd) {
	static char lwd[PATH_MAX+1];  // Last working directory
	token_t token;
	char *arg;
	char aux[PATH_MAX+1] = "/";
	char PWD[PATH_MAX+1] = "PWD=";
	DIR* dir;

	get_next_token(&token);
	arg = strndup(token.begin, token.length);

	strcpy(lwd, cwd);

	if (strcmp(arg, "..") == 0) {
		chdir("..");
		getcwd(cwd, sizeof(cwd));
		strcat(PWD, cwd);
		putenv(PWD);
	} else {
		dir = opendir(arg);
		if (dir) {
			strcat(aux, arg);
			strcat(cwd, aux);
			strcat(PWD, cwd);
			chdir(cwd);
			putenv(PWD);
			closedir(dir);
		} else if (ENOENT == errno) {
			printf("bash: cd: %s: Arquivo ou diretório inexistente\n",arg);
		}
	}

	if (chdir(cwd) != 0) {
		return ERR_GENERIC;
	}

	return ERR_NO_ERROR;
}


errcode_t echo(command_t cmd) {
token_t token;
	char *arg, *str;

	get_next_token(&token);
	arg = strndup(token.begin, token.length);

	if(arg[0] == '$'){
		arg = strndup(token.begin+1, token.length);
		str = getenv(arg);
		printf("%s\n",arg);
		if(str != NULL){
			printf("Value of \"%s Environmemt Variable\" : %s\n", arg, str);
		}
	} else {
		printf("%s\n", arg);
	}
	return ERR_NO_ERROR;
}


errcode_t export(command_t cmd) {
	token_t token;
	char *arg, *p = NULL;
	int ret = 1;

	get_next_token(&token);
	arg = strndup(token.begin, token.length);

	p = strstr (arg,"=");
	if (p != NULL) {
		ret = putenv(arg);
	}

	if (ret != 0) {
		printf("bash: export: `%s': nao é um identificador valido\n", arg);
	}

	return ERR_NO_ERROR;
}


errcode_t history(command_t cmd) {
	size_t latest = history_latest();
	size_t i = latest;

	do {
		i = (i + 1) % HISTORY_SIZE;

		if (history_buffer[i] != NULL)
			printf("%s\n", history_buffer[i]);
	} while (i != latest);

	return ERR_NO_ERROR;
}


errcode_t send_signal(command_t cmd) {
	token_t token;
	char *arg;

	get_next_token(&token);
	arg = strndup(token.begin, token.length);

	if (arg[0] == '-') {
		switch (atoi(&arg[1]))
		{
		case 1:
			get_next_token(&token);
			arg = strndup(token.begin, token.length);
			kill(atoi(arg), SIGHUP);
			break;
		case 2:
			get_next_token(&token);
			arg = strndup(token.begin, token.length);
			kill(atoi(arg), SIGINT);
			break;
		case 9:
			get_next_token(&token);
			arg = strndup(token.begin, token.length);
			kill(atoi(arg), SIGKILL);
			break;
		case 15:
			get_next_token(&token);
			arg = strndup(token.begin, token.length);
			kill(atoi(arg), SIGTERM);
			break;
		default:
			if (atoi(&arg[1]) == 17 || atoi(&arg[1]) == 19 || atoi(&arg[1]) == 23) {
				get_next_token(&token);
				arg = strndup(token.begin, token.length);
				kill(atoi(arg), SIGSTOP);
			} else {
				printf("bash: kill: %s: especificacao de sinal invalida\n", strndup(token.begin+1, token.length));
			}
			break;
		}
	} else {
		printf("bash: kill: %s: argumentos devem ser IDs de trabalhos ou processos\n", arg);
	}

	return ERR_NO_ERROR;
}


errcode_t jobs(command_t cmd) {
	return ERR_NO_ERROR;
}


errcode_t fg(command_t cmd) {
	return ERR_NO_ERROR;
}


errcode_t bg(command_t cmd) {
	return ERR_NO_ERROR;
}


errcode_t set(command_t cmd) {
	return ERR_NO_ERROR;
}
