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


void *mymalloc(size_t size)
{
    if(size == 0) return NULL;
    

    struct chunk_header* ch = head;
    size_t aligned_size = allign_size(size);
    while(ch != NULL)
    {
        if(ch->is_free && ch->size >= aligned_size + sizeof(struct chunk_header)) break;
        ch = ch->next;
    }

    void* ptr = NULL;
    if(ch == NULL)
    {
        ch = sbrk(sizeof(struct chunk_header));
        ptr = sbrk(aligned_size);

        if(head == NULL)
        {
            ch->next = NULL;
            ch->previous = NULL;

            head = ch;
            tail = ch;

            printf("%u\n", head);
            printf("%u\n", tail);
        }
        else
        {
            printf("%u\n", head);
            printf("%u\n", tail);
            tail->next = ch;
            ch->previous = tail;
            tail = ch;
        }
    }
    else
    {
        ptr = ((void*) ch + 1);

        struct chunk_header* free_part = (struct chunk_header*) ptr + aligned_size + sizeof(struct chunk_header);
        free_part->is_free = 1;
        free_part->previous = ch;
        free_part->next = ch->next;
        free_part->size = ch->size - aligned_size;

        ch->next = free_part;
        ch->next->next->previous = free_part;
        ch->size = aligned_size;
        ch->is_free = 0;
    }

    printf("%u\n", head);
    printf("%u\n", tail);

    return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    
}

void myfree(void *ptr)
{
    struct chunk_header* ch = head;
    void* actual_ptr = NULL;
    while(ch != NULL)
    {
        actual_ptr = ((void *) ch + 1);
        if(actual_ptr == ptr) break;
        ch = ch->next;
    }

    if(ch != NULL) {
        ch->is_free = 1;
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
