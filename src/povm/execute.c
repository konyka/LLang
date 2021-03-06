/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: execute.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:51:56 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 12:00:09
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include <math.h>
#include <string.h>
#include "memory_public.h"
#include "debug_public.h"
#include "povm_pri.h"

extern OpcodeInfo dvm_opcode_info[];

povm_virtual_machine_t *
povm_create_virtual_machine(void)
{
    povm_virtual_machine_t *dvm;

    dvm = mem_malloc(sizeof(povm_virtual_machine_t));
    dvm->stack.alloc_size = STACK_ALLOC_SIZE;
    dvm->stack.stack = mem_malloc(sizeof(povm_value_union) * STACK_ALLOC_SIZE);
    dvm->stack.pointer_flags
        = mem_malloc(sizeof(povm_boolean_enum) * STACK_ALLOC_SIZE);
    dvm->stack.stack_pointer = 0;
    dvm->heap.current_heap_size = 0;
    dvm->heap.header = NULL;
    dvm->heap.current_threshold = HEAP_THRESHOLD_SIZE;
    dvm->function = NULL;
    dvm->function_count = 0;
    dvm->executable = NULL;

    dvm_add_native_functions(dvm);

    return dvm;
}

void
povm_add_native_function(povm_virtual_machine_t *dvm, char *func_name,
                        povm_native_function_proc *proc, int arg_count)
{
    dvm->function
        = mem_realloc(dvm->function,
                      sizeof(function_tag) * (dvm->function_count + 1));

    dvm->function[dvm->function_count].name = mem_strdup(func_name);
    dvm->function[dvm->function_count].kind = NATIVE_FUNCTION;
    dvm->function[dvm->function_count].u.native_f.proc = proc;
    dvm->function[dvm->function_count].u.native_f.arg_count = arg_count;
    dvm->function_count++;
}

static void
add_functions(povm_virtual_machine_t *dvm, povm_executable_t *executable)
{
    int src_idx;
    int dest_idx;
    int func_count = 0;

    for (src_idx = 0; src_idx < executable->function_count; src_idx++) {
        if (executable->function[src_idx].is_implemented) {
            func_count++;
            for (dest_idx = 0; dest_idx < dvm->function_count; dest_idx++) {
                if (!strcmp(dvm->function[dest_idx].name,
                            executable->function[src_idx].name)) {
                    dvm_error(NULL, NULL, NO_LINE_NUMBER_PC,
                              FUNCTION_MULTIPLE_DEFINE_ERR,
                              STRING_MESSAGE_ARGUMENT, "name",
                              dvm->function[dest_idx].name,
                              MESSAGE_ARGUMENT_END);
                }
            }
        }
    }
    dvm->function
        = mem_realloc(dvm->function,
                      sizeof(function_tag)
                      * (dvm->function_count + func_count));

    for (src_idx = 0, dest_idx = dvm->function_count;
         src_idx < executable->function_count; src_idx++) {
        if (!executable->function[src_idx].is_implemented)
            continue;
        dvm->function[dest_idx].name
            = mem_strdup(executable->function[src_idx].name);
        dvm->function[dest_idx].u.diksam_f.executable
            = executable;
        dvm->function[dest_idx].u.diksam_f.index = src_idx;
        dest_idx++;
    }
    dvm->function_count += func_count;
}

static int
search_function(povm_virtual_machine_t *dvm, char *name)
{
    int i;

    for (i = 0; i < dvm->function_count; i++) {
        if (!strcmp(dvm->function[i].name, name)) {
            return i;
        }
    }
    dvm_error(NULL, NULL, NO_LINE_NUMBER_PC,
              FUNCTION_NOT_FOUND_ERR,
              STRING_MESSAGE_ARGUMENT, "name", name,
              MESSAGE_ARGUMENT_END);
    return 0; /* make compiler happy */
}

