/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: wchar.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:27:29 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:14:06
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
#include <wchar.h>
#include "debug_public.h"
#include "poc.h"

wchar_t *
poc_mbstowcs_alloc(int line_number, const char *src)
{
    int len;
    wchar_t *ret;

    len = dvm_mbstowcs_len(src);
    if (len < 0) {
        return NULL;
        poc_compile_error(line_number,
                          BAD_MULTIBYTE_CHARACTER_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    ret = mem_malloc(sizeof(wchar_t) * (len+1));
    dvm_mbstowcs(src, ret);

    return ret;
}
