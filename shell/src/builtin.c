/**
 * @file builtin.c
 * Description.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>
#include <lexing.h>




errcode_t cd(command_t cmd) {
	static char lwd[PATH_MAX+1];  // Last working directory

	strcpy(lwd, cwd);
	// TODO Update cwd

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
    if(strcmp(arg, "$PATH") == 0){
        str = getenv("PATH");
        printf("Value of \"PATH Environmemt Variable\" : %s\n", str);
    }
    else if(strcmp(arg, "$LOGNAME") == 0){
        str = getenv("LOGNAME");
        printf("Value of \"LOGNAME Environmemt Variable\" : %s\n", str);
    }
    // else if(strcmp(arg, "$MYPATH") == 0){
    //     str = getenv("MYPATH");
    //     printf("Value of \"MYPATH Environmemt Variable\" : %s\n", str);
    // }
  }else{
    printf("%s\n", arg);
  }
}

errcode_t history(command_t cmd) {
	return ERR_NO_ERROR;
}

errcode_t send_signal(command_t cmd) {
	token_t token;
	char *arg, *str;

	get_next_token(&token);
	arg = strndup(token.begin, token.length);
	if(arg[0] == '-'){
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
				if (atoi(&arg[1]) == 17 || atoi(&arg[1]) == 19 || atoi(&arg[1]) == 23){
					get_next_token(&token);
					arg = strndup(token.begin, token.length);
					kill(atoi(arg), SIGSTOP);
				}else{
					printf("bash: kill: %s: especificacao de sinal invalida\n", strndup(token.begin+1, token.length));
				}
				break;
		}
	}else{
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
  // for (char **env = envp; *env != 0; env++)
  // {
  //   char *thisEnv = *env;
  //   printf("%s\n", thisEnv);
  // }
	return ERR_NO_ERROR;
}
