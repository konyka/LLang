/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: util.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:53:35 by konyka
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
#include "debug_public.h"
#include "povm_pri.h"

void
dvm_vstr_clear(vstring_tag *v)
{
    v->string = NULL;
}

static int
my_strlen(povm_char *str)
{
    if (str == NULL) {
        return 0;
    }
    return dvm_wcslen(str);
}

void
dvm_vstr_append_string(vstring_tag *v, povm_char *str)
{
    int new_size;
    int old_len;

    old_len = my_strlen(v->string);
    new_size = sizeof(povm_char) * (old_len + dvm_wcslen(str)  + 1);
    v->string = mem_realloc(v->string, new_size);
    dvm_wcscpy(&v->string[old_len], str);
}

void
dvm_vstr_append_character(vstring_tag *v, povm_char ch)
{
    int current_len;
    
    current_len = my_strlen(v->string);
    v->string = mem_realloc(v->string,sizeof(povm_char) * (current_len + 2));
    v->string[current_len] = ch;
    v->string[current_len+1] = L'\0';
}

