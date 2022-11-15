#include <chunk.h>
#include <stddef.h>

void add_elem(struct chunk_header_list* list, struct chunk_header* ch)
{
    if(list->head == NULL) 
    {
        list->head = ch;
        list->tail = ch;
        ch->previous = NULL;
        ch->next = NULL;
    }
    else
    {
        ch->previous = list->tail;
        list->tail->next = ch;
        list->tail = ch;
    }
}

struct chunk_header *get_elem(struct chunk_header_list* list, struct chunk_header* ch){
    struct chunk_header* elem = list->head;
    while(elem != NULL)
    {
        if(elem == ch) break;
        elem = elem->next;
    }
    return elem;
}

void remove_elem(struct chunk_header_list* list, struct chunk_header* ch)
{
    struct chunk_header* elem = get_elem(list, ch);
    if(elem != NULL)
    {
        struct chunk_header* prev = elem->previous;
        struct chunk_header* next = elem->next;

        if(elem == list->tail) 
        {
            list->tail = prev;
            list->tail->next = NULL;
        }
        else if (elem == list->head)
        {
            list->head = next;
            list->head->previous = NULL;
        }
        else
        {
            prev->next = next;
            next->previous = prev;
        }
    }
}