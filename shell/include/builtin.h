/**
 * @file builtin.h
 * Description.
 */

#ifndef BUILTIN_H
#define BUILTIN_H


typedef struct command {
	enum {
		CD,
		ECHO,
		HISTORY,
		KILL,
		JOBS,
		FG,
		BG,
		SET,
	} name;
} command_t;

enum op {
	OP_AMPERSAND,
	OP_SEMICOLON,
	OP_LT,
	OP_GT,
	OP_PIPE,
};

errcode_t cd(command_t cmd);
errcode_t echo(command_t cmd);
errcode_t fg(command_t cmd);
errcode_t bg(command_t cmd);
errcode_t history(command_t cmd);
errcode_t jobs(command_t cmd);
errcode_t send_signal(command_t cmd);
errcode_t set(command_t cmd);

#endif
