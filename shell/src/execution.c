#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#include <common.h>
#include <builtin.h>
#include <execution.h>


char cwd[PATH_MAX+1];

job_t *firstjob = NULL;

int pipe_process = 0;


//launch jobs
errcode_t exec_external(char *argv[], _Bool wait_child, int *return_code) {
	errcode_t ret = ERR_NO_ERROR;
	char *path = getenv("MYPATH");
	char  pathenv[strlen(path) + sizeof("MYPATH=")];
	sprintf(pathenv, "MYPATH=%s", path);
	char *envp[] = {pathenv, NULL};
	
	int status, i;
	job_t *job, *aux = NULL;
	process_t *current_process;
	int mypipe[2], infile, outfile;

	job = malloc(sizeof(job_t));

	init_job(aux, job);

	launch_job(argv, job);

	current_process = job->first_process;

	infile = job->in;
	for(i = 0; current_process != NULL; i++) {
		if (current_process->next != NULL) {  // if exist other process in pipe
			if (pipe (mypipe) < 0)
			{
				perror ("pipe");
				exit(1);
			}
			outfile = mypipe[1];
		} else{
			outfile = job->out;
		}

		pid_t pid = fork();
		if (pid == 0) {
			launch_process(argv, current_process, envp, pid, infile, outfile, job->err);

			close(mypipe[1]);

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
		} else if (pid < 0) {
			perror ("fork");
			exit (1);
		}
		current_process = current_process->next;

		/* Clean up after pipes. */
		if (infile != job->in)
			close (infile);
		if (outfile != job->out)
			close (outfile);
		infile = mypipe[0];
	}

	return ret;
}


errcode_t launch_process(char *argv[], process_t *process, char *envp[], pid_t pgid, int infile, int outfile, int errfile) {
	pid_t pid;

	pid = getpid();

	if (pgid == 0) 
		pgid = pid;
	setpgid (pid, pgid);

	if (infile != STDIN_FILENO) {
		dup2 (infile, STDIN_FILENO);
		close (infile);
	}

	if (outfile != STDOUT_FILENO) {
		dup2 (outfile, STDOUT_FILENO);
		close (outfile);
	}

	execvpe(process->argv, argv, envp);
	perror("execvpe");
	exit(EXIT_SUCCESS);

	return ERR_NO_ERROR;
}


errcode_t exec_builtin(command_t cmd) {
	switch (cmd.name) {
	case CD:
		return cd(cmd);
	case ECHO:
		return echo(cmd);
	case EXPORT:
		return export(cmd);
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
	}
}


errcode_t launch_job(char *argv[], job_t *job) {
	process_t *new_process;
	process_t *aux =  NULL; 

	for (int i = 0; argv[i] != NULL; i++) {
		if (strcmp(argv[i], "|") == 0) {
			pipe_process = 1;
		} else {
			new_process = malloc (sizeof(process_t));
			new_process->completed = 0;
			new_process->stopped = 0;
			new_process->pid = getpid();
			new_process->argv = argv[i];
		
			if (aux == NULL) { //first process in job
				job->first_process = new_process;
			}else{
				aux->next = new_process;
			}
			aux = new_process;
		}
	}

	return ERR_NO_ERROR;
}


errcode_t init_job(job_t *aux, job_t *new_job) {
	if (aux == NULL)
		aux = new_job;
	else
		aux->next = new_job;

	new_job->in = 0;
	new_job->out = 1;
	new_job->pgid = getpid();

	if (firstjob == NULL)
		firstjob = new_job;

	return ERR_NO_ERROR;
}
