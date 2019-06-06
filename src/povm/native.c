/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: native.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:53:06 by konyka
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
#include "povm_pri.h"

static povm_value_union
nv_print_proc(povm_virtual_machine_t *dvm,
              int arg_count, povm_value_union *args)
{
    povm_value_union ret;

    ret.int_value = 0;

    debug_assert(arg_count == 1, ("arg_count..%d", arg_count));
    dvm_print_wcs(stdout, args[0].object->u.string.string);
    fflush(stdout);

    return ret;
}

void
dvm_add_native_functions(povm_virtual_machine_t *dvm)
{
    povm_add_native_function(dvm, "print", nv_print_proc, 1);
}
