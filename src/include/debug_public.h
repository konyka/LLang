/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: debug_public.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 13:17:14 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 15:11:58
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_DEBUG_PUBLIC_H__
#define __DARKBLUE_DEBUG_PUBLIC_H__

#include <stdio.h>      
#include <stdarg.h>

typedef struct debug_controller_tag *debug_controller_tp;
void debug_set(debug_controller_tp controller, char *file, int line);
void debug_set_expression(char *expression);

#ifdef DEBUG_NO_DEBUG
#define debug_create_controller()         ((void)0)
#define debug_set_debug_level(level)      ((void)0)
#define debug_set_debug_write_fp(fp)      ((void)0)
#define debug_assert(expression, arg)     ((void)0)
#define debug_panic(arg)                  ((void)0)
#define debug_debug_write(arg)            ((void)0)
#else /* DEBUG_NO_DEBUG */
#ifdef DEBUG_CONTROLLER
#define DEBUG_CURRENT_CONTROLLER  DEBUG_CONTROLLER
#else /* DEBUG_CONTROLLER */
#define DEBUG_CURRENT_CONTROLLER  debug_default_controller
#endif /* DEBUG_CONTROLLER */
extern debug_controller_tp DEBUG_CURRENT_CONTROLLER;

#define debug_create_controller() (debug_create_controller_func())
#define debug_set_debug_level(level) \
(debug_set_debug_level_func(DEBUG_CURRENT_CONTROLLER, (level)))
#define debug_set_debug_write_fp(fp) \
(debug_set_debug_write_fp(DEBUG_CURRENT_CONTROLLER, (fp))
#define debug_assert(expression, arg) \
 ((expression) ? (void)(0) :\
  ((debug_set(DEBUG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
   (debug_set_expression(#expression)),\
   debug_assert_func arg))
#define debug_panic(arg) \
 ((debug_set(DEBUG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
  debug_panic_func arg)
#define debug_debug_write(arg) \
 ((debug_set(DEBUG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
  debug_debug_write_func arg)
#endif /* DEBUG_NO_DEBUG */

typedef enum {
    DEBUG_TRUE = 1,
    DEBUG_FALSE = 0
} debug_boolean;

debug_controller_tp debug_create_controller_func(void);
void debug_set_debug_level_func(debug_controller_tp controller, int level);
void debug_set_debug_write_fp_func(debug_controller_tp controller, FILE *fp);
void debug_assert_func(char *fmt, ...);
void debug_panic_func(char *fmt, ...);
void debug_debug_write_func(int level, char *fmt, ...);



#endif /* __DARKBLUE_DEBUG_PUBLIC_H__ */
