#include "chunk.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>


struct chunk_header_list* init_list(void* heap_start)
{
    struct chunk_header_list* list = inc_heap_end(heap_start, sizeof(struct chunk_header_list));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void append_elem(struct chunk_header_list* list, struct chunk_header* ch)
{
    ch->next = NULL;
    if(list->head == NULL)
    {
        ch->previous = NULL;
        list->head = ch;
        list->tail = ch;
    }
    else
    {
        list->tail->next = ch;
        ch->previous = list->tail;
        list->tail = ch;
    }

    list->length++;
}

void remove_elem(struct chunk_header_list* list, struct chunk_header* ch)
{
    if(ch->previous != NULL) ch->previous->next = ch->next;
    else list->head = ch->next;

    if(ch->next != NULL) ch->next->previous = ch->previous;
    else list->tail = ch->previous;

    list->length--;
}

struct chunk_header* get_right_neighbour(struct chunk_header* ch)
{
    return ((void*)(ch + 1)) + ch->size;
}

void print_list(struct chunk_header_list* free_list)
{
    struct chunk_header* ch = free_list->head;
    while (ch != NULL)
    {
        printf("%lu ", ch->size);
        ch = ch->next;
    }
    printf("\n");
}
