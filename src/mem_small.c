/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include <stdbool.h>
#include "mem.h"
#include "mem_internals.h"

bool is_pool_empty(void *pool);
void initialize_pool();
void *poll(void *head);

void *
emalloc_small(unsigned long size)
{

    if (is_pool_empty(arena.chunkpool))
    {
        initialize_pool(arena.chunkpool);
    }

    void *user_zone = poll(arena.chunkpool);

    return (void *)user_zone;
}

void *poll(void *head)
{
    void *user_zone;
    void *next_element = *(void **)head;
    user_zone = mark_memarea_and_get_user_ptr(head, CHUNKSIZE, SMALL_KIND);
    head = next_element;

    return user_zone;
}

void efree_small(Alloc a)
{
    // verify it is CHUNK_SIZE and SMALL ALLOC

    // add block to the linked list
    assert("Not yet implemented :(");
}

/// @brief Initialize a memory pool, transforming it into a linked list
/// which head pointed by the arena.chunkpool pointer
void initialize_pool()
{

    unsigned long pool_size_in_bytes = mem_realloc_small();
    assert(pool_size_in_bytes % CHUNKSIZE == 0);

    int number_of_blocks = pool_size_in_bytes / CHUNKSIZE;
    void *block_pointer = arena.chunkpool;
    for (int block_index = 0; block_index < number_of_blocks; block_index++)
    {
        *(void **)block_pointer = (char *)block_pointer + 96;
        block_pointer = (char *)block_pointer + 96;
    }
}

/// @brief Verifies if pool header is null
/// @param pool: Linked list head
/// @return
bool is_pool_empty(void *pool)
{
    return *(void **)pool == 0;
}