static void
convert_code(povm_virtual_machine_t *dvm, povm_executable_t *exe,
             povm_byte *code, int code_size, povm_function_tag *func)
{
    int i;
    int j;
    OpcodeInfo *info;
    int src_idx;
    unsigned int dest_idx;

    for (i = 0; i < code_size; i++) {
        if (code[i] == POVM_PUSH_STACK_INT
            || code[i] == POVM_PUSH_STACK_DOUBLE
            || code[i] == POVM_PUSH_STACK_STRING
            || code[i] == POVM_POP_STACK_INT
            || code[i] == POVM_POP_STACK_DOUBLE
            || code[i] == POVM_POP_STACK_STRING) {

            debug_assert(func != NULL, ("func == NULL!\n"));

            src_idx = GET_2BYTE_INT(&code[i+1]);
            if (src_idx >= func->parameter_count) {
                dest_idx = src_idx + CALL_INFO_ALIGN_SIZE;
            } else {
                dest_idx = src_idx;
            }
            SET_2BYTE_INT(&code[i+1], dest_idx);

        } else if (code[i] == POVM_PUSH_FUNCTION) {
            int idx_in_exe;
            unsigned int func_idx;

            idx_in_exe = GET_2BYTE_INT(&code[i+1]);
            func_idx = search_function(dvm, exe->function[idx_in_exe].name);
            SET_2BYTE_INT(&code[i+1], func_idx);
        }
        info = &dvm_opcode_info[code[i]];
        for (j = 0; info->parameter[j] != '\0'; j++) {
            switch (info->parameter[j]) {
            case 'b':
                i++;
                break;
            case 's': /* FALLTHRU */
            case 'p':
                i += 2;
                break;
            default:
                debug_assert(0, ("param..%s, j..%d", info->parameter, j));
            }
        }
    }
}

static void
initialize_value(povm_virtual_machine_t *dvm,
                 povm_basic_type_enum basic_type, povm_value_union *value)
{
    switch (basic_type) {
    case POVM_BOOLEAN_TYPE: /* FALLTHRU */
    case POVM_INT_TYPE:
        value->int_value = 0;
        break;
    case POVM_DOUBLE_TYPE:
        value->double_value = 0.0;
        break;
    case POVM_STRING_TYPE:
        value->object = dvm_literal_to_dvm_string_i(dvm, L"");
        break;
    default:
        debug_assert(0, ("basic_type..%d", basic_type));
    }
}

static void
add_static_variables(povm_virtual_machine_t *dvm, povm_executable_t *exe)
{
    int i;

    dvm->static_v.variable
        = mem_malloc(sizeof(povm_value_union) * exe->global_variable_count);
    dvm->static_v.variable_count = exe->global_variable_count;

    for (i = 0; i < exe->global_variable_count; i++) {
        if (exe->global_variable[i].type->basic_type == POVM_STRING_TYPE) {
            dvm->static_v.variable[i].object = NULL;
        }
    }
    for (i = 0; i < exe->global_variable_count; i++) {
        initialize_value(dvm,
                         exe->global_variable[i].type->basic_type,
                         &dvm->static_v.variable[i]);
    }
}


void
povm_add_executable(povm_virtual_machine_t *dvm, povm_executable_t *executable)
{
    int i;

    dvm->executable = executable;

    add_functions(dvm, executable);

    convert_code(dvm, executable,
                 executable->code, executable->code_size,
                 NULL);


    for (i = 0; i < executable->function_count; i++) {
        convert_code(dvm, executable,
                     executable->function[i].code,
                     executable->function[i].code_size,
                     &executable->function[i]);
    }

    add_static_variables(dvm, executable);
}

static povm_object_t *
chain_string(povm_virtual_machine_t *dvm, povm_object_t *str1, povm_object_t *str2)
{
    int len;
    povm_char    *str;
    povm_object_t *ret;

    len = dvm_wcslen(str1->u.string.string)
        + dvm_wcslen(str2->u.string.string);
    str = mem_malloc(sizeof(povm_char) * (len + 1));

    dvm_wcscpy(str, str1->u.string.string);
    dvm_wcscat(str, str2->u.string.string);

    ret = dvm_create_dvm_string_i(dvm, str);

    return ret;
}

static void
invoke_native_function(povm_virtual_machine_t *dvm, function_tag *func,
                       int *sp_p)
{
    povm_value_union   *stack;
    int         sp;
    povm_value_union   ret;

    stack = dvm->stack.stack;
    sp = *sp_p;
    debug_assert(func->kind == NATIVE_FUNCTION, ("func->kind..%d", func->kind));

    ret = func->u.native_f.proc(dvm,
                                func->u.native_f.arg_count,
                                &stack[sp-func->u.native_f.arg_count-1]);

    stack[sp-func->u.native_f.arg_count-1] = ret;

    *sp_p = sp - (func->u.native_f.arg_count);
}

