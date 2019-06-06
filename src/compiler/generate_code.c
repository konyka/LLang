/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: generate_code.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:15:02 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:23:49
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
#include <stdarg.h>
#include "memory_public.h"
#include "debug_public.h"
#include "poc.h"

extern OpcodeInfo dvm_opcode_info[];

#define OPCODE_ALLOC_SIZE (256)
#define LABEL_TABLE_ALLOC_SIZE (256)

typedef struct {
    int label_address;
} LabelTable;

typedef struct {
    int         size;
    int         alloc_size;
    povm_byte    *code;
    int         label_table_size;
    int         label_table_alloc_size;
    LabelTable  *label_table;
    int         line_number_size;
    povm_line_number_tag      *line_number;
} OpcodeBuf;

static povm_executable_t *
alloc_executable(void)
{
    povm_executable_t      *exe;

    exe = mem_malloc(sizeof(povm_executable_t));
    exe->constant_pool_count = 0;
    exe->constant_pool = NULL;
    exe->global_variable_count = 0;
    exe->global_variable = NULL;
    exe->function_count = 0;
    exe->function = NULL;
    exe->code_size = 0;
    exe->code = NULL;

    return exe;
}

static int
add_constant_pool(povm_executable_t *exe, povm_constant_pool_tag *cp)
{
    int ret;

    exe->constant_pool
        = mem_realloc(exe->constant_pool,
                      sizeof(povm_constant_pool_tag)
                      * (exe->constant_pool_count + 1));
    exe->constant_pool[exe->constant_pool_count] = *cp;

    ret = exe->constant_pool_count;
    exe->constant_pool_count++;

    return ret;
}

static povm_type_specifier_t *copy_type_specifier(type_specifier_t *src);

static povm_local_variable_tag *
copy_parameter_list(parameter_list_t *src, int *param_count_p)
{
    int param_count = 0;
    parameter_list_t *param;
    povm_local_variable_tag *dest;
    int i;

    for (param = src; param; param = param->next) {
        param_count++;
    }
    *param_count_p = param_count;
    dest = mem_malloc(sizeof(povm_local_variable_tag) * param_count);
    
    for (param = src, i = 0; param; param = param->next, i++) {
        dest[i].name = mem_strdup(param->name);
        dest[i].type = copy_type_specifier(param->type);
    }

    return dest;
}

static povm_local_variable_tag *
copy_local_variables(function_definition_t *fd, int param_count)
{
    int i;
    int local_variable_count;
    povm_local_variable_tag *dest;

    local_variable_count = fd->local_variable_count - param_count;

    dest = mem_malloc(sizeof(povm_local_variable_tag) * local_variable_count);

    for (i = 0; i < local_variable_count; i++) {
        dest[i].name
            = mem_strdup(fd->local_variable[i+param_count]->name);
        dest[i].type
            = copy_type_specifier(fd->local_variable[i+param_count]->type);
    }

    return dest;
}

static povm_type_specifier_t *
copy_type_specifier(type_specifier_t *src)
{
    povm_type_specifier_t *dest;
    int derive_count = 0;
    type_derive_t *derive;
    int param_count;
    int i;

    dest = mem_malloc(sizeof(povm_type_specifier_t));

    dest->basic_type = src->basic_type;

    for (derive = src->derive; derive; derive = derive->next) {
        derive_count++;
    }
    dest->derive_count = derive_count;
    dest->derive = mem_malloc(sizeof(povm_type_derive_t) * derive_count);
    for (i = 0, derive = src->derive; derive;
         derive = derive->next, i++) {
        switch (derive->tag) {
        case POVM_FUNCTION_DERIVE:
            dest->derive[i].tag = POVM_FUNCTION_DERIVE;
            dest->derive[i].u.function_d.parameter
                = copy_parameter_list(derive->u.function_d.parameter_list,
                                      &param_count);
            dest->derive[i].u.function_d.parameter_count = param_count;
            break;
        default:
            debug_assert(0, ("derive->tag..%d\n", derive->tag));
        }
    }

    return dest;
}

static void
add_global_variable(poc_compiler_t *compiler, povm_executable_t *exe)
{
    declaration_list_t *dl;
    int i;
    int var_count = 0;

    for (dl = compiler->declaration_list; dl; dl = dl->next) {
        var_count++;
    }
    exe->global_variable_count = var_count;
    exe->global_variable = mem_malloc(sizeof(povm_variable_tag) * var_count);

    for (dl = compiler->declaration_list, i = 0; dl; dl = dl->next, i++) {
        exe->global_variable[i].name = mem_strdup(dl->declaration->name);
        exe->global_variable[i].type
            = copy_type_specifier(dl->declaration->type);
    }
}

