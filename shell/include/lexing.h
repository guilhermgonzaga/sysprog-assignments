/**
 * @file lexing.h
 * Description.
 */

#ifndef LEXING_H
#define LEXING_H


typedef struct token {
	char *begin;
	size_t length;
} token_t;


errcode_t tokenize_input();

errcode_t get_next_token(token_t *out);


#endif
