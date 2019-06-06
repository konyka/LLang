/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: create_parse_tree.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 13:58:06 by konyka
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
#include "poc.h"

declaration_list_t *
poc_chain_declaration(declaration_list_t *list, declaration_tag *decl)
{
    declaration_list_t *new_item;
    declaration_list_t *pos;

    new_item = poc_malloc(sizeof(declaration_list_t));
    new_item->declaration = decl;
    new_item->next = NULL;

    if (list == NULL) {
        return new_item;
    }

    for (pos = list; pos->next != NULL; pos = pos->next)
        ;
    pos->next = new_item;

    return list;
}

declaration_tag *
poc_alloc_declaration(type_specifier_t *type, char *identifier)
{
    declaration_tag *decl;

    decl = poc_malloc(sizeof(declaration_tag));
    decl->name = identifier;
    decl->type = type;
    decl->variable_index = -1;

    return decl;
}

static function_definition_t *
create_function_definition(povm_basic_type_enum type, char *identifier,
                           parameter_list_t *parameter_list, block_t *block)
{
    function_definition_t *fd;
    poc_compiler_t *compiler;

    compiler = poc_get_current_compiler();

    fd = poc_malloc(sizeof(function_definition_t));
    fd->type = poc_alloc_type_specifier(type);
    fd->name = identifier;
    fd->parameter = parameter_list;
    fd->block = block;
    fd->index = compiler->function_count;
    compiler->function_count++;
    fd->local_variable_count = 0;
    fd->local_variable = NULL;
    fd->next = NULL;

    return fd;
}

void
poc_function_define(povm_basic_type_enum type, char *identifier,
                    parameter_list_t *parameter_list, block_t *block)
{
    function_definition_t *fd;
    function_definition_t *pos;
    poc_compiler_t *compiler;

    if (poc_search_function(identifier)
        || poc_search_declaration(identifier, NULL)) {
        poc_compile_error(poc_get_current_compiler()->current_line_number,
                          FUNCTION_MULTIPLE_DEFINE_ERR,
                          STRING_MESSAGE_ARGUMENT, "name", identifier,
                          MESSAGE_ARGUMENT_END);
        return;
    }
    fd = create_function_definition(type, identifier, parameter_list,
                                    block);
    if (block) {
        block->type = FUNCTION_BLOCK;
        block->parent.function.function = fd;
    }

    compiler = poc_get_current_compiler();
    if (compiler->function_list) {
        for (pos = compiler->function_list; pos->next; pos = pos->next)
            ;
        pos->next = fd;
    } else {
        compiler->function_list = fd;
    }
}

parameter_list_t *
poc_create_parameter(povm_basic_type_enum type, char *identifier)
{
    parameter_list_t       *p;

    p = poc_malloc(sizeof(parameter_list_t));
    p->name = identifier;
    p->type = poc_alloc_type_specifier(type);
    p->line_number = poc_get_current_compiler()->current_line_number;
    p->next = NULL;

    return p;
}

parameter_list_t *
poc_chain_parameter(parameter_list_t *list, povm_basic_type_enum type,
                    char *identifier)
{
    parameter_list_t *pos;

    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = poc_create_parameter(type, identifier);

    return list;
}

argument_list_t *
poc_create_argument_list(expression_t *expression)
{
    argument_list_t *al;

    al = poc_malloc(sizeof(argument_list_t));
    al->expression = expression;
    al->next = NULL;

    return al;
}

argument_list_t *
poc_chain_argument_list(argument_list_t *list, expression_t *expr)
{
    argument_list_t *pos;

    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = poc_create_argument_list(expr);

    return list;
}

statement_list_t *
poc_create_statement_list(statement_t *statement)
{
    statement_list_t *sl;

    sl = poc_malloc(sizeof(statement_list_t));
    sl->statement = statement;
    sl->next = NULL;

    return sl;
}

statement_list_t *
poc_chain_statement_list(statement_list_t *list, statement_t *statement)
{
    statement_list_t *pos;

    if (list == NULL)
        return poc_create_statement_list(statement);

    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = poc_create_statement_list(statement);

    return list;
}

expression_t *
poc_alloc_expression(expression_kind_enum kind)
{
    expression_t  *exp;

    exp = poc_malloc(sizeof(expression_t));
    exp->type = NULL;
    exp->kind = kind;
    exp->line_number = poc_get_current_compiler()->current_line_number;

    return exp;
}