static void
add_line_number(OpcodeBuf *ob, int line_number, int start_pc)
{
    if (ob->line_number == NULL
        || (ob->line_number[ob->line_number_size-1].line_number
            != line_number)) {
        ob->line_number = mem_realloc(ob->line_number,
                                      sizeof(povm_line_number_tag)
                                      * (ob->line_number_size + 1));

        ob->line_number[ob->line_number_size].line_number = line_number;
        ob->line_number[ob->line_number_size].start_pc = start_pc;
        ob->line_number[ob->line_number_size].pc_count
            = ob->size - start_pc;
        ob->line_number_size++;

    } else {
        ob->line_number[ob->line_number_size-1].pc_count
            += ob->size - start_pc;
    }
}

static void
generate_code(OpcodeBuf *ob, int line_number, povm_opcode_enum code, ...)
{
    va_list     ap;
    int         i;
    char        *param;
    int         param_count;
    int         start_pc;

    va_start(ap, code);

    param = dvm_opcode_info[(int)code].parameter;
    param_count = strlen(param);
    if (ob->alloc_size < ob->size + 1 + (param_count * 2)) {
        ob->code = mem_realloc(ob->code, ob->alloc_size + OPCODE_ALLOC_SIZE);
        ob->alloc_size += OPCODE_ALLOC_SIZE;
    }

    start_pc = ob->size;
    ob->code[ob->size] = code;
    ob->size++;
    for (i = 0; param[i] != '\0'; i++) {
        unsigned int value = va_arg(ap, int);
        switch (param[i]) {
        case 'b': /* byte */
            ob->code[ob->size] = (povm_byte)value;
            ob->size++;
            break;
        case 's': /* short(2byte int) */
            ob->code[ob->size] = (povm_byte)(value >> 8);
            ob->code[ob->size+1] = (povm_byte)(value & 0xff);
            ob->size += 2;
            break;
        case 'p': /* constant pool index */
            ob->code[ob->size] = (povm_byte)(value >> 8);
            ob->code[ob->size+1] = (povm_byte)(value & 0xff);
            ob->size += 2;
            break;
        default:
            debug_assert(0, ("param..%s, i..%d", param, i));
        }
    }
    add_line_number(ob, line_number, start_pc);

    va_end(ap);
}

static void
generate_boolean_expression(povm_executable_t *cf, expression_t *expr,
                            OpcodeBuf *ob)
{
    if (expr->u.boolean_value) {
        generate_code(ob, expr->line_number, POVM_PUSH_INT_1BYTE, 1);
    } else {
        generate_code(ob, expr->line_number, POVM_PUSH_INT_1BYTE, 0);
    }
}

static void
generate_int_expression(povm_executable_t *cf, expression_t *expr,
                        OpcodeBuf *ob)
{
    povm_constant_pool_tag cp;
    int cp_idx;

    if (expr->u.int_value >= 0 && expr->u.int_value < 256) {
        generate_code(ob, expr->line_number,
                      POVM_PUSH_INT_1BYTE, expr->u.int_value);
    } else if (expr->u.int_value >= 0 && expr->u.int_value < 65536) {
        generate_code(ob, expr->line_number,
                      POVM_PUSH_INT_2BYTE, expr->u.int_value);
    } else {
        cp.tag = POVM_CONSTANT_INT;
        cp.u.c_int = expr->u.int_value;
        cp_idx = add_constant_pool(cf, &cp);

        generate_code(ob, expr->line_number, POVM_PUSH_INT, cp_idx);
    }
}

static void
generate_double_expression(povm_executable_t *cf, expression_t *expr,
                           OpcodeBuf *ob)
{
    povm_constant_pool_tag cp;
    int cp_idx;

    if (expr->u.double_value == 0.0) {
        generate_code(ob, expr->line_number, POVM_PUSH_DOUBLE_0);
    } else if (expr->u.double_value == 1.0) {
        generate_code(ob, expr->line_number, POVM_PUSH_DOUBLE_1);
    } else {
        cp.tag = POVM_CONSTANT_DOUBLE;
        cp.u.c_double = expr->u.double_value;
        cp_idx = add_constant_pool(cf, &cp);

        generate_code(ob, expr->line_number, POVM_PUSH_DOUBLE, cp_idx);
    }
}

