/**
 * @file execution.h
 * Description.
 */

#ifndef EXECUTION_H
#define EXECUTION_H


#define USERNAME_MAX 32
#define HOSTNAME_MAX 64


extern char username[USERNAME_MAX+1];
extern char hostname[HOSTNAME_MAX+1];
extern char cwd[PATH_MAX+1];


errcode_t exec_external(char *argv[], _Bool wait_child, int *return_code);


#endif
