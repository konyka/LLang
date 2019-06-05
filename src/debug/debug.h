/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: debug.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 12:57:20 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 13:27:45
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_DEBUG_H__
#define __DARKBLUE_DEBUG_H__

#include <stdio.h>
#include "debug_public.h"

struct debug_controller_tag {
    FILE        *debug_write_fp;
    int         current_debug_level;
};

#endif /* __DARKBLUE_DEBUG_H__ */
