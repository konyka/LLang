/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: main.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:25:46 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:08:12
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include <stdio.h>
#include <locale.h>
#include "poc_public.h"
#include "povm.h"
#include "memory_public.h"

int
main(int argc, char **argv)
{
    poc_compiler_t *compiler;
    FILE *fp;
    povm_executable_t *exe;
    povm_virtual_machine_t *dvm;

    if (argc < 2) {
        fprintf(stderr, "usage:%s filename arg1, arg2, ...", argv[0]);
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "%s not found.\n", argv[1]);
        exit(1);
    }

    setlocale(LC_CTYPE, "");
    compiler = poc_create_compiler();
    exe = poc_compile(compiler, fp);
    dvm = povm_create_virtual_machine();
    povm_add_executable(dvm,exe);
    povm_execute(dvm);
    poc_dispose_compiler(compiler);
    povm_dispose_virtual_machine(dvm);

    mem_check_all_blocks();
    mem_dump_blocks(stdout);

    return 0;
}