expression_t *
poc_create_comma_expression(expression_t *left, expression_t *right)
{
    expression_t *exp;

    exp = poc_alloc_expression(COMMA_EXPRESSION);
    exp->u.comma.left = left;
    exp->u.comma.right = right;

    return exp;
}

expression_t *
poc_create_assign_expression(expression_t *left, assignment_operator_enum operator,
                             expression_t *operand)
{
    expression_t *exp;

    exp = poc_alloc_expression(ASSIGN_EXPRESSION);
    exp->u.assign_expression.left = left;
    exp->u.assign_expression.operator = operator;
    exp->u.assign_expression.operand = operand;

    return exp;
}

expression_t *
poc_create_binary_expression(expression_kind_enum operator,
                             expression_t *left, expression_t *right)
{
#if 0
    if ((left->kind == INT_EXPRESSION
         || left->kind == DOUBLE_EXPRESSION)
        && (right->kind == INT_EXPRESSION
            || right->kind == DOUBLE_EXPRESSION)) {
        POC_Value v;
        v = poc_eval_binary_expression(poc_get_current_compiler(),
                                       NULL, operator, left, right);
        /* Overwriting left hand expression. */
        *left = convert_value_to_expression(&v);

        return left;
    } else {
#endif
        expression_t *exp;
        exp = poc_alloc_expression(operator);
        exp->u.binary_expression.left = left;
        exp->u.binary_expression.right = right;
        return exp;
#if 0
    }
#endif
}

expression_t *
poc_create_minus_expression(expression_t *operand)
{
#if 0
    if (operand->kind == INT_EXPRESSION
        || operand->kind == DOUBLE_EXPRESSION) {
        POC_Value       v;
        v = poc_eval_minus_expression(poc_get_current_compiler(),
                                      NULL, operand);
        /* Notice! Overwriting operand expression. */
        *operand = convert_value_to_expression(&v);
        return operand;
    } else {
#endif
        expression_t      *exp;
        exp = poc_alloc_expression(MINUS_EXPRESSION);
        exp->u.minus_expression = operand;
        return exp;
#if 0
    }
#endif
}

expression_t *
poc_create_logical_not_expression(expression_t *operand)
{
    expression_t  *exp;

    exp = poc_alloc_expression(LOGICAL_NOT_EXPRESSION);
    exp->u.logical_not = operand;

    return exp;
}

expression_t *
poc_create_incdec_expression(expression_t *operand, expression_kind_enum inc_or_dec)
{
    expression_t *exp;

    exp = poc_alloc_expression(inc_or_dec);
    exp->u.inc_dec.operand = operand;

    return exp;
}


expression_t *
poc_create_identifier_expression(char *identifier)
{
    expression_t  *exp;

    exp = poc_alloc_expression(IDENTIFIER_EXPRESSION);
    exp->u.identifier.name = identifier;

    return exp;
}

expression_t *
poc_create_function_call_expression(expression_t *function,
                                    argument_list_t *argument)
{
    expression_t  *exp;

    exp = poc_alloc_expression(FUNCTION_CALL_EXPRESSION);
    exp->u.function_call_expression.function = function;
    exp->u.function_call_expression.argument = argument;

    return exp;
}

expression_t *
poc_create_boolean_expression(povm_boolean_enum value)
{
    expression_t *exp;

    exp = poc_alloc_expression(BOOLEAN_EXPRESSION);
    exp->u.boolean_value = value;

    return exp;
}

static statement_t *
alloc_statement(statement_type_enum type)
{
    statement_t *st;

    st = poc_malloc(sizeof(statement_t));
    st->type = type;
    st->line_number = poc_get_current_compiler()->current_line_number;

    return st;
}

statement_t *
poc_create_if_statement(expression_t *condition,
                        block_t *then_block, elsif_t *elsif_list,
                        block_t *else_block)
{
    statement_t *st;

    st = alloc_statement(IF_STATEMENT);
    st->u.if_s.condition = condition;
    st->u.if_s.then_block = then_block;
    st->u.if_s.elsif_list = elsif_list;
    st->u.if_s.else_block = else_block;

    return st;
}

