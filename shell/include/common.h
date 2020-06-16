/**
 * @file common.h
 * Description.
 */

#ifndef COMMON_H
#define COMMON_H


typedef enum {
	ERR_NO_ERROR,
	ERR_GENERIC,
	ERR_INPUT,
	ERR_OUTPUT,
	ERR_MEMORY,
	ERR_END_OF_DATA,
	ERR_INVALID_PARAM,
	ERR_INVALID_OP,
	ERR_QUOTE_MISMATCH,
} errcode_t;


#endif
