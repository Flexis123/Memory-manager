#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALIGMENT sizeof(long)
#define MERGE_FACTOR 20

struct chunk_header
{
    struct chunk_header* next;
    struct chunk_header* previous;
    size_t size;
    int is_free;
};

struct chunk_header_list
{
    struct chunk_header* head;
    struct chunk_header* tail;
    size_t length;
};

struct chunk_header_list* init_list()
{
    struct chunk_header_list* list = sbrk(sizeof(struct chunk_header_list));
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


struct chunk_header_list* heap = NULL;
struct chunk_header_list* free_list = NULL;


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

size_t allign_size(size_t size)
{
    size_t unallig = size % ALIGMENT;
    if(unallig != 0) return size + (ALIGMENT - unallig);
    return size;
}

struct chunk_header* get_right_neighbour(struct chunk_header* ch)
{
    return ((void*)(ch + 1)) + ch->size;
}


void split_chunks(void* ptr, struct chunk_header*  ch, size_t aligned_size)
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
    append_elem(heap, ch);
}


void *mymalloc(size_t size)
{
    if(size == 0) return NULL;
    
    if(free_list == NULL || heap == NULL)
    {
        heap = init_list();
        free_list = init_list();
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
        ch = sbrk(sizeof(struct chunk_header));
        ptr = sbrk(aligned_size);

        ch->size = aligned_size;
        ch->is_free = 0;
        append_elem(heap, ch);
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
    struct chunk_header* ch = heap->head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }

    if(ch != NULL) 
    {
        ch->is_free = 1;
        remove_elem(heap, ch);
        append_elem(free_list, ch);
        
        if(free_list->length % MERGE_FACTOR)
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

    struct chunk_header* ch = heap->head;
    while(ch != NULL)
    {
        if(ch + 1 == ptr) break;
        ch = ch->next;
    }

    struct chunk_header* ch_next = get_right_neighbour(ch);
    size_t extra_size = aligned_size - ch->size;

    size_t free_part_size = (ch_next->size + sizeof(struct chunk_header));
    size_t new_free_part_size = free_part_size - extra_size;

    if(ch_next->is_free && aligned_size > ch->size && free_part_size >= extra_size)
    {
        struct chunk_header* free_part;
        return ptr;
    }
    else
    {
        void* new_ptr = mymalloc(aligned_size);
        if(new_ptr != NULL)
        {
            size_t cpy_size = ch->size < aligned_size ? ch->size : aligned_size;
            memcpy(new_ptr, ptr, cpy_size);
            myfree(ptr);
        }
        return new_ptr;
    }
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
