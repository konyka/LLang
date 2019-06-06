/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: dispose.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 15:10:30 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:20:44
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include "memory_public.h"
#include "debug_public.h"
#include "povm_code.h"

static void dispose_local_variable(int local_variable_count,
                                   povm_local_variable_tag *local_variable_list);

static void
dispose_type_specifier(povm_type_specifier_t *type)
{
    int i;

    for (i = 0; i < type->derive_count; i++) {
        switch (type->derive[i].tag) {
        case POVM_FUNCTION_DERIVE:
            dispose_local_variable(type->derive[i].u
                                   .function_d.parameter_count,
                                   type->derive[i].u
                                   .function_d.parameter);
            break;
        default:
            debug_assert(0, ("derive->tag..%d\n", type->derive[i].tag));
        }
    }
    mem_free(type->derive);
    mem_free(type);
}

static void
dispose_local_variable(int local_variable_count,
                       povm_local_variable_tag *local_variable)
{
    int i;

    for (i = 0; i < local_variable_count; i++) {
        mem_free(local_variable[i].name);
        dispose_type_specifier(local_variable[i].type);
    }
    mem_free(local_variable);
}

void
dvm_dispose_executable(povm_executable_t *exe)
{
    int i;

    for (i = 0; i < exe->constant_pool_count; i++) {
        if (exe->constant_pool[i].tag == POVM_CONSTANT_STRING) {
            mem_free(exe->constant_pool[i].u.c_string);
        }
    }
    mem_free(exe->constant_pool);
    
    for (i = 0; i < exe->global_variable_count; i++) {
        mem_free(exe->global_variable[i].name);
        dispose_type_specifier(exe->global_variable[i].type);
    }
    mem_free(exe->global_variable);

    for (i = 0; i < exe->function_count; i++) {
        dispose_type_specifier(exe->function[i].type);
        mem_free(exe->function[i].name);
        dispose_local_variable(exe->function[i].parameter_count,
                               exe->function[i].parameter);
        if (exe->function[i].is_implemented) {
            dispose_local_variable(exe->function[i].local_variable_count,
                                   exe->function[i].local_variable);
            mem_free(exe->function[i].code);
            mem_free(exe->function[i].line_number);
        }
    }
    mem_free(exe->function);
    mem_free(exe->code);
    mem_free(exe->line_number);
    mem_free(exe);
}