static void
generate_string_expression(povm_executable_t *cf, expression_t *expr,
                           OpcodeBuf *ob)
{
    povm_constant_pool_tag cp;
    int cp_idx;

    cp.tag = POVM_CONSTANT_STRING;
    cp.u.c_string = expr->u.string_value;
    cp_idx = add_constant_pool(cf, &cp);
    generate_code(ob, expr->line_number, POVM_PUSH_STRING, cp_idx);
}

static int
get_opcode_type_offset(povm_basic_type_enum basic_type)
{
    switch (basic_type) {
    case POVM_BOOLEAN_TYPE:
        return 0;
        break;
    case POVM_INT_TYPE:
        return 0;
        break;
    case POVM_DOUBLE_TYPE:
        return 1;
        break;
    case POVM_STRING_TYPE:
        return 2;
        break;
    default:
        debug_assert(0, ("basic_type..%d", basic_type));
    }

    return 0;
}

static void
generate_identifier_expression(povm_executable_t *exe, block_t *block,
                               expression_t *expr, OpcodeBuf *ob)
{
    if (expr->u.identifier.is_function) {
        generate_code(ob, expr->line_number,
                      POVM_PUSH_FUNCTION,
                      expr->u.identifier.u.function->index);
        return;
    }

    if (expr->u.identifier.u.declaration->is_local) {
        generate_code(ob,  expr->line_number,
                      POVM_PUSH_STACK_INT
                      + get_opcode_type_offset(expr->u.identifier
                                               .u.declaration
                                               ->type->basic_type),
                      expr->u.identifier.u.declaration->variable_index);
    } else {
        generate_code(ob, expr->line_number,
                      POVM_PUSH_STATIC_INT
                      + get_opcode_type_offset(expr->u.identifier
                                               .u.declaration
                                               ->type->basic_type),
                      expr->u.identifier.u.declaration->variable_index);
    }
}

static void generate_expression(povm_executable_t *exe, block_t *current_block,
                                expression_t *expr, OpcodeBuf *ob);

static void
generate_pop_to_identifier(declaration_tag *decl, int line_number,
                           OpcodeBuf *ob)
{
    if (decl->is_local) {
        generate_code(ob, line_number,
                      POVM_POP_STACK_INT
                      + get_opcode_type_offset(decl->type->basic_type),
                      decl->variable_index);
    } else {
        generate_code(ob, line_number,
                      POVM_POP_STATIC_INT
                      + get_opcode_type_offset(decl->type->basic_type),
                      decl->variable_index);
    }
}

