/**
 * @file history.h
 * Description.
 */

#ifndef HISTORY_H
#define HISTORY_H


#define HISTORY_SIZE 50


errcode_t history_append(const char *cmdline);

errcode_t history_free();


#endif
