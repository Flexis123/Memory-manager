#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>

struct chunk_header
{
    struct chunk_header* next;
    struct chunk_header* previous;
    size_t size;
};

struct chunk_header_list
{
    struct chunk_header* head;
    struct chunk_header* tail;
};

void append_elem(struct chunk_header_list* list, struct chunk_header* ch);
void remove_elem(struct chunk_header_list* list, struct chunk_header* ch);

#endif