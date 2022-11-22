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
    list->length = 0;
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

struct chunk_header* get_right_neighbour(struct chunk_header* ch, void* heap_end)
{
    void* neighbour = ((void*)(ch + 1)) + ch->size;
    if(neighbour >= heap_end) {
        struct chunk_header* heap_end_ch = heap_end;
        heap_end_ch->is_free = 0;
        heap_end_ch->size = 0;
        heap_end_ch->next = NULL;
        heap_end_ch->previous = NULL;
        return heap_end_ch;
    }
    return neighbour;
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
