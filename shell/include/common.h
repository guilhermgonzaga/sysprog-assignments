#ifndef COMMON_H
#define COMMON_H


#define ARG_MAX 0x32000

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
	OP_IN,
	OP_OUT,
	OP_PIPE,
};


#endif
