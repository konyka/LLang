/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: poc_public.h
 *      Version: v0.0.0
 *   Created on: 2019-06-06 08:23:25 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 09:10:26
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_POC_PUBLIC_H__
#define __DARKBLUE_POC_PUBLIC_H__

#include <stdio.h>
#include "povm_code.h"

typedef struct poc_compiler_tag poc_compiler_t;

typedef enum {
    POC_FILE_INPUT_MODE = 1,
    POC_STRING_INPUT_MODE
} poc_input_mode_enum;

poc_compiler_t *poc_create_compiler(void);
povm_executable_t *poc_compile(poc_compiler_t *compiler, FILE *fp);
povm_executable_t *poc_compile_string(poc_compiler_t *compiler, char **lines);
void poc_dispose_compiler(poc_compiler_t *compiler);

#endif /* __DARKBLUE_POC_PUBLIC_H__ */
