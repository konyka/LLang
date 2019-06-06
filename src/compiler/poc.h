/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: poc.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:20:59 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 09:08:10
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_POC_H__
#define __DARKBLUE_POC_H__


#include <stdio.h>
#include <setjmp.h>
#include <wchar.h>
#include "memory_public.h"
#include "poc_public.h"
#include "povm_code.h"
#include "common.h"

#define smaller(a, b) ((a) < (b) ? (a) : (b))
#define larger(a, b) ((a) > (b) ? (a) : (b))

#define MESSAGE_ARGUMENT_MAX    (256)
#define LINE_BUF_SIZE           (1024)

#define ARRAY_SIZE(array)  (sizeof(array) / sizeof((array)[0]))

#define UNDEFINED_LABEL (-1)

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
    PARSE_ERR = 1,
    CHARACTER_INVALID_ERR,
    FUNCTION_MULTIPLE_DEFINE_ERR,
    BAD_MULTIBYTE_CHARACTER_ERR,
    UNEXPECTED_WIDE_STRING_IN_COMPILE_ERR,
    PARAMETER_MULTIPLE_DEFINE_ERR,
    VARIABLE_MULTIPLE_DEFINE_ERR,
    IDENTIFIER_NOT_FOUND_ERR,
    DERIVE_TYPE_CAST_ERR,
    CAST_MISMATCH_ERR,
    MATH_TYPE_MISMATCH_ERR,
    COMPARE_TYPE_MISMATCH_ERR,
    LOGICAL_TYPE_MISMATCH_ERR,
    MINUS_TYPE_MISMATCH_ERR,
    LOGICAL_NOT_TYPE_MISMATCH_ERR,
    INC_DEC_TYPE_MISMATCH_ERR,
    FUNCTION_NOT_IDENTIFIER_ERR,
    FUNCTION_NOT_FOUND_ERR,
    ARGUMENT_COUNT_MISMATCH_ERR,
    NOT_LVALUE_ERR,
    LABEL_NOT_FOUND_ERR,
    COMPILE_ERROR_COUNT_PLUS_1
} compile_error_enum;

typedef struct expression_tag expression_t;

typedef enum {
    BOOLEAN_EXPRESSION = 1,
    INT_EXPRESSION,
    DOUBLE_EXPRESSION,
    STRING_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    COMMA_EXPRESSION,
    ASSIGN_EXPRESSION,
    ADD_EXPRESSION,
    SUB_EXPRESSION,
    MUL_EXPRESSION,
    DIV_EXPRESSION,
    MOD_EXPRESSION,
    EQ_EXPRESSION,
    NE_EXPRESSION,
    GT_EXPRESSION,
    GE_EXPRESSION,
    LT_EXPRESSION,
    LE_EXPRESSION,
    LOGICAL_AND_EXPRESSION,
    LOGICAL_OR_EXPRESSION,
    MINUS_EXPRESSION,
    LOGICAL_NOT_EXPRESSION,
    FUNCTION_CALL_EXPRESSION,
    INCREMENT_EXPRESSION,
    DECREMENT_EXPRESSION,
    CAST_EXPRESSION,
    EXPRESSION_KIND_COUNT_PLUS_1
} expression_kind_enum;

#define poc_is_numeric_type(type)\
  ((type) == POC_INT_VALUE || (type) == POC_DOUBLE_VALUE)

#define poc_is_math_operator(operator) \
  ((operator) == ADD_EXPRESSION || (operator) == SUB_EXPRESSION\
   || (operator) == MUL_EXPRESSION || (operator) == DIV_EXPRESSION\
   || (operator) == MOD_EXPRESSION)

#define poc_is_compare_operator(operator) \
  ((operator) == EQ_EXPRESSION || (operator) == NE_EXPRESSION\
   || (operator) == GT_EXPRESSION || (operator) == GE_EXPRESSION\
   || (operator) == LT_EXPRESSION || (operator) == LE_EXPRESSION)

#define poc_is_int(type) \
  ((type)->basic_type == POVM_INT_TYPE && (type)->derive == NULL)

#define poc_is_double(type) \
  ((type)->basic_type == POVM_DOUBLE_TYPE && (type)->derive == NULL)

