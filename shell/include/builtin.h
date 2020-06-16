/**
 * @file builtin.h
 * Description.
 */

#ifndef BUILTIN_H
#define BUILTIN_H


enum cmd {
	CMD_CD,
	CMD_ECHO,
	CMD_HISTORY,
	CMD_KILL,
	CMD_JOBS,
	CMD_FG,
	CMD_BG,
	CMD_SET,
};

enum op {
	OP_AMPERSAND,
	OP_SEMICOLON,
	OP_LT,
	OP_GT,
	OP_PIPE,
};


#endif
