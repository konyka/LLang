/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: storage.c
 *      Version: v0.0.0
 *   Created on: 2015-05-01 20:13:22 by konyka
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
 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "memory.h"

typedef union {
    long        l_dummy;
    double      d_dummy;
    void        *p_dummy;
} cell_union;
typedef cell_union cell_t;

#define CELL_SIZE               (sizeof(cell_union))
#define DEFAULT_PAGE_SIZE       (1024)  /* cell num */

typedef struct memory_page_tag memory_page_t;
typedef memory_page_t *memory_page_list_tp;

struct memory_page_tag {
    int                 cell_num;
    int                 use_cell_num;
    memory_page_list_tp    next;
    cell_union            cell[1];
};

struct mem_storage_tag {
    memory_page_list_tp      page_list;
    int                 current_page_size;
};

#define larger(a, b) (((a) > (b)) ? (a) : (b))

mem_storage_tp
mem_open_storage_func(mem_controller_tp controller,
                      char *filename, int line, int page_size)
{
    mem_storage_tp storage;

    storage = mem_malloc_func(controller, filename, line,
                              sizeof(struct mem_storage_tag));
    storage->page_list = NULL;
    assert(page_size >= 0);
    if (page_size > 0) {
        storage->current_page_size = page_size;
    } else {
        storage->current_page_size = DEFAULT_PAGE_SIZE;
    }

    return storage;
}

void*
mem_storage_malloc_func(mem_controller_tp controller,
                        char *filename, int line, mem_storage_tp storage,
                        size_t size)
{
    int                 cell_num;
    memory_page_t          *new_page;
    void                *p;

    cell_num = ((size - 1) / CELL_SIZE) + 1;

    if (storage->page_list != NULL
        && (storage->page_list->use_cell_num + cell_num
            < storage->page_list->cell_num)) {
        p = &(storage->page_list->cell[storage->page_list->use_cell_num]);
        storage->page_list->use_cell_num += cell_num;
    } else {
        int     alloc_cell_num;

        alloc_cell_num = larger(cell_num, storage->current_page_size);

        new_page = mem_malloc_func(controller, filename, line,
                                   sizeof(memory_page_t)
                                   + CELL_SIZE * (alloc_cell_num - 1));
        new_page->next = storage->page_list;
        new_page->cell_num = alloc_cell_num;
        storage->page_list = new_page;

        p = &(new_page->cell[0]);
        new_page->use_cell_num = cell_num;
    }

    return p;
}

void
mem_dispose_storage_func(mem_controller_tp controller, mem_storage_tp storage)
{
    memory_page_t  *temp;

    while (storage->page_list) {
        temp = storage->page_list->next;
        mem_free_func(controller, storage->page_list);
        storage->page_list = temp;
    }
    mem_free_func(controller, storage);
}
