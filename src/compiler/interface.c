/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: interface.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:25:23 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:18:53
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
#define GLOBAL_VARIABLE_DEFINE
#include "poc.h"

poc_compiler_t *
poc_create_compiler(void)
{
    mem_storage_tp storage;
    poc_compiler_t *compiler;

    storage = mem_open_storage(0);
    compiler = mem_storage_malloc(storage,
                                  sizeof(struct poc_compiler_tag));
    compiler->compile_storage = storage;
    compiler->function_list = NULL;
    compiler->function_count = 0;
    compiler->declaration_list = NULL;
    compiler->statement_list = NULL;
    compiler->current_block = NULL;
    compiler->current_line_number = 1;
    compiler->input_mode = POC_FILE_INPUT_MODE;
#ifdef EUC_SOURCE
    compiler->source_encoding = EUC_ENCODING;
#else
#ifdef SHIFT_JIS_SOURCE
    compiler->source_encoding = SHIFT_JIS_ENCODING;
#else
#ifdef UTF_8_SOURCE
    compiler->source_encoding = UTF_8_ENCODING;
#else
    debug_panic(("source encoding is not defined.\n"));
#endif
#endif
#endif

    poc_set_current_compiler(compiler);

    return compiler;
}

static povm_executable_t *do_compile(poc_compiler_t *compiler)
{
    extern int yyparse(void);
    povm_executable_t *exe;

    poc_set_current_compiler(compiler);
    if (yyparse()) {
        fprintf(stderr, "Error ! Error ! Error !\n");
        exit(1);
    }
    poc_fix_tree(compiler);
    exe = poc_generate(compiler);

    /*
    dvm_disassemble(exe);
    */

    return exe;
}

povm_executable_t *
poc_compile(poc_compiler_t *compiler, FILE *fp)
{
    extern FILE *yyin;
    povm_executable_t *exe;

    compiler->current_line_number = 1;
    compiler->input_mode = POC_FILE_INPUT_MODE;

    yyin = fp;

    exe = do_compile(compiler);

    poc_reset_string_literal_buffer();

    return exe;
}

povm_executable_t *
POC_compile_string(poc_compiler_t *compiler, char **lines)
{
    extern int yyparse(void);
    povm_executable_t *exe;

    poc_set_source_string(lines);
    compiler->current_line_number = 1;
    compiler->input_mode = POC_STRING_INPUT_MODE;

    exe = do_compile(compiler);

    poc_reset_string_literal_buffer();

    return exe;
}

void
poc_dispose_compiler(poc_compiler_t *compiler)
{
    function_definition_t *fd_pos;

    for (fd_pos = compiler->function_list; fd_pos; fd_pos = fd_pos->next) {
        mem_free(fd_pos->local_variable);
    }
    mem_dispose_storage(compiler->compile_storage);
}