#define poc_is_boolean(type) \
  ((type)->basic_type == POVM_BOOLEAN_TYPE && (type)->derive == NULL)

#define poc_is_string(type) \
  ((type)->basic_type == POVM_STRING_TYPE && (type)->derive == NULL)

#define poc_is_logical_operator(operator) \
  ((operator) == LOGICAL_AND_EXPRESSION || (operator) == LOGICAL_OR_EXPRESSION)

typedef struct argumentList_tag {
    expression_t *expression;
    struct argumentList_tag *next;
} argument_list_t;

typedef struct type_specifier_tag type_specifier_t;

typedef struct parameterList_tag {
    char                *name;
    type_specifier_t       *type;
    int                 line_number;
    struct parameterList_tag *next;
} parameter_list_t;

typedef enum {
    FUNCTION_DERIVE
} derive_tag_enum;

typedef struct {
    parameter_list_t       *parameter_list;
} function_derive_tag;

typedef struct type_derive_tag {
    derive_tag_enum   tag;
    union {
        function_derive_tag  function_d;
    } u;
    struct type_derive_tag       *next;
} type_derive_t;

struct type_specifier_tag {
    povm_basic_type_enum       basic_type;
    type_derive_t  *derive;
};

typedef struct function_definition_tag function_definition_t;

typedef struct {
    char        *name;
    type_specifier_t       *type;
    expression_t  *initializer;
    int variable_index;
    povm_boolean_enum is_local;
} declaration_tag;

typedef struct declaration_list_tag {
    declaration_tag *declaration;
    struct declaration_list_tag *next;
} declaration_list_t;

typedef struct {
    char        *name;
    povm_boolean_enum is_function;
    union {
        function_definition_t *function;
        declaration_tag     *declaration;
    } u;
} identifier_expression_tag;

typedef struct {
    expression_t  *left;
    expression_t  *right;
} comma_expression_tag;

typedef enum {
    NORMAL_ASSIGN = 1,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN
} assignment_operator_enum;

typedef struct {
    assignment_operator_enum  operator;
    expression_t  *left;
    expression_t  *operand;
} assign_expression_tag;

typedef struct {
    expression_t  *left;
    expression_t  *right;
} binary_expression_tag;

typedef struct {
    expression_t          *function;
    argument_list_t        *argument;
} function_call_expression_tag;

typedef struct {
    expression_t  *operand;
} increment_or_decrement_tag;

typedef enum {
    INT_TO_DOUBLE_CAST,
    DOUBLE_TO_INT_CAST,
    BOOLEAN_TO_STRING_CAST,
    INT_TO_STRING_CAST,
    DOUBLE_TO_STRING_CAST
} cast_type_enum;

typedef struct {
    cast_type_enum    type;
    expression_t  *operand;
} cast_expression_tag;

struct expression_tag {
    type_specifier_t *type;
    expression_kind_enum kind;
    int line_number;
    union {
        povm_boolean_enum             boolean_value;
        int                     int_value;
        double                  double_value;
        povm_char                *string_value;
        identifier_expression_tag    identifier;
        comma_expression_tag         comma;
        assign_expression_tag        assign_expression;
        binary_expression_tag        binary_expression;
        expression_t              *minus_expression;
        expression_t              *logical_not;
        function_call_expression_tag  function_call_expression;
        increment_or_decrement_tag    inc_dec;
        cast_expression_tag          cast;
    } u;
};

typedef struct statement_tag statement_t;

typedef struct statement_list_tag {
    statement_t   *statement;
    struct statement_list_tag    *next;
} statement_list_t;

typedef enum {
    UNDEFINED_BLOCK = 1,
    FUNCTION_BLOCK,
    WHILE_STATEMENT_BLOCK,
    FOR_STATEMENT_BLOCK
} block_type_enum;

typedef struct {
    statement_t   *statement;
    int         continue_label;
    int         break_label;
} statement_block_info_tag;

typedef struct {
    function_definition_t  *function;
    int         end_label;
} function_block_info_tag;

typedef struct block_tag {
    block_type_enum           type;
    struct block_tag    *outer_block;
    statement_list_t       *statement_list;
    declaration_list_t     *declaration_list;
    union {
        statement_block_info_tag      statement;
        function_block_info_tag       function;
    } parent;
} block_t;

