/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: memory_public.h
 *      Version: v0.0.0
 *   Created on: 2019-06-05 09:43:57 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 11:44:37
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_MEMORY_PUBLIC_H__
#define __DARKBLUE_MEMORY_PUBLIC_H__

//public
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    MEM_FAIL_AND_EXIT,
    MEM_FAIL_AND_RETURN
} mem_fail_mode_enum;

typedef struct mem_controller_tag *mem_controller_tp;
typedef void (*mem_error_handler)(mem_controller_tp, char *, int, char *);
typedef struct mem_storage_tag *mem_storage_tp;

extern mem_controller_tp mem_default_controller;

#ifdef MEM_CONTROLLER
#define MEM_CURRENT_CONTROLLER MEM_CONTROLLER
#else /* MEM_CONTROLLER */
#define MEM_CURRENT_CONTROLLER mem_default_controller
#endif /* MEM_CONTROLLER */

/*
 * Don't use mem_*_func function.
 * There are private functions of MEM module.
 */
mem_controller_tp mem_create_controller(void);
void *mem_malloc_func(mem_controller_tp controller,
                      char *filename, int line, size_t size);
void *mem_realloc_func(mem_controller_tp controller,
                       char *filename, int line, void *ptr, size_t size);
char *MEM_strdup_func(mem_controller_tp controller,
                      char *filename, int line, char *str);
mem_storage_tp mem_open_storage_func(mem_controller_tp controller,
                                  char *filename, int line, int page_size);
void *mem_storage_malloc_func(mem_controller_tp controller,
                              char *filename, int line,
                              mem_storage_tp storage, size_t size);
void mem_free_func(mem_controller_tp controller, void *ptr);
void mem_dispose_storage_func(mem_controller_tp controller,
                              mem_storage_tp storage);

void mem_set_error_handler(mem_controller_tp controller,
                           mem_error_handler handler);
void mem_set_fail_mode(mem_controller_tp controller,
                       mem_fail_mode_enum mode);
void mem_dump_blocks_func(mem_controller_tp controller, FILE *fp);
void mem_check_block_func(mem_controller_tp controller,
                          char *filename, int line, void *p);
void mem_check_all_blocks_func(mem_controller_tp controller,
                               char *filename, int line);

#define mem_malloc(size)\
  (mem_malloc_func(MEM_CURRENT_CONTROLLER,\
                   __FILE__, __LINE__, size))
#define mem_realloc(ptr, size)\
  (mem_realloc_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, ptr, size))
#define mem_strdup(str)\
  (MEM_strdup_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, str))
#define mem_open_storage(page_size)\
  (mem_open_storage_func(MEM_CURRENT_CONTROLLER,\
                         __FILE__, __LINE__, page_size))
#define mem_storage_malloc(storage, size)\
  (mem_storage_malloc_func(MEM_CURRENT_CONTROLLER,\
                           __FILE__, __LINE__, storage, size))
#define mem_free(ptr) (mem_free_func(MEM_CURRENT_CONTROLLER, ptr))
#define mem_dispose_storage(storage)\
  (mem_dispose_storage_func(MEM_CURRENT_CONTROLLER, storage))
#ifdef DEBUG
#define mem_dump_blocks(fp)\
  (mem_dump_blocks_func(MEM_CURRENT_CONTROLLER, fp))
#define mem_check_block(p)\
  (mem_check_block_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, p))
#define mem_check_all_blocks()\
  (mem_check_all_blocks_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__))
#else /* DEBUG */
#define mem_dump_blocks(fp) ((void)0)
#define mem_check_block(p)  ((void)0)
#define mem_check_all_blocks() ((void)0)
#endif /* DEBUG */
//end public

#endif /* __DARKBLUE_MEMORY_PUBLIC_H__ */





