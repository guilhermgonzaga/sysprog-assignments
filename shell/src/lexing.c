#include <string.h>

#include <common.h>
#include <lexing.h>


static const char token_delims[] = " \"\'&;<>|";
static char *next_token;


errcode_t tokenize_input(char *input) {
	next_token = input;
	return ERR_NO_ERROR;
}


errcode_t get_next_token(token_t *out) {
	errcode_t ret = ERR_NO_ERROR;
	next_token += strspn(next_token, " ");

	token_t token = {
		.begin = next_token,
		.length = strcspn(next_token, token_delims)
	};

	char *end = token.begin + token.length;

	switch (*end) {
	case ' ':
		break;

	case '\"':
	case '\'':
		token.length += 2 + strcspn(end+1, (char [2]) {*end, '\0'});

		if (token.begin[token.length] != '\0')
			token.length += strcspn(token.begin + token.length, token_delims);
		else  // Error
			ret = ERR_QUOTE_MISMATCH;

		end = token.begin + token.length;
		break;

	case '&':
	case ';':
	case '<':
	case '>':
	case '|':
		if (token.length == 0) {
			token.length++;
			end++;
		}
		break;

	case '\0':
		if (*token.begin == '\0')  // No remaining tokens
			ret = ERR_END_OF_DATA;
		break;

	default:
		// TODO treat error
		break;
	}

	next_token = end;
	*out = token;

	return ret;
}
