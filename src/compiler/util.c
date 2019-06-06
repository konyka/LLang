/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: util.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:27:07 by konyka
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
#include "poc.h"

static poc_compiler_t *st_current_compiler;

poc_compiler_t *
poc_get_current_compiler(void)
{
    return st_current_compiler;
}

void
poc_set_current_compiler(poc_compiler_t *compiler)
{
    st_current_compiler = compiler;
}

void *
poc_malloc(size_t size)
{
    void *p;
    poc_compiler_t *compiler;

    compiler = poc_get_current_compiler();
    p = mem_storage_malloc(compiler->compile_storage, size);

    return p;
}

type_specifier_t *
poc_alloc_type_specifier(povm_basic_type_enum type)
{
    type_specifier_t *ts = poc_malloc(sizeof(type_specifier_t));

    ts->basic_type = type;
    ts->derive = NULL;

    return ts;
}

function_definition_t *
poc_search_function(char *name)
{
    poc_compiler_t *compiler;
    function_definition_t *pos;

    compiler = poc_get_current_compiler();

    for (pos = compiler->function_list; pos; pos = pos->next) {
        if (!strcmp(pos->name, name))
            break;
    }
    return pos;
}

declaration_tag *
poc_search_declaration(char *identifier, block_t *block)
{
    block_t *b_pos;
    declaration_list_t *d_pos;
    poc_compiler_t *compiler;

    for (b_pos = block; b_pos; b_pos = b_pos->outer_block) {
        for (d_pos = b_pos->declaration_list; d_pos; d_pos = d_pos->next) {
            if (!strcmp(identifier, d_pos->declaration->name)) {
                return d_pos->declaration;
            }
        }
    }

    compiler = poc_get_current_compiler();

    for (d_pos = compiler->declaration_list; d_pos; d_pos = d_pos->next) {
        if (!strcmp(identifier, d_pos->declaration->name)) {
            return d_pos->declaration;
        }
    }

    return NULL;
}

void
poc_vstr_clear(vstring_tag *v)
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
poc_vstr_append_string(vstring_tag *v, povm_char *str)
{
    int new_size;
    int old_len;

    old_len = my_strlen(v->string);
    new_size = sizeof(povm_char) * (old_len + dvm_wcslen(str)  + 1);
    v->string = mem_realloc(v->string, new_size);
    dvm_wcscpy(&v->string[old_len], str);
}

void
poc_vstr_append_character(vstring_tag *v, povm_char ch)
{
    int current_len;
    
    current_len = my_strlen(v->string);
    v->string = mem_realloc(v->string,sizeof(povm_char) * (current_len + 2));
    v->string[current_len] = ch;
    v->string[current_len+1] = L'\0';
}

char *
poc_get_basic_type_name(povm_basic_type_enum type)
{
    switch (type) {
    case POVM_BOOLEAN_TYPE:
        return "boolean";
        break;
    case POVM_INT_TYPE:
        return "int";
        break;
    case POVM_DOUBLE_TYPE:
        return "double";
        break;
    case POVM_STRING_TYPE:
        return "string";
        break;
    default:
        debug_assert(0, ("bad case. type..%d\n", type));
    }
    return NULL;
}

povm_char *
poc_expression_to_string(expression_t *expr)
{
    char        buf[LINE_BUF_SIZE];
    povm_char    wc_buf[LINE_BUF_SIZE];
    int         len;
    povm_char    *new_str;

    if (expr->kind == BOOLEAN_EXPRESSION) {
        if (expr->u.boolean_value) {
            dvm_mbstowcs("true", wc_buf);
        } else {
            dvm_mbstowcs("false", wc_buf);
        }
    } else if (expr->kind == INT_EXPRESSION) {
        sprintf(buf, "%d", expr->u.int_value);
        dvm_mbstowcs(buf, wc_buf);
    } else if (expr->kind == DOUBLE_EXPRESSION) {
        sprintf(buf, "%f", expr->u.double_value);
        dvm_mbstowcs(buf, wc_buf);
    } else if (expr->kind == STRING_EXPRESSION) {
        return expr->u.string_value;
    } else {
        return NULL;
    }
    len = dvm_wcslen(wc_buf);
    new_str = mem_malloc(sizeof(povm_char) * (len + 1));
    dvm_wcscpy(new_str, wc_buf);

    return new_str;
}
