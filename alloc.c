#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <chunk.h>

#define ALIGMENT sizeof(long)

struct chunk_header_list heap;
struct chunk_header_list free_list;


size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    if(unallig != 0) return size + (ALIGMENT - unallig);
    return size;
}


void *mymalloc(size_t size)
{
    if(size == 0) return NULL;

    struct chunk_header* ch;
    void* ptr;

    ch = free_list.head;
    size_t aligned_size = allign_size(size);
    while(ch != NULL)
    {
        if(ch->size >= aligned_size + sizeof(struct chunk_header)) break;
    }

    if(ch == NULL)
    {
        ch = sbrk(sizeof(struct chunk_header));
        ptr = sbrk(aligned_size);
        add_elem(&heap, ch);
    }
    else
    {
        ptr = ((void*) (ch + sizeof(struct chunk_header)));

        remove_elem(&free_list, ch);
        
        struct chunk_header* free_part = ch + sizeof(struct chunk_header) + aligned_size;
        free_part->is_free = 1;
        free_part->previous = ch;
        free_part->next = ch->next;
        free_part = ch->size - aligned_size;

        ch->next = free_part;
        ch->is_free = 0;
        ch->size = aligned_size;

        add_elem(&free_list, free_part);
    }

    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    size_t alligned_size = nmemb * size;
    if(alligned_size < nmemb || alligned_size < size || alligned_size == 0) return NULL;

    void* ptr = malloc(alligned_size);
    return memset(ptr, 0, allign_size(alligned_size));
}

void myfree(void *ptr)
{
    if(ptr == NULL) return;

    struct chunk_header* free_ch = get_elem(&heap, ptr - sizeof(struct chunk_header));
    if(free_ch != NULL)
    {
        free_ch->is_free = 1;

        if(free_ch->next != NULL && free_ch->next->is_free)
        {
            free_ch->size += free_ch->next->size;
            free_ch->next = free_ch->next->next;
        }

        if(free_ch->previous != NULL && free_ch->previous->is_free)
        {
            free_ch->size += free_ch->previous->size;
            free_ch->next = free_ch->previous->previous;
        }
        add_elem(&free_list, free_ch);
    }
}

void *myrealloc(void *ptr, size_t size)
{
    return NULL;
}


/*
 * Enable the code below to enable system allocator support for your allocator.
 * Doing so will make debugging much harder (e.g., using printf may result in
 * infinite loops).
 */
#if 0
void *malloc(size_t size) { return mymalloc(size); }
void *calloc(size_t nmemb, size_t size) { return mycalloc(nmemb, size); }
void *realloc(void *ptr, size_t size) { return myrealloc(ptr, size); }
void free(void *ptr) { myfree(ptr); }
#endif