static void
generate_assign_expression(povm_executable_t *exe, block_t *block,
                           expression_t *expr, OpcodeBuf *ob,
                           povm_boolean_enum is_toplevel)
{
    if (expr->u.assign_expression.operator != NORMAL_ASSIGN) {
        generate_identifier_expression(exe, block, 
                                       expr->u.assign_expression.left,
                                       ob);
    }
    generate_expression(exe, block, expr->u.assign_expression.operand, ob);

    switch (expr->u.assign_expression.operator) {
    case NORMAL_ASSIGN :
        break;
    case ADD_ASSIGN:
        generate_code(ob, expr->line_number,
                      POVM_ADD_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    case SUB_ASSIGN:
        generate_code(ob, expr->line_number,
                      POVM_SUB_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    case MUL_ASSIGN:
        generate_code(ob, expr->line_number,
                      POVM_MUL_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    case DIV_ASSIGN:
        generate_code(ob, expr->line_number,
                      POVM_DIV_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    case MOD_ASSIGN:
        generate_code(ob, expr->line_number,
                      POVM_MOD_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    default:
        debug_assert(0, ("operator..%d\n", expr->u.assign_expression.operator));
    }

    if (!is_toplevel) {
        generate_code(ob, expr->line_number, POVM_DUPLICATE);
    }
    generate_pop_to_identifier(expr->u.assign_expression.left
                               ->u.identifier.u.declaration,
                               expr->line_number,
                               ob);
}

static void
generate_binary_expression(povm_executable_t *exe, block_t *block,
                           expression_t *expr, povm_opcode_enum code,
                           OpcodeBuf *ob)
{
    debug_assert(expr->u.binary_expression.left->type->basic_type
               == expr->u.binary_expression.right->type->basic_type,
               ("left..%d, right..%d",
                expr->u.binary_expression.left->type->basic_type,
                expr->u.binary_expression.right->type->basic_type));

    generate_expression(exe, block, expr->u.binary_expression.left, ob);
    generate_expression(exe, block, expr->u.binary_expression.right, ob);
    generate_code(ob, expr->line_number,
                  code
                  + get_opcode_type_offset(expr->u.binary_expression.left
                                           ->type->basic_type));
}

static int
get_label(OpcodeBuf *ob)
{
    int ret;

    if (ob->label_table_alloc_size < ob->label_table_size + 1) {
        ob->label_table = mem_realloc(ob->label_table,
                                      (ob->label_table_alloc_size
                                       + LABEL_TABLE_ALLOC_SIZE)
                                      * sizeof(LabelTable));
        ob->label_table_alloc_size += LABEL_TABLE_ALLOC_SIZE;
    }
    ret = ob->label_table_size;
    ob->label_table_size++;

    return ret;
}

static void
set_label(OpcodeBuf *ob, int label)
{
    ob->label_table[label].label_address = ob->size;
}

static void
generate_logical_and_expression(povm_executable_t *exe, block_t *block,
                                expression_t *expr,
                                OpcodeBuf *ob)
{
    int false_label;

    false_label = get_label(ob);
    generate_expression(exe, block, expr->u.binary_expression.left, ob);
    generate_code(ob, expr->line_number, POVM_DUPLICATE);
    generate_code(ob, expr->line_number, POVM_JUMP_IF_FALSE, false_label);
    generate_expression(exe, block, expr->u.binary_expression.right, ob);
    generate_code(ob, expr->line_number, POVM_LOGICAL_AND);
    set_label(ob, false_label);
}

static void
generate_logical_or_expression(povm_executable_t *exe, block_t *block,
                               expression_t *expr,
                               OpcodeBuf *ob)
{
    int true_label;

    true_label = get_label(ob);
    generate_expression(exe, block, expr->u.binary_expression.left, ob);
    generate_code(ob, expr->line_number, POVM_DUPLICATE);
    generate_code(ob, expr->line_number, POVM_JUMP_IF_TRUE, true_label);
    generate_expression(exe, block, expr->u.binary_expression.right, ob);
    generate_code(ob, expr->line_number, POVM_LOGICAL_OR);
    set_label(ob, true_label);
}

static void
generate_cast_expression(povm_executable_t *exe, block_t *block,
                         expression_t *expr, OpcodeBuf *ob)
{
    generate_expression(exe, block, expr->u.cast.operand, ob);
    switch (expr->u.cast.type) {
    case INT_TO_DOUBLE_CAST:
        generate_code(ob, expr->line_number, POVM_CAST_INT_TO_DOUBLE);
        break;
    case DOUBLE_TO_INT_CAST:
        generate_code(ob, expr->line_number, POVM_CAST_DOUBLE_TO_INT);
        break;
    case BOOLEAN_TO_STRING_CAST:
        generate_code(ob, expr->line_number, POVM_CAST_BOOLEAN_TO_STRING);
        break;
    case INT_TO_STRING_CAST:
        generate_code(ob, expr->line_number, POVM_CAST_INT_TO_STRING);
        break;
    case DOUBLE_TO_STRING_CAST:
        generate_code(ob, expr->line_number, POVM_CAST_DOUBLE_TO_STRING);
        break;
    default:
        debug_assert(0, ("expr->u.cast.type..%d", expr->u.cast.type));
    }
}

static void
generate_inc_dec_expression(povm_executable_t *exe, block_t *block,
                            expression_t *expr, expression_kind_enum kind,
                            OpcodeBuf *ob, povm_boolean_enum is_toplevel)
{
    generate_expression(exe, block, expr->u.inc_dec.operand, ob);

    if (kind == INCREMENT_EXPRESSION) {
        generate_code(ob, expr->line_number, POVM_INCREMENT);
    } else {
        debug_assert(kind == DECREMENT_EXPRESSION, ("kind..%d\n", kind));
        generate_code(ob, expr->line_number, POVM_DECREMENT);
    }
    if (!is_toplevel) {
        generate_code(ob, expr->line_number, POVM_DUPLICATE);
    }
    generate_pop_to_identifier(expr->u.inc_dec.operand
                               ->u.identifier.u.declaration,
                               expr->line_number,
                               ob);
}

static void
generate_function_call_expression(povm_executable_t *exe, block_t *block,
                                  expression_t *expr, OpcodeBuf *ob)
{
    function_call_expression_tag *fce = &expr->u.function_call_expression;
    argument_list_t *arg_pos;

    for (arg_pos = fce->argument; arg_pos; arg_pos = arg_pos->next) {
        generate_expression(exe, block, arg_pos->expression, ob);
    }
    generate_expression(exe, block, fce->function, ob);
    generate_code(ob, expr->line_number, POVM_INVOKE);
}

static void
generate_expression(povm_executable_t *exe, block_t *current_block,
                    expression_t *expr, OpcodeBuf *ob)
{
    switch (expr->kind) {
    case BOOLEAN_EXPRESSION:
        generate_boolean_expression(exe, expr, ob);
        break;
    case INT_EXPRESSION:
        generate_int_expression(exe, expr, ob);
        break;
    case DOUBLE_EXPRESSION:
        generate_double_expression(exe, expr, ob);
        break;
    case STRING_EXPRESSION:
        generate_string_expression(exe, expr, ob);
        break;
    case IDENTIFIER_EXPRESSION:
        generate_identifier_expression(exe, current_block,
                                       expr, ob);
        break;
    case COMMA_EXPRESSION:
        generate_expression(exe, current_block, expr->u.comma.left, ob);
        generate_expression(exe, current_block, expr->u.comma.right, ob);
        break;
    case ASSIGN_EXPRESSION:
        generate_assign_expression(exe, current_block, expr, ob, POVM_FALSE);
        break;
    case ADD_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_ADD_INT, ob);
        break;
    case SUB_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_SUB_INT, ob);
        break;
    case MUL_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_MUL_INT, ob);
        break;
    case DIV_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_DIV_INT, ob);
        break;
    case MOD_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_MOD_INT, ob);
        break;
    case EQ_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_EQ_INT, ob);
        break;
    case NE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_NE_INT, ob);
        break;
    case GT_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_GT_INT, ob);
        break;
    case GE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_GE_INT, ob);
        break;
    case LT_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_LT_INT, ob);
        break;
    case LE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   POVM_LE_INT, ob);
        break;
    case LOGICAL_AND_EXPRESSION:
        generate_logical_and_expression(exe, current_block, expr, ob);
        break;
    case LOGICAL_OR_EXPRESSION:
        generate_logical_or_expression(exe, current_block, expr, ob);
        break;
    case MINUS_EXPRESSION:
        generate_expression(exe, current_block, expr->u.minus_expression, ob);
        generate_code(ob, expr->line_number,
                      POVM_MINUS_INT
                      + get_opcode_type_offset(expr->type->basic_type));
        break;
    case LOGICAL_NOT_EXPRESSION:
        generate_expression(exe, current_block, expr->u.logical_not, ob);
        generate_code(ob, expr->line_number, POVM_LOGICAL_NOT);
        break;
    case FUNCTION_CALL_EXPRESSION:
        generate_function_call_expression(exe, current_block,
                                          expr, ob);
        break;
    case INCREMENT_EXPRESSION:  /* FALLTHRU */
    case DECREMENT_EXPRESSION:
        generate_inc_dec_expression(exe, current_block, expr, expr->kind,
                                    ob, POVM_FALSE);
        break;
    case CAST_EXPRESSION:
        generate_cast_expression(exe, current_block, expr, ob);
        break;
    case EXPRESSION_KIND_COUNT_PLUS_1:  /* FALLTHRU */
    default:
        debug_assert(0, ("expr->kind..%d", expr->kind));
    }
}