typedef struct elsif_tag {
    expression_t  *condition;
    block_t       *block;
    struct elsif_tag    *next;
} elsif_t;

typedef struct {
    expression_t  *condition;
    block_t       *then_block;
    elsif_t       *elsif_list;
    block_t       *else_block;
} if_statement_tag;

typedef struct {
    char        *label;
    expression_t  *condition;
    block_t       *block;
} while_statement_tag;

typedef struct {
    char        *label;
    expression_t  *init;
    expression_t  *condition;
    expression_t  *post;
    block_t       *block;
} for_statement_tag;

typedef struct {
    char        *label;
    char        *variable;
    expression_t  *collection;
    block_t       *block;
} foreach_statement_tag;

typedef struct {
    expression_t *return_value;
} return_statement_tag;

typedef struct {
    char        *label;
} break_statement_tag;

typedef struct {
    char        *label;
} continue_statement_tag;

typedef struct {
    block_t       *try_block;
    block_t       *catch_block;
    char        *exception;
    block_t       *finally_block;
} try_statement_tag;

typedef struct {
    expression_t  *exception;
} throw_statement_tag;

typedef enum {
    EXPRESSION_STATEMENT = 1,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    FOREACH_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    TRY_STATEMENT,
    THROW_STATEMENT,
    DECLARATION_STATEMENT,
    STATEMENT_TYPE_COUNT_PLUS_1
} statement_type_enum;

struct statement_tag {
    statement_type_enum       type;
    int                 line_number;
    union {
        expression_t      *expression_s;
        if_statement_tag     if_s;
        while_statement_tag  while_s;
        for_statement_tag    for_s;
        foreach_statement_tag        foreach_s;
        break_statement_tag  break_s;
        continue_statement_tag       continue_s;
        return_statement_tag return_s;
        try_statement_tag    try_s;
        throw_statement_tag  throw_s;
        declaration_tag     *declaration_s;
    } u;
};

struct function_definition_tag {
    type_specifier_t       *type;
    char                *name;
    parameter_list_t       *parameter;
    block_t               *block;
    int                 local_variable_count;
    declaration_tag         **local_variable;
    int                 index;
    struct function_definition_tag       *next;
};

typedef enum {
    EUC_ENCODING = 1,
    SHIFT_JIS_ENCODING,
    UTF_8_ENCODING
} encoding_enum;

struct poc_compiler_tag {
    mem_storage_tp         compile_storage;
    function_definition_t  *function_list;
    int                 function_count;
    declaration_list_t     *declaration_list;
    statement_list_t       *statement_list;
    int                 current_line_number;
    block_t               *current_block;
    poc_input_mode_enum       input_mode;
    encoding_enum            source_encoding;
};

typedef struct {
    povm_char    *string;
} vstring_tag;

/* po.l */
void poc_set_source_string(char **source);

/* create.c */
declaration_list_t *poc_chain_declaration(declaration_list_t *list,
                                       declaration_tag *decl);
declaration_tag *poc_alloc_declaration(type_specifier_t *type, char *identifier);
void poc_function_define(povm_basic_type_enum type, char *identifier,
                         parameter_list_t *parameter_list, block_t *block);
parameter_list_t *poc_create_parameter(povm_basic_type_enum type, char *identifier);
parameter_list_t *poc_chain_parameter(parameter_list_t *list,
                                   povm_basic_type_enum type, char *identifier);
argument_list_t *poc_create_argument_list(expression_t *expression);
argument_list_t *poc_chain_argument_list(argument_list_t *list, expression_t *expr);
statement_list_t *poc_create_statement_list(statement_t *statement);
statement_list_t *poc_chain_statement_list(statement_list_t *list,
                                        statement_t *statement);
expression_t *poc_alloc_expression(expression_kind_enum type);
expression_t *poc_create_comma_expression(expression_t *left, expression_t *right);
expression_t *poc_create_assign_expression(expression_t *left,
                                         assignment_operator_enum operator,
                                         expression_t *operand);
expression_t *poc_create_binary_expression(expression_kind_enum operator,
                                         expression_t *left,
                                         expression_t *right);
