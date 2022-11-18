#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALIGMENT sizeof(long)


struct chunk_header
{
    struct chunk_header* next;
    struct chunk_header* previous;
    struct chunk_header* next_free;
    struct chunk_header* previous_free;
    size_t size;
    short is_free;
};

struct chunk_header* head = NULL;
struct chunk_header* tail = NULL;

struct chunk_header* free_head = NULL;
struct chunk_header* free_tail = NULL;

void print_list(struct chunk_header* list_head)
{
    struct chunk_header* ch = list_head;
    while (ch != NULL)
    {
        printf("%lu ", ch->size);
        ch = ch->next;
    }
    printf("\n");
}

size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    if(unallig != 0) return size + (ALIGMENT - unallig);
    return size;
}

void add_free_header(struct chunk_header* ch)
{
    ch->next_free = NULL;
    if(free_head == NULL)
    {
        ch->previous_free = NULL;
        free_head = ch;
        free_tail = ch;
    }
    else
    {
        free_tail->next_free = ch;
        ch->previous_free = free_tail;
        free_tail = ch;
    }
}

void remove_free_header(struct chunk_header* ch){
    if(ch->previous_free != NULL) ch->previous_free->next_free = ch->next_free;
    else free_head = ch->next_free;

    if(ch->next_free != NULL) ch->next_free->previous_free = ch->previous_free;
    else free_tail = ch->previous_free;
}

void *mymalloc(size_t size)
{
    if(size == 0) return NULL;
    
    
    struct chunk_header* ch = free_head;
    size_t aligned_size = allign_size(size);

    while(ch != NULL)
    {
        if(ch->size >= aligned_size) break; 
        ch = ch->next_free;
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

        size_t free_part_size = ch->size - aligned_size;
        ch->is_free = 0;
        ch->size = aligned_size;

        struct chunk_header* free_part = ptr + ch->size;
        if(free_part_size < sizeof(struct chunk_header))
        {
            ch->size += free_part_size;
        }
        else
        {
            free_part->next = ch->next;

            if(ch->next != NULL) ch->next->previous = free_part;
            else tail = free_part;

            ch->next = free_part;
            free_part->previous = ch;
            free_part->is_free = 1;
            free_part->size = free_part_size - sizeof(struct chunk_header);

            add_free_header(free_part);
        }

        remove_free_header(ch);
    }

    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    size_t aligned_size = allign_size(nmemb * size);
    if(aligned_size == 0) return NULL;
    if(aligned_size < nmemb || aligned_size < size) return NULL;

    void* ptr = mymalloc(aligned_size);
    bzero(ptr, aligned_size);
    return ptr;
}

void myfree(void *ptr)
{
    
    struct chunk_header* ch = head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }
    print_list(free_head);
    print_list(head);

    if(ch != NULL) {
        ch->is_free = 1;

        if(ch->next != NULL && ch->next->is_free)
        {
            ch->size += ch->next->size;
            
            remove_free_header(ch->next);
            
            ch->next = ch->next->next;
        }

        if(ch->previous != NULL && ch->previous->is_free)
        {
            ch->size += ch->previous->size;
            remove_free_header(ch->previous);
            ch->previous= ch->previous->previous;
        }

        add_free_header(ch);
    }

    print_list(free_head);
    print_list(head);
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