elsif_t *
poc_chain_elsif_list(elsif_t *list, elsif_t *add)
{
    elsif_t *pos;

    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = add;

    return list;
}

elsif_t *
poc_create_elsif(expression_t *expr, block_t *block)
{
    elsif_t *ei;

    ei = poc_malloc(sizeof(elsif_t));
    ei->condition = expr;
    ei->block = block;
    ei->next = NULL;

    return ei;
}

statement_t *
poc_create_while_statement(char *label,
                           expression_t *condition, block_t *block)
{
    statement_t *st;

    st = alloc_statement(WHILE_STATEMENT);
    st->u.while_s.label = label;
    st->u.while_s.condition = condition;
    st->u.while_s.block = block;
    block->type = WHILE_STATEMENT_BLOCK;
    block->parent.statement.statement = st;

    return st;
}

statement_t *
poc_create_for_statement(char *label, expression_t *init, expression_t *cond,
                         expression_t *post, block_t *block)
{
    statement_t *st;

    st = alloc_statement(FOR_STATEMENT);
    st->u.for_s.label = label;
    st->u.for_s.init = init;
    st->u.for_s.condition = cond;
    st->u.for_s.post = post;
    st->u.for_s.block = block;
    block->type = FOR_STATEMENT_BLOCK;
    block->parent.statement.statement = st;

    return st;
}

statement_t *
poc_create_foreach_statement(char *label, char *variable,
                             expression_t *collection, block_t *block)
{
    statement_t *st;

    st = alloc_statement(FOREACH_STATEMENT);
    st->u.foreach_s.label = label;
    st->u.foreach_s.variable = variable;
    st->u.foreach_s.collection = collection;
    st->u.for_s.block = block;

    return st;
}

block_t *
poc_open_block(void)
{
    block_t *new_block;

    poc_compiler_t *compiler = poc_get_current_compiler();
    new_block = poc_malloc(sizeof(block_t));
    new_block->type = UNDEFINED_BLOCK;
    new_block->outer_block = compiler->current_block;
    new_block->declaration_list = NULL;
    compiler->current_block = new_block;

    return new_block;
}

block_t *
poc_close_block(block_t *block, statement_list_t *statement_list)
{
    poc_compiler_t *compiler = poc_get_current_compiler();

    debug_assert(block == compiler->current_block,
               ("block mismatch.\n"));
    block->statement_list = statement_list;
    compiler->current_block = block->outer_block;

    return block;
}

statement_t *
poc_create_expression_statement(expression_t *expression)
{
    statement_t *st;

    st = alloc_statement(EXPRESSION_STATEMENT);
    st->u.expression_s = expression;

    return st;
}

statement_t *
poc_create_return_statement(expression_t *expression)
{
    statement_t *st;

    st = alloc_statement(RETURN_STATEMENT);
    st->u.return_s.return_value = expression;

    return st;
}

statement_t *
poc_create_break_statement(char *label)
{
    statement_t *st;

    st = alloc_statement(BREAK_STATEMENT);
    st->u.break_s.label = label;

    return st;
}

statement_t *
poc_create_continue_statement(char *label)
{
    statement_t *st;

    st = alloc_statement(CONTINUE_STATEMENT);
    st->u.continue_s.label = label;

    return st;
}

statement_t *
poc_create_try_statement(block_t *try_block, char *exception,
                         block_t *catch_block, block_t *finally_block)
{
    statement_t *st;

    st = alloc_statement(TRY_STATEMENT);
    st->u.try_s.try_block = try_block;
    st->u.try_s.catch_block = catch_block;
    st->u.try_s.exception = exception;
    st->u.try_s.finally_block = finally_block;

    return st;
}

statement_t *
poc_create_throw_statement(expression_t *expression)
{
    statement_t *st;

    st = alloc_statement(THROW_STATEMENT);
    st->u.throw_s.exception = expression;

    return st;
}

statement_t *
poc_create_declaration_statement(povm_basic_type_enum type, char *identifier,
                                 expression_t *initializer)
{
    statement_t *st;
    declaration_tag *decl;

    st = alloc_statement(DECLARATION_STATEMENT);

    decl = poc_alloc_declaration(poc_alloc_type_specifier(type), identifier);

    decl->initializer = initializer;

    st->u.declaration_s = decl;

    return st;
}




