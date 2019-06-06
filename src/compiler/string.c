/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: string.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:26:44 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:22:03
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include <stdio.h>
#include <string.h>
#include "memory_public.h"
#include "poc.h"

#define STRING_ALLOC_SIZE       (256)

static char *st_string_literal_buffer = NULL;
static int st_string_literal_buffer_size = 0;
static int st_string_literal_buffer_alloc_size = 0;

void
poc_open_string_literal(void)
{
    st_string_literal_buffer_size = 0;
}

void
poc_add_string_literal(int letter)
{
    if (st_string_literal_buffer_size == st_string_literal_buffer_alloc_size) {
        st_string_literal_buffer_alloc_size += STRING_ALLOC_SIZE;
        st_string_literal_buffer
            = mem_realloc(st_string_literal_buffer,
                          st_string_literal_buffer_alloc_size);
    }
    st_string_literal_buffer[st_string_literal_buffer_size] = letter;
    st_string_literal_buffer_size++;
}

void
poc_reset_string_literal_buffer(void)
{
    mem_free(st_string_literal_buffer);
    st_string_literal_buffer = NULL;
    st_string_literal_buffer_size = 0;
    st_string_literal_buffer_alloc_size = 0;
}

povm_char *
poc_close_string_literal(void)
{
    povm_char *new_str;
    int new_str_len;

    poc_add_string_literal('\0');
    new_str_len = dvm_mbstowcs_len(st_string_literal_buffer);
    if (new_str_len < 0) {
        poc_compile_error(poc_get_current_compiler()->current_line_number,
                          BAD_MULTIBYTE_CHARACTER_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    new_str = mem_malloc(sizeof(povm_char) * (new_str_len+1));
    dvm_mbstowcs(st_string_literal_buffer, new_str);

    return new_str;
}

char *
poc_create_identifier(char *str)
{
    char *new_str;

    new_str = poc_malloc(strlen(str) + 1);

    strcpy(new_str, str);

    return new_str;
}
