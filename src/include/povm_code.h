/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: povm_code.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:54:47 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 19:53:50
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_POVM_CODE_H__
#define __DARKBLUE_POVM_CODE_H__

#include <stdio.h>
#include <wchar.h>
#include "povm.h"

typedef enum {
    POVM_BOOLEAN_TYPE,
    POVM_INT_TYPE,
    POVM_DOUBLE_TYPE,
    POVM_STRING_TYPE
} povm_basic_type_enum;

typedef struct povm_type_specifier_tag povm_type_specifier_t;

typedef struct {
    char                *name;
    povm_type_specifier_t   *type;
} povm_local_variable_tag;

typedef enum {
    POVM_FUNCTION_DERIVE
} povm_derive_tag_enum;

typedef struct {
    int                 parameter_count;
    povm_local_variable_tag   *parameter;
} povm_function_derive_tag;

typedef struct povm_type_derive_tag {
    povm_derive_tag_enum       tag;
    union {
        povm_function_derive_tag      function_d;
    } u;
} povm_type_derive_t;

struct povm_type_specifier_tag {
    povm_basic_type_enum       basic_type;
    int                 derive_count;
    povm_type_derive_t      *derive;
};


typedef wchar_t povm_char;
typedef unsigned char povm_byte;

typedef enum {
    POVM_PUSH_INT_1BYTE = 1,
    POVM_PUSH_INT_2BYTE,
    POVM_PUSH_INT,
    POVM_PUSH_DOUBLE_0,
    POVM_PUSH_DOUBLE_1,
    POVM_PUSH_DOUBLE,
    POVM_PUSH_STRING,
    /**********/
    POVM_PUSH_STACK_INT,
    POVM_PUSH_STACK_DOUBLE,
    POVM_PUSH_STACK_STRING,
    POVM_POP_STACK_INT,
    POVM_POP_STACK_DOUBLE,
    POVM_POP_STACK_STRING,
    /**********/
    POVM_PUSH_STATIC_INT,
    POVM_PUSH_STATIC_DOUBLE,
    POVM_PUSH_STATIC_STRING,
    POVM_POP_STATIC_INT,
    POVM_POP_STATIC_DOUBLE,
    POVM_POP_STATIC_STRING,
    /**********/
    POVM_ADD_INT,
    POVM_ADD_DOUBLE,
    POVM_ADD_STRING,
    POVM_SUB_INT,
    POVM_SUB_DOUBLE,
    POVM_MUL_INT,
    POVM_MUL_DOUBLE,
    POVM_DIV_INT,
    POVM_DIV_DOUBLE,
    POVM_MOD_INT,
    POVM_MOD_DOUBLE,
    POVM_MINUS_INT,
    POVM_MINUS_DOUBLE,
    POVM_INCREMENT,
    POVM_DECREMENT,
    POVM_CAST_INT_TO_DOUBLE,
    POVM_CAST_DOUBLE_TO_INT,
    POVM_CAST_BOOLEAN_TO_STRING,
    POVM_CAST_INT_TO_STRING,
    POVM_CAST_DOUBLE_TO_STRING,
    POVM_EQ_INT,
    POVM_EQ_DOUBLE,
    POVM_EQ_STRING,
    POVM_GT_INT,
    POVM_GT_DOUBLE,
    POVM_GT_STRING,
    POVM_GE_INT,
    POVM_GE_DOUBLE,
    POVM_GE_STRING,
    POVM_LT_INT,
    POVM_LT_DOUBLE,
    POVM_LT_STRING,
    POVM_LE_INT,
    POVM_LE_DOUBLE,
    POVM_LE_STRING,
    POVM_NE_INT,
    POVM_NE_DOUBLE,
    POVM_NE_STRING,
    POVM_LOGICAL_AND,
    POVM_LOGICAL_OR,
    POVM_LOGICAL_NOT,
    POVM_POP,
    POVM_DUPLICATE,
    POVM_JUMP,
    POVM_JUMP_IF_TRUE,
    POVM_JUMP_IF_FALSE,
    /**********/
    POVM_PUSH_FUNCTION,
    POVM_INVOKE,
    POVM_RETURN
} povm_opcode_enum;

typedef enum {
    POVM_CONSTANT_INT,
    POVM_CONSTANT_DOUBLE,
    POVM_CONSTANT_STRING
} povm_constant_pool_enum;

typedef struct {
    povm_constant_pool_enum tag;
    union {
        int     c_int;
        double  c_double;
        povm_char *c_string;
    } u;
} povm_constant_pool_tag;

typedef struct {
    char                *name;
    povm_type_specifier_t   *type;
} povm_variable_tag;

typedef struct {
    int line_number;
    int start_pc;
    int pc_count;
} povm_line_number_tag;

typedef struct {
    povm_type_specifier_t   *type;
    char                *name;
    int                 parameter_count;
    povm_local_variable_tag   *parameter;
    povm_boolean_enum         is_implemented;
    int                 local_variable_count;
    povm_local_variable_tag   *local_variable;
    int                 code_size;
    povm_byte            *code;
    int                 line_number_size;
    povm_line_number_tag      *line_number;
    int                 need_stack_size;
} povm_function_tag;

struct povm_excecutable_tag {
    int                 constant_pool_count;
    povm_constant_pool_tag    *constant_pool;
    int                 global_variable_count;
    povm_variable_tag        *global_variable;
    int                 function_count;
    povm_function_tag        *function;
    int                 code_size;
    povm_byte            *code;
    int                 line_number_size;
    povm_line_number_tag      *line_number;
    int                 need_stack_size;
};




#endif /* __DARKBLUE_POVM_CODE_H__ */
