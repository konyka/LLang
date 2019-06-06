/**
 *===========================================================================
 *  None Source File.
 *  Copyright (C), DarkBlue Studios.
 * -------------------------------------------------------------------------
 *    File name: heap.c
 *      Version: v0.0.0
 *   Created on: 2015-05-05 14:52:47 by konyka
 *  Modified by: konyka
 *Modified time: 2019-06-06 11:20:44
 *       Editor: Sublime Text3
 *        Email: 
 *  Description: 
 * -------------------------------------------------------------------------
 *      History: 
 *
 *===========================================================================
 */
 




#include <stdio.h>
#include <string.h>
#include "memory_public.h"
#include "debug_public.h"
#include "povm_pri.h"

static void
check_gc(povm_virtual_machine_t *dvm)
{
#if 0
    dvm_garbage_collect(dvm);
#endif
    if (dvm->heap.current_heap_size > dvm->heap.current_threshold) {
        /* fprintf(stderr, "garbage collecting..."); */
        dvm_garbage_collect(dvm);
        /* fprintf(stderr, "done.\n"); */

        dvm->heap.current_threshold
            = dvm->heap.current_heap_size + HEAP_THRESHOLD_SIZE;
    }
}

static povm_object_t *
alloc_object(povm_virtual_machine_t *dvm, object_type_enum type)
{
    povm_object_t *ret;

    check_gc(dvm);
    ret = mem_malloc(sizeof(povm_object_t));
    dvm->heap.current_heap_size += sizeof(povm_object_t);
    ret->type = type;
    ret->marked = POVM_FALSE;
    ret->prev = NULL;
    ret->next = dvm->heap.header;
    dvm->heap.header = ret;
    if (ret->next) {
        ret->next->prev = ret;
    }

    return ret;
}

povm_object_t *
dvm_literal_to_dvm_string_i(povm_virtual_machine_t *dvm, povm_char *str)
{
    povm_object_t *ret;

    ret = alloc_object(dvm, STRING_OBJECT);
    ret->u.string.string = str;
    ret->u.string.is_literal = POVM_TRUE;

    return ret;
}

povm_object_t *
dvm_create_dvm_string_i(povm_virtual_machine_t *dvm, povm_char *str)
{
    povm_object_t *ret;

    ret = alloc_object(dvm, STRING_OBJECT);
    ret->u.string.string = str;
    dvm->heap.current_heap_size += sizeof(povm_char) * (dvm_wcslen(str) + 1);
    ret->u.string.is_literal = POVM_FALSE;

    return ret;
}

static void
gc_mark(povm_object_t *obj)
{
    if (obj == NULL)
        return;

    if (obj->marked)
        return;

    obj->marked = POVM_TRUE;
}

static void
gc_reset_mark(povm_object_t *obj)
{
    obj->marked = POVM_FALSE;
}

static void
gc_mark_objects(povm_virtual_machine_t *dvm)
{
    povm_object_t *obj;
    int i;

    for (obj = dvm->heap.header; obj; obj = obj->next) {
        gc_reset_mark(obj);
    }
    
    for (i = 0; i < dvm->static_v.variable_count; i++) {
        if (dvm->executable->global_variable[i].type->basic_type
            == POVM_STRING_TYPE) {
            gc_mark(dvm->static_v.variable[i].object);
        }
    }

    for (i = 0; i < dvm->stack.stack_pointer; i++) {
        if (dvm->stack.pointer_flags[i]) {
            gc_mark(dvm->stack.stack[i].object);
        }
    }
}

static void
gc_dispose_object(povm_virtual_machine_t *dvm, povm_object_t *obj)
{
    switch (obj->type) {
    case STRING_OBJECT:
        if (!obj->u.string.is_literal) {
            dvm->heap.current_heap_size
                -= sizeof(povm_char) * (dvm_wcslen(obj->u.string.string) + 1);
            mem_free(obj->u.string.string);
        }
        break;
    case OBJECT_TYPE_COUNT_PLUS_1:
    default:
        debug_assert(0, ("bad type..%d\n", obj->type));
    }
    dvm->heap.current_heap_size -= sizeof(povm_object_t);
    mem_free(obj);
}

static void
gc_sweep_objects(povm_virtual_machine_t *dvm)
{
    povm_object_t *obj;
    povm_object_t *tmp;

    for (obj = dvm->heap.header; obj; ) {
        if (!obj->marked) {
            if (obj->prev) {
                obj->prev->next = obj->next;
            } else {
                dvm->heap.header = obj->next;
            }
            if (obj->next) {
                obj->next->prev = obj->prev;
            }
            tmp = obj->next;
            gc_dispose_object(dvm, obj);
            obj = tmp;
        } else {
            obj = obj->next;
        }
    }
}

void
dvm_garbage_collect(povm_virtual_machine_t *dvm)
{
    gc_mark_objects(dvm);
    gc_sweep_objects(dvm);
}