static void
initialize_local_variables(povm_virtual_machine_t *dvm,
                           povm_function_tag *func, int from_sp)
{
    int i;
    int sp_idx;

    for (i = 0, sp_idx = from_sp; i < func->local_variable_count;
         i++, sp_idx++) {
        dvm->stack.pointer_flags[i] = POVM_FALSE;
    }

    for (i = 0, sp_idx = from_sp; i < func->local_variable_count;
         i++, sp_idx++) {
        initialize_value(dvm,
                         func->local_variable[i].type->basic_type,
                         &dvm->stack.stack[sp_idx]);
        if (func->local_variable[i].type->basic_type == POVM_STRING_TYPE) {
            dvm->stack.pointer_flags[i] = POVM_TRUE;
        }
    }
}

static void
expand_stack(povm_virtual_machine_t *dvm, int need_stack_size)
{
    int revalue_up;
    int rest;

    rest = dvm->stack.alloc_size - dvm->stack.stack_pointer;
    if (rest <= need_stack_size) {
        revalue_up = ((rest / STACK_ALLOC_SIZE) + 1) * STACK_ALLOC_SIZE;

        dvm->stack.alloc_size += revalue_up;
        dvm->stack.stack
            = mem_realloc(dvm->stack.stack,
                          dvm->stack.alloc_size * sizeof(povm_value_union));
        dvm->stack.pointer_flags
            = mem_realloc(dvm->stack.pointer_flags,
                          dvm->stack.alloc_size * sizeof(povm_boolean_enum));
    }
}

static void
invoke_diksam_function(povm_virtual_machine_t *dvm,
                       function_tag **caller_p, function_tag *callee,
                       povm_byte **code_p, int *code_size_p, int *pc_p,
                       int *sp_p, int *base_p, povm_executable_t **exe_p)
{
    call_info_tag *callInfo;
    povm_function_tag *callee_p;
    int i;

    *exe_p = callee->u.diksam_f.executable;
    callee_p = &(*exe_p)->function[callee->u.diksam_f.index];

    expand_stack(dvm,
                 CALL_INFO_ALIGN_SIZE
                 + callee_p->local_variable_count
                 + (*exe_p)->function[callee->u.diksam_f.index]
                 .need_stack_size);

    callInfo = (call_info_tag*)&dvm->stack.stack[*sp_p-1];
    callInfo->caller = *caller_p;
    callInfo->caller_address = *pc_p;
    callInfo->base = *base_p;
    for (i = 0; i < CALL_INFO_ALIGN_SIZE; i++) {
        dvm->stack.pointer_flags[*sp_p-1+i] = POVM_FALSE;
    }

    *base_p = *sp_p - callee_p->parameter_count - 1;
    *caller_p = callee;

    initialize_local_variables(dvm, callee_p,
                               *sp_p + CALL_INFO_ALIGN_SIZE - 1);

    *sp_p += CALL_INFO_ALIGN_SIZE + callee_p->local_variable_count - 1;
    *pc_p = 0;

    *code_p = (*exe_p)->function[callee->u.diksam_f.index].code;
    *code_size_p = (*exe_p)->function[callee->u.diksam_f.index].code_size;
}

static void
return_function(povm_virtual_machine_t *dvm, function_tag **func_p,
                povm_byte **code_p, int *code_size_p, int *pc_p,
                int *sp_p, int *base_p, povm_executable_t **exe_p)
{
    povm_value_union return_value;
    call_info_tag *callInfo;
    povm_function_tag *caller_p;
    povm_function_tag *callee_p;

    return_value = dvm->stack.stack[(*sp_p)-1];

    callee_p = &(*exe_p)->function[(*func_p)->u.diksam_f.index];
    callInfo = (call_info_tag*)&dvm->stack.stack[*sp_p - 1
                                           - callee_p->local_variable_count
                                           - CALL_INFO_ALIGN_SIZE];

    if (callInfo->caller) {
        *exe_p = callInfo->caller->u.diksam_f.executable;
        caller_p = &(*exe_p)->function[callInfo->caller->u.diksam_f.index];
        *code_p = caller_p->code;
        *code_size_p = caller_p->code_size;
    } else {
        *exe_p = dvm->executable;
        *code_p = dvm->executable->code;
        *code_size_p = dvm->executable->code_size;
    }
    *func_p = callInfo->caller;

    *pc_p = callInfo->caller_address + 1;
    *base_p = callInfo->base;

    *sp_p -= callee_p->local_variable_count + CALL_INFO_ALIGN_SIZE
        + callee_p->parameter_count;

    dvm->stack.stack[*sp_p-1] = return_value;
}