static void
generate_expression_statement(povm_executable_t *exe, block_t *block,
                              expression_t *expr, OpcodeBuf *ob)
{
    if (expr->kind == ASSIGN_EXPRESSION) {
        generate_assign_expression(exe, block, expr, ob, POVM_TRUE);
    } else if (expr->kind == INCREMENT_EXPRESSION
               || expr->kind == DECREMENT_EXPRESSION) {
        generate_inc_dec_expression(exe, block, expr, expr->kind, ob,
                                    POVM_TRUE);
    } else {
        generate_expression(exe, block, expr, ob);
        generate_code(ob, expr->line_number, POVM_POP);
    }
}

static void generate_statement_list(povm_executable_t *exe, block_t *current_block,
                                    statement_list_t *statement_list,
                                    OpcodeBuf *ob);

static void
generate_if_statement(povm_executable_t *exe, block_t *block,
                      statement_t *statement, OpcodeBuf *ob)
{
    int if_false_label;
    int end_label;
    if_statement_tag *if_s = &statement->u.if_s;
    elsif_t *elsif;

    generate_expression(exe, block, if_s->condition, ob);
    if_false_label = get_label(ob);
    generate_code(ob, statement->line_number,
                  POVM_JUMP_IF_FALSE, if_false_label);
    generate_statement_list(exe, if_s->then_block,
                            if_s->then_block->statement_list, ob);
    end_label = get_label(ob);
    generate_code(ob, statement->line_number, POVM_JUMP, end_label);
    set_label(ob, if_false_label);

    for (elsif = if_s->elsif_list; elsif; elsif = elsif->next) {
        generate_expression(exe, block, elsif->condition, ob);
        if_false_label = get_label(ob);
        generate_code(ob, statement->line_number, POVM_JUMP_IF_FALSE,
                      if_false_label);
        generate_statement_list(exe, elsif->block,
                                elsif->block->statement_list, ob);
        generate_code(ob, statement->line_number, POVM_JUMP, end_label);
        set_label(ob, if_false_label);
    }
    if (if_s->else_block) {
        generate_statement_list(exe, if_s->else_block,
                                if_s->else_block->statement_list,
                                ob);
    }
    set_label(ob, end_label);
}

