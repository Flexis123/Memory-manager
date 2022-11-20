#include "heap.h"
#include <unistd.h>
#include <stdio.h>


void* inc_heap_end(struct heap_t* hp, size_t offset)
{
    void* old_heap_end = hp->heap_end;

    hp->length += offset;
    if(hp->length > hp->size)
    {
        size_t size_inc = offset + PAGE_SIZE;
        if(sbrk(size_inc) != NULL)
        {
            hp->size += size_inc;
        }
    }
    hp->heap_end += offset;

    return old_heap_end;
}

struct heap_t* init_heap()
{
    struct heap_t* hp = sbrk(sizeof(struct heap_t));
    hp->length = 0;
    hp->size = PAGE_SIZE;
    hp->heap_end = sbrk(PAGE_SIZE);

    hp->heap = init_list(hp);

    return hp;
}