#define STI(dvm, sp) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].int_value)
#define STD(dvm, sp) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].double_value)
#define STO(dvm, sp) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].object)

#define STI_I(dvm, sp) \
  ((dvm)->stack.stack[(sp)].int_value)
#define STD_I(dvm, sp) \
  ((dvm)->stack.stack[(sp)].double_value)
#define STO_I(dvm, sp) \
  ((dvm)->stack.stack[(sp)].object)

#define STI_WRITE(dvm, sp, r) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].int_value = r,\
   (dvm)->stack.pointer_flags[(dvm)->stack.stack_pointer+(sp)] = POVM_FALSE)
#define STD_WRITE(dvm, sp, r) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].double_value = r, \
   (dvm)->stack.pointer_flags[(dvm)->stack.stack_pointer+(sp)] = POVM_FALSE)
#define STO_WRITE(dvm, sp, r) \
  ((dvm)->stack.stack[(dvm)->stack.stack_pointer+(sp)].object = r, \
   (dvm)->stack.pointer_flags[(dvm)->stack.stack_pointer+(sp)] = POVM_TRUE)

#define STI_WRITE_I(dvm, sp, r) \
  ((dvm)->stack.stack[(sp)].int_value = r,\
   (dvm)->stack.pointer_flags[(sp)] = POVM_FALSE)
#define STD_WRITE_I(dvm, sp, r) \
  ((dvm)->stack.stack[(sp)].double_value = r, \
   (dvm)->stack.pointer_flags[(sp)] = POVM_FALSE)
#define STO_WRITE_I(dvm, sp, r) \
  ((dvm)->stack.stack[(sp)].object = r, \
   (dvm)->stack.pointer_flags[(sp)] = POVM_TRUE)

