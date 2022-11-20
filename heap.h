#ifndef HEAP_H
#define HEAP_H

#include "chunk.h"

struct heap_t
{
    void* heap_end;
    struct chunk_header_list* heap;
    size_t length;
    size_t size;
};

struct heap_t* init_heap(size_t size);
void* inc_heap_end(struct heap_t* hp, size_t offset);


#endif