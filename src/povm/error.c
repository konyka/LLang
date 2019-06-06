/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: error.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:51:32 by konyka
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
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "memory_public.h"
#include "debug_public.h"
#include "povm_pri.h"

extern error_tag dvm_error_message_format[];

typedef struct {
    message_argument_enum type;
    char        *name;
    union {
        int     int_val;
        double  double_val;
        char    *string_val;
        void    *pointer_val;
        int     character_val;
    } u;
} MessageArgument;

static void
create_message_argument(MessageArgument *arg, va_list ap)
{
    int index = 0;
    message_argument_enum type;
    
    while ((type = va_arg(ap, message_argument_enum))
           != MESSAGE_ARGUMENT_END) {
        arg[index].type = type;
        arg[index].name = va_arg(ap, char*);
        switch (type) {
        case INT_MESSAGE_ARGUMENT:
            arg[index].u.int_val = va_arg(ap, int);
            break;
        case DOUBLE_MESSAGE_ARGUMENT:
            arg[index].u.double_val = va_arg(ap, double);
            break;
        case STRING_MESSAGE_ARGUMENT:
            arg[index].u.string_val = va_arg(ap, char*);
            break;
        case POINTER_MESSAGE_ARGUMENT:
            arg[index].u.pointer_val = va_arg(ap, void*);
            break;
        case CHARACTER_MESSAGE_ARGUMENT:
            arg[index].u.character_val = va_arg(ap, int);
            break;
        case MESSAGE_ARGUMENT_END:
            assert(0);
            break;
        default:
            assert(0);
        }
        index++;
        assert(index < MESSAGE_ARGUMENT_MAX);
    }
}

static void
search_argument(MessageArgument *arg_list,
                char *arg_name, MessageArgument *arg)
{
    int i;

    for (i = 0; arg_list[i].type != MESSAGE_ARGUMENT_END; i++) {
        if (!strcmp(arg_list[i].name, arg_name)) {
            *arg = arg_list[i];
            return;
        }
    }
    assert(0);
}

static void
format_message(povm_executable_t *exe, function_tag *func, int pc,
               error_tag *format, vstring_tag *v,
               va_list ap)
{
    int         i;
    char        buf[LINE_BUF_SIZE];
    povm_char    wc_buf[LINE_BUF_SIZE];
    int         arg_name_index;
    char        arg_name[LINE_BUF_SIZE];
    MessageArgument     arg[MESSAGE_ARGUMENT_MAX];
    MessageArgument     cur_arg;
    povm_char    *wc_format;

    create_message_argument(arg, ap);

    wc_format = dvm_mbstowcs_alloc(exe, func, pc, format->format);
    debug_assert(wc_format != NULL, ("wc_format is null.\n"));
    
    for (i = 0; wc_format[i] != L'\0'; i++) {
        if (wc_format[i] != L'$') {
            dvm_vstr_append_character(v, wc_format[i]);
            continue;
        }
        assert(wc_format[i+1] == L'(');
        i += 2;
        for (arg_name_index = 0; wc_format[i] != L')';
             arg_name_index++, i++) {
            arg_name[arg_name_index] = dvm_wctochar(wc_format[i]);
        }
        arg_name[arg_name_index] = '\0';
        assert(wc_format[i] == L')');

        search_argument(arg, arg_name, &cur_arg);
        switch (cur_arg.type) {
        case INT_MESSAGE_ARGUMENT:
            sprintf(buf, "%d", cur_arg.u.int_val);
            dvm_mbstowcs(buf, wc_buf);
            dvm_vstr_append_string(v, wc_buf);
            break;
        case DOUBLE_MESSAGE_ARGUMENT:
            sprintf(buf, "%f", cur_arg.u.double_val);
            dvm_mbstowcs(buf, wc_buf);
            dvm_vstr_append_string(v, wc_buf);
            break;
        case STRING_MESSAGE_ARGUMENT:
            dvm_mbstowcs(cur_arg.u.string_val, wc_buf);
            dvm_vstr_append_string(v, wc_buf);
            break;
        case POINTER_MESSAGE_ARGUMENT:
            sprintf(buf, "%p", cur_arg.u.pointer_val);
            dvm_mbstowcs(buf, wc_buf);
            dvm_vstr_append_string(v, wc_buf);
            break;
        case CHARACTER_MESSAGE_ARGUMENT:
            sprintf(buf, "%c", cur_arg.u.character_val);
            dvm_mbstowcs(buf, wc_buf);
            dvm_vstr_append_string(v, wc_buf);
            break;
        case MESSAGE_ARGUMENT_END:
            assert(0);
            break;
        default:
            assert(0);
        }
    }
    mem_free(wc_format);
}

static void
self_check()
{
    if (strcmp(dvm_error_message_format[0].format, "dummy") != 0) {
        debug_panic(("runtime error message format error.\n"));
    }
    if (strcmp(dvm_error_message_format
               [RUNTIME_ERROR_COUNT_PLUS_1].format,
               "dummy") != 0) {
        debug_panic(("runtime error message format error. "
                   "RUNTIME_ERROR_COUNT_PLUS_1..%d\n",
                   RUNTIME_ERROR_COUNT_PLUS_1));
    }
}

static int
conv_pc_to_line_number(povm_executable_t *exe, function_tag *func, int pc)
{
    povm_line_number_tag *line_number;
    int line_number_size;
    int i;
    int ret;

    if (func) {
        line_number = exe->function[func->u.diksam_f.index].line_number;
        line_number_size
            = exe->function[func->u.diksam_f.index].line_number_size;
    } else {
        line_number = exe->line_number;
        line_number_size = exe->line_number_size;
    }

    for (i = 0; i < exe->line_number_size; i++) {
        if (pc >= line_number[i].start_pc
            && pc < line_number[i].start_pc + line_number[i].pc_count) {
            ret = line_number[i].line_number;
        }
    }

    return ret;
}

void
dvm_error(povm_executable_t *exe, function_tag *func, int pc, runtime_error_enum id, ...)
{
    va_list     ap;
    vstring_tag     message;
    int         line_number;

    self_check();
    va_start(ap, id);

    dvm_vstr_clear(&message);
    format_message(exe, func, pc,
                   &dvm_error_message_format[id],
                   &message, ap);

    if (pc != NO_LINE_NUMBER_PC) {
        line_number = conv_pc_to_line_number(exe, func, pc);
        fprintf(stderr, "%3d:", line_number);
    }
    dvm_print_wcs_ln(stderr, message.string);
    va_end(ap);

    exit(1);
}
