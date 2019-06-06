/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: povm_pri.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:29:13 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 23:00:52
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_POVM_PRI_H__
#define __DARKBLUE_POVM_PRI_H__

#include "povm_code.h"
#include "povm_dev.h"
#include "common.h"

#define STACK_ALLOC_SIZE (4096)
#define HEAP_THRESHOLD_SIZE     (1024 * 256)
#define TRUE_STRING (L"true")
#define FALSE_STRING (L"false")

#define NO_LINE_NUMBER_PC (-1)

#define MESSAGE_ARGUMENT_MAX    (256)
#define LINE_BUF_SIZE (1024)

#define GET_2BYTE_INT(p) (((p)[0] << 8) + (p)[1])
#define SET_2BYTE_INT(p, value) \
  (((p)[0] = (value) >> 8), ((p)[1] = value & 0xff))

typedef enum {
    INT_MESSAGE_ARGUMENT = 1,
    DOUBLE_MESSAGE_ARGUMENT,
    STRING_MESSAGE_ARGUMENT,
    CHARACTER_MESSAGE_ARGUMENT,
    POINTER_MESSAGE_ARGUMENT,
    MESSAGE_ARGUMENT_END
} message_argument_enum;

typedef struct {
    char *format;
} error_tag;

typedef enum {
    BAD_MULTIBYTE_CHARACTER_ERR = 1,
    FUNCTION_NOT_FOUND_ERR,
    FUNCTION_MULTIPLE_DEFINE_ERR,
    RUNTIME_ERROR_COUNT_PLUS_1
} runtime_error_enum;

typedef struct {
    povm_char    *string;
} vstring_tag;

typedef enum {
    NATIVE_FUNCTION,
    PO_FUNCTION
} function_kind_enum;

typedef struct {
    povm_native_function_proc *proc;
    int arg_count;
} native_function_enum;

typedef struct {
    povm_executable_t      *executable;
    int                 index;
} po_function_tag;

typedef struct {
    char                *name;
    function_kind_enum        kind;
    union {
        native_function_enum native_f;
        po_function_tag diksam_f;
    } u;
} function_tag;

typedef struct {
    function_tag    *caller;
    int         caller_address;
    int         base;
} call_info_tag;

#define revalue_up_align(val)   ((val) ? (((val) - 1) / sizeof(povm_value_union) + 1)\
                                 : 0)
#define CALL_INFO_ALIGN_SIZE    (revalue_up_align(sizeof(call_info_tag)))

typedef union {
    call_info_tag    s;
    povm_value_union   u[CALL_INFO_ALIGN_SIZE];
} call_info_union;

typedef struct {
    int         alloc_size;
    int         stack_pointer;
    povm_value_union   *stack;
    povm_boolean_enum *pointer_flags;
} stack_tag;

typedef enum {
    STRING_OBJECT = 1,
    OBJECT_TYPE_COUNT_PLUS_1
} object_type_enum;

struct povm_string_tag {
    povm_boolean_enum is_literal;
    povm_char    *string;
};

struct povm_object_tag {
    object_type_enum  type;
    unsigned int        marked:1;
    union {
        povm_string      string;
    } u;
    struct povm_object_tag *prev;
    struct povm_object_tag *next;
};

typedef struct {
    int         current_heap_size;
    int         current_threshold;
    povm_object_t  *header;
} heap_tag;

typedef struct {
    int         variable_count;
    povm_value_union   *variable;
} static_tag;

struct povm_virtual_machine_tag {
    stack_tag       stack;
    heap_tag        heap;
    static_tag      static_v;
    int         pc;
    function_tag    *function;
    int         function_count;
    povm_executable_t      *executable;
};

/* heap.c */
povm_object_t *
dvm_literal_to_dvm_string_i(povm_virtual_machine_t *inter, povm_char *str);
povm_object_t *
dvm_create_dvm_string_i(povm_virtual_machine_t *dvm, povm_char *str);
void dvm_garbage_collect(povm_virtual_machine_t *dvm);
/* native.c */
void dvm_add_native_functions(povm_virtual_machine_t *dvm);
/* wchar.c */
wchar_t *dvm_mbstowcs_alloc(povm_executable_t *exe, function_tag *func, int pc,
                            const char *src);
/* util.c */
void dvm_vstr_clear(vstring_tag *v);
void dvm_vstr_append_string(vstring_tag *v, povm_char *str);
void dvm_vstr_append_character(vstring_tag *v, povm_char ch);
/* error.c */
void
dvm_error(povm_executable_t *exe, function_tag *func, int pc, runtime_error_enum id, ...);




#endif /* __DARKBLUE_POVM_PRI_H__ */
