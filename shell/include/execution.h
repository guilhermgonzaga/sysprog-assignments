/**
 * @file execution.h
 * Description.
 */

#ifndef EXECUTION_H
#define EXECUTION_H


#define USERNAME_MAX 32
#define HOSTNAME_MAX 64


/* A process is a single process. */
typedef struct process {
	_Bool completed;       // true if process has completed
	_Bool stopped;         // true if process has stopped
	int status;            // reported status value
	pid_t pid;             // process ID
	char *argv;           // for exec
	struct process *next;  // next process in pipeline
} process_t;

/* A job is a pipeline of processes. */
typedef struct job {
	_Bool notified;            // true if user told about stopped job
	int in, out, err;          // IO channels
	pid_t pgid;                // process group ID
	char *command;             // command line, used for messages
	process_t *first_process;  // list of processes in this job
	struct job *next;          // next active job
} job_t;


extern char *username;
extern char hostname[HOSTNAME_MAX+1];

extern char cwd[PATH_MAX+1];  // Current working directory


errcode_t exec_external(char *argv[], _Bool wait_child, int *return_code);

errcode_t exec_builtin(command_t cmd);

errcode_t launch_process(char *argv[], process_t *process, char *envp[],pid_t pgid, int infile, int outfile, int errfile);

errcode_t launch_job(char *argv[], job_t *job);

errcode_t init_job(job_t *aux, job_t *new_job);


#endif