expression_t *poc_create_minus_expression(expression_t *operand);
expression_t *poc_create_logical_not_expression(expression_t *operand);
expression_t *poc_create_index_expression(expression_t *array, expression_t *index);
expression_t *poc_create_incdec_expression(expression_t *operand,
                                         expression_kind_enum inc_or_dec);
expression_t *poc_create_identifier_expression(char *identifier);
expression_t *poc_create_function_call_expression(expression_t *function,
                                                argument_list_t *argument);
expression_t *poc_create_member_expression(expression_t *expression,
                                         char *member_name);
expression_t *poc_create_boolean_expression(povm_boolean_enum value);
expression_t *poc_create_null_expression(void);
statement_t *poc_create_if_statement(expression_t *condition,
                                   block_t *then_block, elsif_t *elsif_list,
                                   block_t *else_block);
elsif_t *poc_chain_elsif_list(elsif_t *list, elsif_t *add);
elsif_t *poc_create_elsif(expression_t *expr, block_t *block);
statement_t *poc_create_while_statement(char *label,
                                      expression_t *condition, block_t *block);
statement_t *
poc_create_foreach_statement(char *label, char *variable,
                             expression_t *collection, block_t *block);
statement_t *poc_create_for_statement(char *label,
                                    expression_t *init, expression_t *cond,
                                    expression_t *post, block_t *block);
block_t * poc_open_block(void);
block_t *poc_close_block(block_t *block, statement_list_t *statement_list);
statement_t *poc_create_expression_statement(expression_t *expression);
statement_t *poc_create_return_statement(expression_t *expression);
statement_t *poc_create_break_statement(char *label);
statement_t *poc_create_continue_statement(char *label);
statement_t *poc_create_try_statement(block_t *try_block, char *exception,
                                    block_t *catch_block,
                                    block_t *finally_block);
statement_t *poc_create_throw_statement(expression_t *expression);
statement_t *poc_create_declaration_statement(povm_basic_type_enum type,
                                            char *identifier,
                                            expression_t *initializer);

/* string.c */
char *poc_create_identifier(char *str);
void poc_open_string_literal(void);
void poc_add_string_literal(int letter);
void poc_reset_string_literal_buffer(void);
povm_char *poc_close_string_literal(void);

/* fix_tree.c */
void poc_fix_tree(poc_compiler_t *compiler);

/* generate.c */
povm_executable_t *poc_generate(poc_compiler_t *compiler);

/* util.c */
poc_compiler_t *poc_get_current_compiler(void);
void poc_set_current_compiler(poc_compiler_t *compiler);
void *poc_malloc(size_t size);
type_specifier_t *poc_alloc_type_specifier(povm_basic_type_enum type);
function_definition_t *poc_search_function(char *name);
declaration_tag *poc_search_declaration(char *identifier, block_t *block);
void poc_vstr_clear(vstring_tag *v);
void poc_vstr_append_string(vstring_tag *v, povm_char *str);
void poc_vstr_append_character(vstring_tag *v, povm_char ch);
char *poc_get_basic_type_name(povm_basic_type_enum type);
povm_char *poc_expression_to_string(expression_t *expr);

/* wchar.c */
size_t poc_wcslen(povm_char *str);
povm_char *poc_wcscpy(povm_char *dest, povm_char *src);
povm_char *poc_wcsncpy(povm_char *dest, povm_char *src, size_t n);
int poc_wcscmp(povm_char *s1, povm_char *s2);
povm_char *poc_wcscat(povm_char *s1, povm_char *s2);
int poc_mbstowcs_len(const char *src);
void poc_mbstowcs(const char *src, povm_char *dest);
povm_char *poc_mbstowcs_alloc(int line_number, const char *src);
int poc_wcstombs_len(const povm_char *src);
void poc_wcstombs(const povm_char *src, char *dest);
char *poc_wcstombs_alloc(const povm_char *src);
char poc_wctochar(povm_char src);
int poc_print_wcs(FILE *fp, povm_char *str);
int poc_print_wcs_ln(FILE *fp, povm_char *str);
povm_boolean_enum poc_iswdigit(povm_char ch);

/* error.c */
void poc_compile_error(int line_number, compile_error_enum id, ...);

/* disassemble.c */
void poc_disassemble(povm_executable_t *exe);




#endif /* __DARKBLUE_POC_H__ */
