/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: povm_dev.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:55:27 by konyka
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
 

#ifndef __DARKBLUE_POVM_DEV_H__
#define __DARKBLUE_POVM_DEV_H__

#include "povm.h"

typedef povm_value_union povm_native_function_proc(povm_virtual_machine_t *dvm,
                                         int arg_count, povm_value_union *args);
void povm_add_native_function(povm_virtual_machine_t *dvm, char *func_name,
                             povm_native_function_proc *proc, int arg_count);




#endif /* __DARKBLUE_POVM_DEV_H__ */
