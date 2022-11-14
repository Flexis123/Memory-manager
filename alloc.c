#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define ALIGMENT sizeof(long)

struct chunk_header{
    struct chunk_header* next;
    struct chunk_header* previous;
    size_t size;
    short is_free;
};


size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    return size + (ALIGMENT - unallig);
}

struct chunk_header* heap_head = NULL;
struct chunk_header* heap_tail = NULL;

void *mymalloc(size_t size)
{
    struct chunk_header* ch = sbrk(sizeof(struct chunk_header));
    void* ptr = sbrk(allign_size(size));
    
    ch->is_free = 0;
    ch->size = allign_size(size);

    if(heap_head == NULL) 
    {
        heap_head = ch;
        heap_tail = ch;
        ch->previous = NULL;
        ch->next = NULL;
    }else{
        ch->previous = heap_tail;
        heap_tail->next = ch;
        heap_tail = ch;
    }

    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    return NULL;
}

void myfree(void *ptr)
{
    struct chunk_header* free_ch = heap_head;
    while(free_ch != NULL){
        if(free_ch + sizeof(struct chunk_header) == ptr){
            break;
        }
        free_ch = free_ch->next;
    }
    free_ch->is_free = 1;
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
