/**
 * @file common.h
 * Description.
 */

#ifndef COMMON_H
#define COMMON_H


enum builtin {
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
	OP_LT,
	OP_GT,
	OP_PIPE,
	OP_AMPERSAND,
};


#endif
