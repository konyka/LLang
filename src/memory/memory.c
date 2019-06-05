/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: memory.c
 *      Version: v0.0.0
 *   Created on: 2015-05-01 20:13:22 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-05 11:51:57
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "memory.h"

static void default_error_handler(mem_controller_tp controller,
                                  char *filename, int line, char *msg);

static struct mem_controller_tag st_default_controller = {
    NULL,/* stderr */
    default_error_handler,
    MEM_FAIL_AND_EXIT
};
mem_controller_tp mem_default_controller = &st_default_controller;

typedef union {
    long        l_dummy;
    double      d_dummy;
    void        *p_dummy;
} align;

#define MARK_SIZE       (4)

typedef struct {
    int         size;
    char        *filename;
    int         line;
    header_t      *prev;
    header_t      *next;
    unsigned char       mark[MARK_SIZE];
} header_tag;

#define ALIGN_SIZE      (sizeof(align))
#define revalue_up_align(val)   ((val) ? (((val) - 1) / ALIGN_SIZE + 1) : 0)
#define HEADER_ALIGN_SIZE       (revalue_up_align(sizeof(header_tag)))
#define MARK (0xCD)

union header_union {
    header_tag       s;
    align               u[HEADER_ALIGN_SIZE];
};

static void
default_error_handler(mem_controller_tp controller,
                      char *filename, int line, char *msg)
{
    fprintf(controller->error_fp,
            "MEM:%s failed in %s at %d\n", msg, filename, line);
}

static void
error_handler(mem_controller_tp controller, char *filename, int line, char *msg)
{
    if (controller->error_fp == NULL) {
        controller->error_fp = stderr;
    }
    controller->error_handler(controller, filename, line, msg);

    if (controller->fail_mode == MEM_FAIL_AND_EXIT) {
        exit(1);
    }
}

mem_controller_tp
mem_create_controller(void)
{
    mem_controller_tp      p;

    p = mem_malloc_func(&st_default_controller, __FILE__, __LINE__,
                        sizeof(struct mem_controller_tag));
    *p = st_default_controller;

    return p;
}

#ifdef DEBUG
static void
chain_block(mem_controller_tp controller, header_t *new_header)
{
    if (controller->block_header) {
        controller->block_header->s.prev = new_header;
    }
    new_header->s.prev = NULL;
    new_header->s.next = controller->block_header;
    controller->block_header = new_header;
}

static void
rechain_block(mem_controller_tp controller, header_t *header_t)
{
    if (header_t->s.prev) {
        header_t->s.prev->s.next = header_t;
    } else {
        controller->block_header = header_t;
    }
    if (header_t->s.next) {
        header_t->s.next->s.prev = header_t;
    }
}

static void
unchain_block(mem_controller_tp controller, header_t *header_t)
{
    if (header_t->s.prev) {
        header_t->s.prev->s.next = header_t->s.next;
    } else {
        controller->block_header = header_t->s.next;
    }
    if (header_t->s.next) {
        header_t->s.next->s.prev = header_t->s.prev;
    }
}

void
set_header(header_t *header_t, int size, char *filename, int line)
{
    header_t->s.size = size;
    header_t->s.filename = filename;
    header_t->s.line = line;
    memset(header_t->s.mark, MARK, (char*)&header_t[1] - (char*)header_t->s.mark);
}

void
set_tail(void *ptr, int alloc_size)
{
    char *tail;
    tail = ((char*)ptr) + alloc_size - MARK_SIZE;
    memset(tail, MARK, MARK_SIZE);
}

void
check_mark_sub(unsigned char *mark, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (mark[i] != MARK) {
            fprintf(stderr, "bad mark\n");
            abort();
        }
    }
}

void
check_mark(header_t *header_t)
{
    unsigned char       *tail;
    check_mark_sub(header_t->s.mark, (char*)&header_t[1] - (char*)header_t->s.mark);
    tail = ((unsigned char*)header_t) + header_t->s.size + sizeof(header_t);
    check_mark_sub(tail, MARK_SIZE);
}
#endif /* DEBUG */

void*
mem_malloc_func(mem_controller_tp controller, char *filename, int line,
                size_t size)
{
    void        *ptr;
    size_t      alloc_size;

#ifdef DEBUG
    alloc_size = size + sizeof(header_t) + MARK_SIZE;
#else
    alloc_size = size;
#endif
    ptr = malloc(alloc_size);
    if (ptr == NULL) {
        error_handler(controller, filename, line, "malloc");
    }

#ifdef DEBUG
    memset(ptr, 0xCC, alloc_size);
    set_header(ptr, size, filename, line);
    set_tail(ptr, alloc_size);
    chain_block(controller, (header_t*)ptr);
    ptr = (char*)ptr + sizeof(header_t);
#endif

    return ptr;
}

