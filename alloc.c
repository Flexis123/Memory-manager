#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALIGMENT sizeof(long)


struct chunk_header
{
    struct chunk_header* next;
    struct chunk_header* previous;
    size_t size;
    short is_free;
};

struct chunk_header* head = NULL;
struct chunk_header* tail = NULL;


size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    if(unallig != 0) return size + (ALIGMENT - unallig);
    return size;
}

void print_list()
{
    struct chunk_header* ch = head;
    while (ch != NULL)
    {
        printf("%lu ", ch->size);
        ch = ch->next;
    }
    printf("\n");
}

size_t get_size_of_section(struct chunk_header* section_start);

void *mymalloc(size_t size)
{
    if(size == 0) return NULL;
    
    
    struct chunk_header* ch = head;
    size_t aligned_size = allign_size(size);
    while(ch != NULL)
    {
        if(ch->is_free && ch->size >= aligned_size) break; 
        ch = ch->next;
    }


    void* ptr = NULL;
    if(ch == NULL)
    {
        ch = sbrk(sizeof(struct chunk_header));
        ptr = sbrk(aligned_size);

        ch->is_free = 0;
        ch->size = aligned_size;
        ch->next = NULL;

        if(head == NULL)
        {
            ch->previous = NULL;
            head = ch;
            tail = ch;
        }
        else
        {
            tail->next = ch;
            ch->previous = tail;
            tail = ch;
        }
    }
    else
    {
        ptr = ch + 1;

        size_t save_size = ch->size;
        ch->size = aligned_size;
        ch->is_free = 0;

        struct chunk_header* free_part = ptr + ch->size;
        free_part->next = ch->next;
        ch->next = free_part;
        free_part->previous = ch;
        ch->next->previous = free_part;
        free_part->is_free = 1;
        free_part->size = save_size - (sizeof(struct chunk_header) + ch->size);
    }
    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    
}

void myfree(void *ptr)
{
    
    struct chunk_header* ch = head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }

    if(ch != NULL) {
        ch->is_free = 1;
        /*
        if(ch->next != NULL && ch->next->is_free)
        {
            ch->size += ch->next->size;
            ch->next = ch->next->next;
        }

        if(ch->previous != NULL && ch->previous->is_free)
        {
            ch->size += ch->previous->size;
            ch->previous = ch->previous->previous;
        }
        */
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
