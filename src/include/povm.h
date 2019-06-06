/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: povm.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 15:08:33 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 23:02:53
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_POVM_H__
#define __DARKBLUE_POVM_H__



typedef struct povm_excecutable_tag povm_executable_t;
typedef struct povm_virtual_machine_tag povm_virtual_machine_t;

typedef struct povm_object_tag povm_object_t;
typedef struct povm_string_tag povm_string;

typedef enum {
    POVM_FALSE = 0,
    POVM_TRUE = 1
} povm_boolean_enum;

typedef union {
    int         int_value;
    double      double_value;
    povm_object_t  *object;
} povm_value_union;

povm_virtual_machine_t *povm_create_virtual_machine(void);
void povm_add_executable(povm_virtual_machine_t *dvm, povm_executable_t *executable);
povm_value_union povm_execute(povm_virtual_machine_t *dvm);
void povm_dispose_virtual_machine(povm_virtual_machine_t *dvm);




#endif /* __DARKBLUE_POVM_H__ */
