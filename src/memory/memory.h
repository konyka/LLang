/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: memory.h
 *      Version: v0.0.0
 *   Created on: 2015-05-01 20:43:54 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-04 22:47:44
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_MEMORY_H__
#define __DARKBLUE_MEMORY_H__

#include "../include/memory.h"

typedef union header_tag header;


struct mem_controller_tag {
    FILE                *error_fp;
    mem_error_handler   error_handler;
    mem_fail_mode       fail_mode;
    header              *block_header;
};

#endif /* __DARKBLUE_MEMORY_H__ */