static void
generate_while_statement(povm_executable_t *exe, block_t *block,
                         statement_t *statement, OpcodeBuf *ob)
{
    int loop_label;
    while_statement_tag *while_s = &statement->u.while_s;

    loop_label = get_label(ob);
    set_label(ob, loop_label);

    generate_expression(exe, block, while_s->condition, ob);

    while_s->block->parent.statement.break_label = get_label(ob);
    while_s->block->parent.statement.continue_label = get_label(ob);

    generate_code(ob, statement->line_number,
                  POVM_JUMP_IF_FALSE,
                  while_s->block->parent.statement.break_label);
    generate_statement_list(exe, while_s->block,
                            while_s->block->statement_list, ob);

    set_label(ob, while_s->block->parent.statement.continue_label);
    generate_code(ob, statement->line_number, POVM_JUMP, loop_label);
    set_label(ob, while_s->block->parent.statement.break_label);
}

static void
generate_for_statement(povm_executable_t *exe, block_t *block,
                       statement_t *statement, OpcodeBuf *ob)
{
    int loop_label;
    for_statement_tag *for_s = &statement->u.for_s;

    if (for_s->init) {
        generate_expression_statement(exe, block, for_s->init, ob);
    }
    loop_label = get_label(ob);
    set_label(ob, loop_label);

    if (for_s->condition) {
        generate_expression(exe, block, for_s->condition, ob);
    }

    for_s->block->parent.statement.break_label = get_label(ob);
    for_s->block->parent.statement.continue_label = get_label(ob);

    if (for_s->condition) {
        generate_code(ob, statement->line_number,
                      POVM_JUMP_IF_FALSE,
                      for_s->block->parent.statement.break_label);
    }

    generate_statement_list(exe, for_s->block,
                            for_s->block->statement_list, ob);
    set_label(ob, for_s->block->parent.statement.continue_label);

    if (for_s->post) {
        generate_expression_statement(exe, block, for_s->post, ob);
    }

    generate_code(ob, statement->line_number,
                  POVM_JUMP, loop_label);
    set_label(ob, for_s->block->parent.statement.break_label);
}

static void
generate_return_statement(povm_executable_t *exe, block_t *block,
                          statement_t *statement, OpcodeBuf *ob)
{
    debug_assert(statement->u.return_s.return_value != NULL,
               ("return value is null."));

    generate_expression(exe, block, statement->u.return_s.return_value, ob);
    generate_code(ob, statement->line_number, POVM_RETURN);
}

