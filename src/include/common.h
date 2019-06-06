/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: common.h
 *      Version: v0.0.0
 *   Created on: 2015-05-05 15:09:08 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 19:58:08
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 

#ifndef __DARKBLUE_COMMON_H__
#define __DARKBLUE_COMMON_H__


#include "povm_code.h"

typedef struct {
    char        *mnemonic;
    char        *parameter;
    int         stack_increment;
} OpcodeInfo;

/* dispose.c */
void dvm_dispose_executable(povm_executable_t *exe);
/* wchar.c */
size_t dvm_wcslen(wchar_t *str);
wchar_t *dvm_wcscpy(wchar_t *dest, wchar_t *src);
wchar_t *dvm_wcsncpy(wchar_t *dest, wchar_t *src, size_t n);
int dvm_wcscmp(wchar_t *s1, wchar_t *s2);
wchar_t *dvm_wcscat(wchar_t *s1, wchar_t *s2);
int dvm_mbstowcs_len(const char *src);
void dvm_mbstowcs(const char *src, wchar_t *dest);
int dvm_wcstombs_len(const wchar_t *src);
void dvm_wcstombs(const wchar_t *src, char *dest);
char *dvm_wcstombs_alloc(const wchar_t *src);
char dvm_wctochar(wchar_t src);
int dvm_print_wcs(FILE *fp, wchar_t *str);
int dvm_print_wcs_ln(FILE *fp, wchar_t *str);
povm_boolean_enum dvm_iswdigit(wchar_t ch);
/* disassemble.c */
void dvm_disassemble(povm_executable_t *exe);





#endif /* __DARKBLUE_COMMON_H__ */
