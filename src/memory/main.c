#include "memory_public.h"

static void
dump_buffer(unsigned char *buf, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        printf("%02x ", buf[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    printf("\n");
}

static void
fill_buffer(unsigned char *buf, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        buf[i] = i;
    }
}

int
main(void)
{
    unsigned char *p1;
    unsigned char *p2;
    unsigned char *p3;

    p1 = mem_malloc(10);
    dump_buffer(p1, 10);
    fill_buffer(p1, 10);
    dump_buffer(p1, 10);

    mem_dump_blocks(stdout);

    p2 = mem_malloc(10);
    fill_buffer(p2, 10);
    p2 = mem_realloc(p2, 15);
    dump_buffer(p2, 15);
    mem_dump_blocks(stdout);

    p2 = mem_realloc(p2, 8);
    dump_buffer(p2, 8);
    mem_dump_blocks(stdout);

    p3 = NULL;
    p3 = mem_realloc(p3, 10);
    dump_buffer(p3, 10);
    fill_buffer(p3, 10);
    dump_buffer(p3, 10);
    mem_dump_blocks(stdout);

    mem_free(p2);
    dump_buffer(p2, 8);
    mem_free(p1);
    dump_buffer(p1, 10);
    mem_free(p3);
    dump_buffer(p3, 10);
    fprintf(stderr, "final dump\n");
    mem_dump_blocks(stdout);

    return 0;
}