static povm_value_union
execute(povm_virtual_machine_t *dvm, function_tag *func,
        povm_byte *code, int code_size)
{
    int         pc;
    int         base;
    povm_value_union   ret;
    povm_value_union   *stack;
    povm_executable_t *exe;

    stack = dvm->stack.stack;
    exe = dvm->executable;

    /*todo fix bug*/
    ret.object = NULL; 

    for (pc = dvm->pc; pc < code_size; ) {
        /*
        fprintf(stderr, "%s  sp(%d)\t\n",
                dvm_opcode_info[code[pc]].mnemonic, dvm->stack.stack_pointer);
        */

        switch (code[pc]) {
        case POVM_PUSH_INT_1BYTE:
            STI_WRITE(dvm, 0, code[pc+1]);
            dvm->stack.stack_pointer++;
            pc += 2;
            break;
        case POVM_PUSH_INT_2BYTE:
            STI_WRITE(dvm, 0, GET_2BYTE_INT(&code[pc+1]));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_INT:
            STI_WRITE(dvm, 0,
                      exe->constant_pool[GET_2BYTE_INT(&code[pc+1])].u.c_int);
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_DOUBLE_0:
            STD_WRITE(dvm, 0, 0.0);
            dvm->stack.stack_pointer++;
            pc++;
            break;
        case POVM_PUSH_DOUBLE_1:
            STD_WRITE(dvm, 0, 1.0);
            dvm->stack.stack_pointer++;
            pc++;
            break;
        case POVM_PUSH_DOUBLE:
            STD_WRITE(dvm, 0, 
                      exe->constant_pool[GET_2BYTE_INT(&code[pc+1])]
                      .u.c_double);
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STRING:
            STO_WRITE(dvm, 0,
                      dvm_literal_to_dvm_string_i(dvm,
                                                  exe->constant_pool
                                                  [GET_2BYTE_INT(&code[pc+1])]
                                                  .u.c_string));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STACK_INT:
            STI_WRITE(dvm, 0,
                      STI_I(dvm, base + GET_2BYTE_INT(&code[pc+1])));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STACK_DOUBLE:
            STD_WRITE(dvm, 0,
                      STD_I(dvm, base + GET_2BYTE_INT(&code[pc+1])));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STACK_STRING:
            STO_WRITE(dvm, 0,
                      STO_I(dvm, base + GET_2BYTE_INT(&code[pc+1])));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_POP_STACK_INT:
            STI_WRITE_I(dvm, base + GET_2BYTE_INT(&code[pc+1]),
                        STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_POP_STACK_DOUBLE:
            STD_WRITE_I(dvm, base + GET_2BYTE_INT(&code[pc+1]),
                        STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_POP_STACK_STRING:
            STO_WRITE_I(dvm, base + GET_2BYTE_INT(&code[pc+1]),
                        STO(dvm, -1));
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_PUSH_STATIC_INT:
            STI_WRITE(dvm, 0,
                      dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])]
                      .int_value);
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STATIC_DOUBLE:
            STD_WRITE(dvm, 0,
                      dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])]
                      .double_value);
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_PUSH_STATIC_STRING:
            STO_WRITE(dvm, 0,
                      dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])]
                      .object);
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_POP_STATIC_INT:
            dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])].int_value
                = STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_POP_STATIC_DOUBLE:
            dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])].double_value
                = STD(dvm, -1);
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_POP_STATIC_STRING:
            dvm->static_v.variable[GET_2BYTE_INT(&code[pc+1])].object
                = STO(dvm, -1);
            dvm->stack.stack_pointer--;
            pc += 3;
            break;
        case POVM_ADD_INT:
            STI(dvm, -2) = STI(dvm, -2) + STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_ADD_DOUBLE:
            STD(dvm, -2) = STD(dvm, -2) + STD(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_ADD_STRING:
            STO(dvm, -2) = chain_string(dvm,
                                        STO(dvm, -2),
                                        STO(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_SUB_INT:
            STI(dvm, -2) = STI(dvm, -2) - STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_SUB_DOUBLE:
            STD(dvm, -2) = STD(dvm, -2) - STD(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_MUL_INT:
            STI(dvm, -2) = STI(dvm, -2) * STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_MUL_DOUBLE:
            STD(dvm, -2) = STD(dvm, -2) * STD(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_DIV_INT:
            STI(dvm, -2) = STI(dvm, -2) / STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_DIV_DOUBLE:
            STD(dvm, -2) = STD(dvm, -2) / STD(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_MOD_INT:
            STI(dvm, -2) = STI(dvm, -2) % STI(dvm, -1);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_MOD_DOUBLE:
            STD(dvm, -2) = fmod(STD(dvm, -2), STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_MINUS_INT:
            STI(dvm, -1) = -STI(dvm, -1);
            pc++;
            break;
        case POVM_MINUS_DOUBLE:
            STD(dvm, -1) = -STD(dvm, -1);
            pc++;
            break;
        case POVM_INCREMENT:
            STI(dvm, -1)++;
            pc++;
            break;
        case POVM_DECREMENT:
            STI(dvm, -1)--;
            pc++;
            break;
        case POVM_CAST_INT_TO_DOUBLE:
            STD(dvm, -1) = (double)STI(dvm, -1);
            pc++;
            break;
        case POVM_CAST_DOUBLE_TO_INT:
            STI(dvm, -1) = (int)STD(dvm, -1);
            pc++;
            break;
        case POVM_CAST_BOOLEAN_TO_STRING:
            if (STI(dvm, -1)) {
                STO_WRITE(dvm, -1,
                          dvm_literal_to_dvm_string_i(dvm, TRUE_STRING));
            } else {
                STO_WRITE(dvm, -1,
                          dvm_literal_to_dvm_string_i(dvm, FALSE_STRING));
            }
            pc++;
            break;
        case POVM_CAST_INT_TO_STRING:
        {
            char buf[LINE_BUF_SIZE];
            povm_char *wc_str;

            sprintf(buf, "%d", STI(dvm, -1));
            wc_str = dvm_mbstowcs_alloc(exe, func, pc, buf);
            STO_WRITE(dvm, -1,
                      dvm_create_dvm_string_i(dvm, wc_str));
            pc++;
            break;
        }
        case POVM_CAST_DOUBLE_TO_STRING:
        {
            char buf[LINE_BUF_SIZE];
            povm_char *wc_str;

            sprintf(buf, "%f", STD(dvm, -1));
            wc_str = dvm_mbstowcs_alloc(exe, func, pc, buf);
            STO_WRITE(dvm, -1,
                      dvm_create_dvm_string_i(dvm, wc_str));
            pc++;
            break;
        }
        case POVM_EQ_INT:
            STI(dvm, -2) = (STI(dvm, -2) == STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_EQ_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) == STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_EQ_STRING:
            STI_WRITE(dvm, -2,
                      !dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                  STO(dvm, -1)->u.string.string));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GT_INT:
            STI(dvm, -2) = (STI(dvm, -2) > STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GT_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) > STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GT_STRING:
            STI_WRITE(dvm, -2,
                      dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                 STO(dvm, -1)->u.string.string) > 0);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GE_INT:
            STI(dvm, -2) = (STI(dvm, -2) >= STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GE_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) >= STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_GE_STRING:
            STI_WRITE(dvm, -2,
                      dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                 STO(dvm, -1)->u.string.string) >= 0);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LT_INT:
            STI(dvm, -2) = (STI(dvm, -2) < STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LT_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) < STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LT_STRING:
            STI_WRITE(dvm, -2,
                      dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                 STO(dvm, -1)->u.string.string) < 0);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LE_INT:
            STI(dvm, -2) = (STI(dvm, -2) <= STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LE_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) <= STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LE_STRING:
            STI_WRITE(dvm, -2,
                      dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                 STO(dvm, -1)->u.string.string) <= 0);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_NE_INT:
            STI(dvm, -2) = (STI(dvm, -2) != STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_NE_DOUBLE:
            STI(dvm, -2) = (STD(dvm, -2) != STD(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_NE_STRING:
            STI_WRITE(dvm, -2,
                      dvm_wcscmp(STO(dvm, -2)->u.string.string,
                                 STO(dvm, -1)->u.string.string) != 0);
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LOGICAL_AND:
            STI(dvm, -2) = (STI(dvm, -2) && STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LOGICAL_OR:
            STI(dvm, -2) = (STI(dvm, -2) || STI(dvm, -1));
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_LOGICAL_NOT:
            STI(dvm, -1) = !STI(dvm, -1);
            pc++;
            break;
        case POVM_POP:
            dvm->stack.stack_pointer--;
            pc++;
            break;
        case POVM_DUPLICATE:
            stack[dvm->stack.stack_pointer]
                = stack[dvm->stack.stack_pointer-1];
            dvm->stack.stack_pointer++;
            pc++;
            break;
        case POVM_JUMP:
            pc = GET_2BYTE_INT(&code[pc+1]);
            break;
        case POVM_JUMP_IF_TRUE:
            if (STI(dvm, -1)) {
                pc = GET_2BYTE_INT(&code[pc+1]);
            } else {
                pc += 3;
            }
            dvm->stack.stack_pointer--;
            break;
        case POVM_JUMP_IF_FALSE:
            if (!STI(dvm, -1)) {
                pc = GET_2BYTE_INT(&code[pc+1]);
            } else {
                pc += 3;
            }
            dvm->stack.stack_pointer--;
            break;
        case POVM_PUSH_FUNCTION:
            STI_WRITE(dvm, 0, GET_2BYTE_INT(&code[pc+1]));
            dvm->stack.stack_pointer++;
            pc += 3;
            break;
        case POVM_INVOKE:
        {
            int func_idx = STI(dvm, -1);
            if (dvm->function[func_idx].kind == NATIVE_FUNCTION) {
                invoke_native_function(dvm, &dvm->function[func_idx],
                                       &dvm->stack.stack_pointer);
                pc++;
            } else {
                invoke_diksam_function(dvm, &func, &dvm->function[func_idx],
                                       &code, &code_size, &pc,
                                       &dvm->stack.stack_pointer, &base, &exe);
            }
            break;
        }
        case POVM_RETURN:
            return_function(dvm, &func, &code, &code_size, &pc,
                            &dvm->stack.stack_pointer, &base, &exe);
            break;
        default:
            debug_assert(0, ("code[pc]..%d\n", code[pc]));
        }
    }

    return ret;
}

povm_value_union
povm_execute(povm_virtual_machine_t *dvm)
{
    povm_value_union ret;

    /*todo fix bug*/
    ret.object = NULL; 

    dvm->pc = 0;
    expand_stack(dvm, dvm->executable->need_stack_size);
    execute(dvm, NULL, dvm->executable->code, dvm->executable->code_size);

    return ret;
}

void
povm_dispose_virtual_machine(povm_virtual_machine_t *dvm)
{
    int i;

    dvm->static_v.variable_count = 0;
    dvm_garbage_collect(dvm);

    mem_free(dvm->stack.stack);
    mem_free(dvm->stack.pointer_flags);

    mem_free(dvm->static_v.variable);

    for (i = 0; i < dvm->function_count; i++) {
        mem_free(dvm->function[i].name);
    }
    mem_free(dvm->function);

    dvm_dispose_executable(dvm->executable);
    mem_free(dvm);
}
