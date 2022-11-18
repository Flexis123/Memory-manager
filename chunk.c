#include "chunk.h"

#include <stddef.h>
#include <stdint.h>

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
}

void remove_elem(struct chunk_header_list* list, struct chunk_header* ch)
{
    if(ch->previous != NULL) ch->previous->next = ch->next;
    else list->head = ch->next;

    if(ch->next != NULL) ch->next->previous = ch->previous;
    else list->tail = ch->previous;
}
