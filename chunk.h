#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>
#include "heap.h"

struct chunk_header
{
    struct chunk_header* next;
    struct chunk_header* previous;
    size_t size;
    char is_free;
};

struct chunk_header_list
{
    struct chunk_header* head;
    struct chunk_header* tail;
    size_t length;
};

void append_elem(struct chunk_header_list* list, struct chunk_header* ch);
void remove_elem(struct chunk_header_list* list, struct chunk_header* ch);
struct chunk_header_list* init_list(void* heap_start);
struct chunk_header* get_right_neighbour(struct chunk_header* ch);
void print_list(struct chunk_header_list* free_list);

#endif