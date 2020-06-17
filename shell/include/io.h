/**
 * @file io.h
 * Description.
 */

#ifndef IO_H
#define IO_H


#include <stdio.h>

#define ARG_MAX 0x32000


extern char cmdline_buffer[ARG_MAX+1];


/**
 * Sets the given file descriptor as input stream for the process.
 * @param stream File descriptor to be used.
 */
errcode_t set_input_stream(FILE *stream);

/**
 * Sets the given file descriptor as output stream for the process.
 * @param stream File descriptor to be used.
 */
errcode_t set_output_stream(FILE *stream);

/**
 * Read a command line from the input buffer.
 * @return Number of characters read
 */
errcode_t read_cmdline();


#endif
