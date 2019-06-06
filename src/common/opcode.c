/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: opcode.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 15:10:49 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 18:19:52
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include "common.h"

OpcodeInfo dvm_opcode_info[] = {
    {"dummy", "", 0},
    {"push_int_1byte", "b", 1},
    {"push_int_2byte", "s", 1},
    {"push_int", "p", 1},
    {"push_double_0", "", 1},
    {"push_double_1", "", 1},
    {"push_double", "p", 1},
    {"push_string", "p", 1},
    /**********/
    {"push_stack_int", "s", 1},
    {"push_stack_double", "s", 1},
    {"push_stack_string", "s", 1},
    {"pop_stack_int", "s", -1},
    {"pop_stack_double", "s", -1},
    {"pop_stack_string", "s", -1},
    /**********/
    {"push_static_int", "s", 1},
    {"push_static_double", "s", 1},
    {"push_static_string", "s", 1},
    {"pop_static_int", "s", -1},
    {"pop_static_double", "s", -1},
    {"pop_static_string", "s", -1},
    /**********/
    {"add_int", "", -1},
    {"add_double", "", -1},
    {"add_string", "", -1},
    {"sub_int", "", -1},
    {"sub_double", "", -1},
    {"mul_int", "", -1},
    {"mul_double", "", -1},
    {"div_int", "", -1},
    {"div_double", "", -1},
    {"mod_int", "", -1},
    {"mod_double", "", -1},
    {"minus_int", "", 0},
    {"minus_double", "", 0},
    {"increment", "", 0},
    {"decrement", "", 0},
    {"cast_int_to_double", "", 0},
    {"cast_double_to_int", "", 0},
    {"cast_boolean_to_string", "", 0},
    {"cast_int_to_string", "", 0},
    {"cast_double_to_string", "", 0},
    {"eq_int", "", -1},
    {"eq_double", "", -1},
    {"eq_string", "", -1},
    {"gt_int", "", -1},
    {"gt_double", "", -1},
    {"gt_string", "", -1},
    {"ge_int", "", -1},
    {"ge_double", "", -1},
    {"ge_string", "", -1},
    {"lt_int", "", -1},
    {"lt_double", "", -1},
    {"lt_string", "", -1},
    {"le_int", "", -1},
    {"le_double", "", -1},
    {"le_string", "", -1},
    {"ne_int", "", -1},
    {"ne_double", "", -1},
    {"ne_string", "", -1},
    {"logical_and", "", -1},
    {"logical_or", "", -1},
    {"logical_not", "", 0},
    {"pop", "", -1},
    {"duplicate", "", 1},
    {"jump", "s", 0},
    {"jump_if_true", "s", -1},
    {"jump_if_false", "s", -1},
    /**********/
    {"push_function", "s", 0},
    {"invoke", "", -1},
    {"return", "", -1},
};