static void
generate_break_statement(povm_executable_t *exe, block_t *block,
                         statement_t *statement, OpcodeBuf *ob)
{
    break_statement_tag *break_s = &statement->u.break_s;
    block_t       *block_p;

    for (block_p = block; block_p; block_p = block_p->outer_block) {
        if (block_p->type != WHILE_STATEMENT_BLOCK
            && block_p->type != FOR_STATEMENT_BLOCK)
            continue;

        if (break_s->label == NULL) {
            break;
        }

        if (block_p->type == WHILE_STATEMENT_BLOCK) {
            if (block_p->parent.statement.statement->u.while_s.label == NULL)
                continue;

            if (!strcmp(break_s->label,
                        block_p->parent.statement.statement
                        ->u.while_s.label)) {
                break;
            }
        } else if (block_p->type == FOR_STATEMENT_BLOCK) {
            if (block_p->parent.statement.statement->u.for_s.label == NULL)
                continue;

            if (!strcmp(break_s->label,
                        block_p->parent.statement.statement
                        ->u.for_s.label)) {
                break;
            }
        }
    }
    if (block_p == NULL) {
        poc_compile_error(statement->line_number,
                          LABEL_NOT_FOUND_ERR,
                          STRING_MESSAGE_ARGUMENT, "label", break_s->label,
                          MESSAGE_ARGUMENT_END);
    }

    generate_code(ob, statement->line_number,
                  POVM_JUMP,
                  block_p->parent.statement.break_label);

}

static void
generate_continue_statement(povm_executable_t *exe, block_t *block,
                            statement_t *statement, OpcodeBuf *ob)
{
    continue_statement_tag *continue_s = &statement->u.continue_s;
    block_t       *block_p;
    

    for (block_p = block; block_p; block_p = block_p->outer_block) {
        if (block_p->type != WHILE_STATEMENT_BLOCK
            && block_p->type != FOR_STATEMENT_BLOCK)
            continue;

        if (continue_s->label == NULL) {
            break;
        }

        if (block_p->type == WHILE_STATEMENT_BLOCK) {
            if (block_p->parent.statement.statement->u.while_s.label == NULL)
                continue;

            if (!strcmp(continue_s->label,
                        block_p->parent.statement.statement
                        ->u.while_s.label)) {
                break;
            }
        } else if (block_p->type == FOR_STATEMENT_BLOCK) {
            if (block_p->parent.statement.statement->u.for_s.label == NULL)
                continue;

            if (!strcmp(continue_s->label,
                        block_p->parent.statement.statement
                        ->u.for_s.label)) {
                break;
            }
        }
    }
    if (block_p == NULL) {
        poc_compile_error(statement->line_number,
                          LABEL_NOT_FOUND_ERR,
                          STRING_MESSAGE_ARGUMENT, "label", continue_s->label,
                          MESSAGE_ARGUMENT_END);
    }
    generate_code(ob, statement->line_number,
                  POVM_JUMP,
                  block_p->parent.statement.continue_label);
}

static void
generate_initializer(povm_executable_t *exe, block_t *block,
                     statement_t *statement, OpcodeBuf *ob)
{
    declaration_tag *decl = statement->u.declaration_s;
    if (decl->initializer == NULL)
        return;

    generate_expression(exe, block, decl->initializer, ob);
    generate_pop_to_identifier(decl, statement->line_number, ob);
}

static void
generate_statement_list(povm_executable_t *exe, block_t *current_block,
                        statement_list_t *statement_list,
                        OpcodeBuf *ob)
{
    statement_list_t *pos;

    for (pos = statement_list; pos; pos = pos->next) {
        switch (pos->statement->type) {
        case EXPRESSION_STATEMENT:
            generate_expression_statement(exe, current_block,
                                          pos->statement->u.expression_s, ob);
            break;
        case IF_STATEMENT:
            generate_if_statement(exe, current_block, pos->statement, ob);
            break;
        case WHILE_STATEMENT:
            generate_while_statement(exe, current_block, pos->statement, ob);
            break;
        case FOR_STATEMENT:
            generate_for_statement(exe, current_block, pos->statement, ob);
            break;
        case FOREACH_STATEMENT:
            break;
        case RETURN_STATEMENT:
            generate_return_statement(exe, current_block, pos->statement, ob);
            break;
        case BREAK_STATEMENT:
            generate_break_statement(exe, current_block, pos->statement, ob);
            break;
        case CONTINUE_STATEMENT:
            generate_continue_statement(exe, current_block,
                                        pos->statement, ob);
            break;
        case TRY_STATEMENT:
            break;
        case THROW_STATEMENT:
            break;
        case DECLARATION_STATEMENT:
            generate_initializer(exe, current_block,
                                 pos->statement, ob);
            break;
        case STATEMENT_TYPE_COUNT_PLUS_1: /* FALLTHRU */
        default:
            debug_assert(0, ("pos->statement->type..", pos->statement->type));
        }
    }
}