void*
mem_realloc_func(mem_controller_tp controller, char *filename, int line,
                 void *ptr, size_t size)
{
    void        *new_ptr;
    size_t      alloc_size;
    void        *real_ptr;
#ifdef DEBUG
    header_t      old_header;
    int         old_size;

    alloc_size = size + sizeof(header_t) + MARK_SIZE;
    if (ptr != NULL) {
        real_ptr = (char*)ptr - sizeof(header_t);
        check_mark((header_t*)real_ptr);
        old_header = *((header_t*)real_ptr);
        old_size = old_header.s.size;
        unchain_block(controller, real_ptr);
    } else {
        real_ptr = NULL;
        old_size = 0;
    }
#else
    alloc_size = size;
    real_ptr = ptr;
#endif

    new_ptr = realloc(real_ptr, alloc_size);
    if (new_ptr == NULL) {
        if (ptr == NULL) {
            error_handler(controller, filename, line, "realloc(malloc)");
        } else {
            error_handler(controller, filename, line, "realloc");
            free(real_ptr);
        }
    }

#ifdef DEBUG
    if (ptr) {
        *((header_t*)new_ptr) = old_header;
        ((header_t*)new_ptr)->s.size = size;
        rechain_block(controller, (header_t*)new_ptr);
        set_tail(new_ptr, alloc_size);
    } else {
        set_header(new_ptr, size, filename, line);
        set_tail(new_ptr, alloc_size);
        chain_block(controller, (header_t*)new_ptr);
   }
    new_ptr = (char*)new_ptr + sizeof(header_t);
    if (size > old_size) {
        memset((char*)new_ptr + old_size, 0xCC, size - old_size);
    }
#endif

    return(new_ptr);
}

char *
MEM_strdup_func(mem_controller_tp controller, char *filename, int line,
                char *str)
{
    char        *ptr;
    int         size;
    size_t      alloc_size;

    size = strlen(str) + 1;
#ifdef DEBUG
    alloc_size = size + sizeof(header_t) + MARK_SIZE;
#else
    alloc_size = size;
#endif
    ptr = malloc(alloc_size);
    if (ptr == NULL) {
        error_handler(controller, filename, line, "strdup");
    }

#ifdef DEBUG
    memset(ptr, 0xCC, alloc_size);
    set_header((header_t*)ptr, size, filename, line);
    set_tail(ptr, alloc_size);
    chain_block(controller, (header_t*)ptr);
    ptr = (char*)ptr + sizeof(header_t);
#endif
    strcpy(ptr, str);

    return(ptr);
}

void
mem_free_func(mem_controller_tp controller, void *ptr)
{
    void        *real_ptr;
#ifdef DEBUG
    int size;
#endif
    if (ptr == NULL)
        return;

#ifdef DEBUG
    real_ptr = (char*)ptr - sizeof(header_t);
    check_mark((header_t*)real_ptr);
    size = ((header_t*)real_ptr)->s.size;
    unchain_block(controller, real_ptr);
    memset(real_ptr, 0xCC, size + sizeof(header_t));
#else
    real_ptr = ptr;
#endif

    free(real_ptr);
}

void
mem_set_error_handler(mem_controller_tp controller, mem_error_handler handler)
{
    controller->error_handler = handler;
}

void
mem_set_fail_mode(mem_controller_tp controller, mem_fail_mode_enum mode)
{
    controller->fail_mode = mode;
}

void
mem_dump_blocks_func(mem_controller_tp controller, FILE *fp)
{
#ifdef DEBUG
    header_t *pos;
    int counter = 0;
    int i;

    for (pos = controller->block_header; pos; pos = pos->s.next) {
        check_mark(pos);
        fprintf(fp, "[%04d]%p********************\n", counter,
                (char*)pos + sizeof(header_t));
        fprintf(fp, "%s line %d size..%d\n",
                pos->s.filename, pos->s.line, pos->s.size);
        fprintf(fp, "[");
        for (i = 0; i < pos->s.size; i++) {
            if (isprint(*((char*)pos + sizeof(header_t)+i))) {
                fprintf(fp, "%c", *((char*)pos + sizeof(header_t)+i));
            } else {
                fprintf(fp, ".");
            }
        }
        fprintf(fp, "]\n");
        counter++;
    }
#endif /* DEBUG */
}

void
mem_check_block_func(mem_controller_tp controller, char *filename, int line,
                     void *p)
{
#ifdef DEBUG
    void *real_ptr = ((char*)p) - sizeof(header_t);

    check_mark(real_ptr);
#endif /* DEBUG */
}

void mem_check_all_blocks_func(mem_controller_tp controller,
                               char *filename, int line)
{
#ifdef DEBUG
    header_t *pos;

    for (pos = controller->block_header; pos; pos = pos->s.next) {
        check_mark(pos);
    }
#endif /* DEBUG */
}
