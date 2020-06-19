/**
 * @file history.h
 * Description.
 */

#ifndef HISTORY_H
#define HISTORY_H


#define HISTORY_SIZE 50


extern char *history_buffer[HISTORY_SIZE];


errcode_t history_append(const char *cmdline);

errcode_t history_free(void);

size_t history_latest(void);


#endif
