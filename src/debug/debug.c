/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: debug.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 12:57:53 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 13:35:50
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "memory_public.h"
#include "debug.h"

static debug_controller_tp   st_current_controller;
static char     *st_current_file_name;
static int      st_current_line;
static char     *st_assert_expression;

static struct debug_controller_tag st_default_controller = {
    NULL, /*stderr,*/
    INT_MAX,
};
debug_controller_tp  debug_default_controller = &st_default_controller;

debug_controller_tp
debug_create_controller_func(void)
{
    debug_controller_tp      controller;

    controller = mem_malloc(sizeof(struct debug_controller_tag));
    controller->debug_write_fp = NULL;
    controller->current_debug_level = INT_MAX;

    return controller;
}

void
debug_set_debug_level_func(debug_controller_tp controller, int level)
{
    controller->current_debug_level = level;
}

void
debug_set_debug_write_fp_func(debug_controller_tp controller, FILE *fp)
{
    controller->debug_write_fp = fp;
}

static void
initialize_debug_write_fp(void)
{
    if (st_default_controller.debug_write_fp == NULL) {
        st_default_controller.debug_write_fp = stderr;
    }
}

static void
assert_func(FILE *fp, char *file, int line, char *expression,
            char *fmt,  va_list ap)
{
    fprintf(fp, "Assertion failure (%s) file..%s line..%d\n",
            expression, file, line);
    if (fmt) {
        vfprintf(fp, fmt, ap);
    }
}

void
debug_assert_func(char *fmt, ...)
{
    va_list     ap;

    va_start(ap, fmt);
    initialize_debug_write_fp();
    assert_func(st_current_controller->debug_write_fp,
                st_current_file_name, st_current_line,
                st_assert_expression, fmt, ap);
    assert_func(stderr,
                st_current_file_name, st_current_line,
                st_assert_expression, fmt, ap);
    va_end(ap);
    abort();
}

static void
panic_func(FILE *fp, char *file, int line, char *fmt,  va_list ap)
{
    fprintf(fp, "Panic!! file..%s line..%d\n", file, line);
    if (fmt) {
        vfprintf(fp, fmt, ap);
    }
}

void
debug_panic_func(char *fmt, ...)
{
    va_list     ap;

    va_start(ap, fmt);
    initialize_debug_write_fp();
    panic_func(st_current_controller->debug_write_fp,
               st_current_file_name, st_current_line, fmt, ap);
    panic_func(stderr, st_current_file_name, st_current_line, fmt, ap);
    va_end(ap);
    abort();
}

void
debug_debug_write_func(int level, char *fmt, ...)
{
    va_list     ap;

    if (level > 0 && level > st_current_controller->current_debug_level) {
        return;
    }
    va_start(ap, fmt);
    initialize_debug_write_fp();
    if (fmt) {
        vfprintf(st_current_controller->debug_write_fp, fmt, ap);
    }
    va_end(ap);
}

void
debug_set(debug_controller_tp controller, char *file, int line)
{
    st_current_controller = controller;
    st_current_file_name = file;
    st_current_line = line;
}

void
debug_set_expression(char *expression)
{
    st_assert_expression = expression;
}




