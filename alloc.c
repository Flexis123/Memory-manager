#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "chunk.h"
#include "heap.h"
#include <math.h>

#define ALIGMENT sizeof(long)
#define MERGE_FACTOR 20


size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    if(unallig != 0) return size + (ALIGMENT - unallig);
    return size;
}


struct heap_t* heap = NULL;
struct chunk_header_list* free_list = NULL;

void split_chunks(void* ptr, struct chunk_header* ch, size_t aligned_size)
{
    size_t free_part_size = ch->size - aligned_size;
    ch->size = aligned_size;
    ch->is_free = 0;

    struct chunk_header* free_part = ptr + ch->size;
    if(free_part_size < sizeof(struct chunk_header))
    {
        ch->size += free_part_size;
    }
    else
    {
        free_part->size = free_part_size - sizeof(struct chunk_header);
        free_part->is_free = 1;
        append_elem(free_list, free_part);
    }

    remove_elem(free_list, ch);
    append_elem(heap->heap, ch);
}


void *mymalloc(size_t size)
{
    if(size == 0) return heap->heap_end;
    
    if(free_list == NULL || heap == NULL)
    {
        heap = init_heap();
        free_list = init_list(heap);
    }
    
    struct chunk_header* ch = free_list->head;
    size_t aligned_size = allign_size(size);

    while(ch != NULL)
    {
        if(ch->size >= aligned_size) break; 
        ch = ch->next;
    }

    void* ptr = NULL;
    if(ch == NULL)
    {
        ch = inc_heap_end(heap, sizeof(struct chunk_header));
        ptr = inc_heap_end(heap, aligned_size);

        ch->size = aligned_size;
        ch->is_free = 0;
        append_elem(heap->heap, ch);
    }
    else
    {
        ptr = ch + 1;
        split_chunks(ptr, ch, aligned_size);
    }

    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    size_t aligned_size = allign_size(nmemb * size);
    if(aligned_size == 0 || aligned_size < nmemb || aligned_size < size) return NULL;

    void* ptr = mymalloc(aligned_size);
    bzero(ptr, aligned_size);
    return ptr;
}

void myfree(void *ptr)
{
    struct chunk_header* ch = heap->heap->head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }

    if(ch != NULL) 
    {
        ch->is_free = 1;
        remove_elem(heap->heap, ch);
        append_elem(free_list, ch);

        if(free_list->length % MERGE_FACTOR == 0)
        {
            struct chunk_header* free_ch = free_list->head;
            while(free_ch != NULL)
            {
                struct chunk_header* ch_next = get_right_neighbour(free_ch);
                if(ch_next->is_free == 1)
                {
                    free_ch->size += ch_next->size + sizeof(struct chunk_header);
                    remove_elem(free_list, ch_next);
                }
                free_ch = free_ch->next;
            }
        }
    }
}

void *myrealloc(void *ptr, size_t size)
{
    size_t aligned_size = allign_size(size);

    if(ptr == NULL && size == 0) return NULL;
    else if(ptr == NULL) return mymalloc(aligned_size);
    else if(size == 0) 
    {
        myfree(ptr);
        return ptr;
    }

    struct chunk_header* ch = heap->heap->head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }

    struct chunk_header* free_part = get_right_neighbour(ch);
    size_t extra_size = aligned_size - ch->size;

    size_t free_part_size = (free_part->size + sizeof(struct chunk_header));
    size_t new_free_part_size = free_part_size - extra_size;
    
    if(free_part->is_free && aligned_size > ch->size && free_part_size >= extra_size)
    {
        remove_elem(free_list, free_part);
        ch->size += extra_size;
        ch->is_free = 0;

        if(new_free_part_size <= sizeof(struct chunk_header)) ch->size += new_free_part_size;
        else
        {
            struct chunk_header* new_free_part = get_right_neighbour(ch);
            new_free_part->size = new_free_part_size - sizeof(struct chunk_header);
            new_free_part->is_free = 1;
            append_elem(free_list, new_free_part);
        }
    }
    else if(aligned_size < ch->size) ;
    else if(aligned_size > ch->size)
    {
        void* new_ptr = mymalloc(aligned_size);
        if(new_ptr != heap->heap_end)
        {
            size_t cpy_size = ch->size < aligned_size ? ch->size : aligned_size;
            memcpy(new_ptr, ptr, cpy_size);
            myfree(ptr);
        }
        return new_ptr;
    }


    return ptr;
}


/*
 * Enable the code below to enable system allocator support for your allocator.
 * Doing so will make debugging much harder (e.g., using printf may result in
 * infinite loops).
 */
#if 1
void *malloc(size_t size) { return mymalloc(size); }
void *calloc(size_t nmemb, size_t size) { return mycalloc(nmemb, size); }
void *realloc(void *ptr, size_t size) { return myrealloc(ptr, size); }
void free(void *ptr) { myfree(ptr); }
#endif
