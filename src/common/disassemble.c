/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: disassemble.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 15:10:07 by konyka
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
 




#include <stdio.h>
#include <string.h>
#include "debug_public.h"
#include "common.h"

extern OpcodeInfo dvm_opcode_info[];

static void
dump_constant_pool(int constant_pool_count, povm_constant_pool_tag *constant_pool)
{
    int i;

    printf("** constant pool ********************\n");
    for (i = 0; i < constant_pool_count; i++) {
        printf("[%d]", i);
        switch (constant_pool[i].tag) {
        case POVM_CONSTANT_INT:
            printf("int %d\n", constant_pool[i].u.c_int);
            break;
        case POVM_CONSTANT_DOUBLE:
            printf("double %f\n", constant_pool[i].u.c_double);
            break;
        case POVM_CONSTANT_STRING:
            printf("string ");
            dvm_print_wcs_ln(stdout, constant_pool[i].u.c_string);
            break;
        default:
            debug_assert(0, ("tag..%d\n", constant_pool[i].tag));
        }
    }
}

static void dump_type(povm_type_specifier_t *type);

static void
dump_parameter_list(int parameter_count, povm_local_variable_tag *parameter_list)
{
    int i;

    printf("(");
    for (i = 0; i < parameter_count; i++) {
        dump_type(parameter_list[i].type);
        printf(" %s", parameter_list[i].name);
        if (i < parameter_count-1) {
            printf(", ");
        }
    }
    printf(")");
}

static void
dump_type(povm_type_specifier_t *type)
{
    int i;

    switch (type->basic_type) {
    case POVM_BOOLEAN_TYPE:
        printf("boolean ");
        break;
    case POVM_INT_TYPE:
        printf("int ");
        break;
    case POVM_DOUBLE_TYPE:
        printf("double ");
        break;
    case POVM_STRING_TYPE:
        printf("string ");
        break;
    default:
        debug_assert(0, ("basic_type..%d\n", type->basic_type));
    }

    for (i = 0; i < type->derive_count; i++) {
        switch (type->derive[i].tag) {
        case POVM_FUNCTION_DERIVE:
            dump_parameter_list(type->derive[i].u.function_d.parameter_count,
                                type->derive[i].u.function_d.parameter);
            break;
        default:
            debug_assert(0, ("derive_tag..%d\n", type->derive->tag));
        }
    }
}

static void
dump_variable(int global_variable_count, povm_variable_tag *global_variable)
{
    int i;

    printf("** global variable ********************\n");
    for (i = 0; i < global_variable_count; i++) {
        dump_type(global_variable[i].type);
        printf(" %s\n", global_variable[i].name);
    }
}

static void
dump_opcode(int code_size, povm_byte *code)
{
    int i;

    for (i = 0; i < code_size; i++) {
        OpcodeInfo *info;
        int value;
        int j;

        info = &dvm_opcode_info[code[i]];
        printf("%4d %s ", i, info->mnemonic);
        for (j = 0; info->parameter[j] != '\0'; j++) {
            switch (info->parameter[j]) {
            case 'b':
                value = code[i+1];
                printf(" %d", value);
                i++;
                break;
            case 's': /* FALLTHRU */
            case 'p':
                value = (code[i+1] << 8) + code[i+2];
                printf(" %d", value);
                i += 2;
                break;
            default:
                debug_assert(0, ("param..%s, j..%d", info->parameter, j));
            }
        }
        printf("\n");
    }
}

static void
dump_function(int function_count, povm_function_tag *function)
{
    int i;

    printf("** function ********************\n");
    for (i = 0; i < function_count; i++) {
        dump_type(function[i].type);
        printf(" %s ", function[i].name);
        dump_parameter_list(function[i].parameter_count,
                            function[i].parameter);
        printf("\n");
        dump_opcode(function[i].code_size, function[i].code);
    }
}

void
dvm_disassemble(povm_executable_t *exe)
{
    dump_constant_pool(exe->constant_pool_count, exe->constant_pool);
    dump_variable(exe->global_variable_count, exe->global_variable);
    dump_function(exe->function_count, exe->function);
    printf("** toplevel ********************\n");
    dump_opcode(exe->code_size, exe->code);
}