static void
init_opcode_buf(OpcodeBuf *ob)
{
    ob->size = 0;
    ob->alloc_size = 0;
    ob->code = NULL;
    ob->label_table_size = 0;
    ob->label_table_alloc_size = 0;
    ob->label_table = NULL;
    ob->line_number_size = 0;
    ob->line_number = NULL;
}

static void
fix_labels(OpcodeBuf *ob)
{
    int i;
    int j;
    OpcodeInfo *info;
    int label;
    int address;

    for (i = 0; i < ob->size; i++) {
        if (ob->code[i] == POVM_JUMP
            || ob->code[i] == POVM_JUMP_IF_TRUE
            || ob->code[i] == POVM_JUMP_IF_FALSE) {
            label = (ob->code[i+1] << 8) + (ob->code[i+2]);
            address = ob->label_table[label].label_address;
            ob->code[i+1] = (povm_byte)(address >> 8);
            ob->code[i+2] = (povm_byte)(address &0xff);
        }
        info = &dvm_opcode_info[ob->code[i]];
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

static povm_byte *
fix_opcode_buf(OpcodeBuf *ob)
{
    povm_byte *ret;

    fix_labels(ob);
    ret = mem_realloc(ob->code, ob->size);
    mem_free(ob->label_table);

    return ret;
}

static int
calc_need_stack_size(povm_byte *code, int code_size)
{
    int i, j;
    int stack_size = 0;
    OpcodeInfo  *info;

    for (i = 0; i < code_size; i++) {
        info = &dvm_opcode_info[code[i]];
        if (info->stack_increment > 0) {
            stack_size += info->stack_increment;
        }
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

    return stack_size;
}

static void
copy_function(function_definition_t *src, povm_function_tag *dest)
{
    dest->type = copy_type_specifier(src->type);
    dest->name = mem_strdup(src->name);
    dest->parameter = copy_parameter_list(src->parameter,
                                          &dest->parameter_count);
    if (src->block) {
        dest->local_variable
            = copy_local_variables(src, dest->parameter_count);
        dest->local_variable_count
            = src->local_variable_count - dest->parameter_count;
    } else {
        dest->local_variable = NULL;
        dest->local_variable_count = 0;
    }
}

static void
add_functions(poc_compiler_t *compiler, povm_executable_t *exe)
{
    function_definition_t  *fd;
    int         i;
    int         func_count = 0;
    OpcodeBuf           ob;

    for (fd = compiler->function_list; fd; fd = fd->next) {
        func_count++;
    }
    exe->function_count = func_count;
    exe->function = mem_malloc(sizeof(povm_function_tag) * func_count);

    for (fd = compiler->function_list, i = 0; fd; fd = fd->next, i++) {
        copy_function(fd, &exe->function[i]);
        if (fd->block) {
            init_opcode_buf(&ob);
            generate_statement_list(exe, fd->block, fd->block->statement_list,
                                    &ob);

            exe->function[i].is_implemented = POVM_TRUE;
            exe->function[i].code_size = ob.size;
            exe->function[i].code = fix_opcode_buf(&ob);
            exe->function[i].line_number_size = ob.line_number_size;
            exe->function[i].line_number = ob.line_number;
            exe->function[i].need_stack_size
                = calc_need_stack_size(exe->function[i].code,
                                       exe->function[i].code_size);
        } else {
            exe->function[i].is_implemented = POVM_FALSE;
        }
    }
}

static void
add_top_level(poc_compiler_t *compiler, povm_executable_t *exe)
{
    OpcodeBuf           ob;

    init_opcode_buf(&ob);
    generate_statement_list(exe, NULL, compiler->statement_list,
                            &ob);
    
    exe->code_size = ob.size;
    exe->code = fix_opcode_buf(&ob);
    exe->line_number_size = ob.line_number_size;
    exe->line_number = ob.line_number;
    exe->need_stack_size = calc_need_stack_size(exe->code, exe->code_size);
}


povm_executable_t *
poc_generate(poc_compiler_t *compiler)
{
    povm_executable_t      *exe;

    exe = alloc_executable();

    add_global_variable(compiler, exe);
    add_functions(compiler, exe);
    add_top_level(compiler, exe);

    return exe;
}





