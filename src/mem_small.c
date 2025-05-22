/******************************************************
 * Copyright Juan Garcia and Grégory Mounié           *
 *  2018 - 2025                                       *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/


#include <assert.h>
#include <stdbool.h>
#include "mem.h"
#include "mem_internals.h"
#include "linked_list.h"

void initialize_small_chunks_pool();

/// @brief Allocates a small memory block, removing it from the pool.
/// @param size is the size of the allocated block, always CHUNKSIZE
/// @return Gives back the user zone pointer to the allocated memory block
void *
emalloc_small(unsigned long size)
{

    if (is_pool_empty(arena.chunkpool))
    {
        initialize_small_chunks_pool(arena.chunkpool);
    }

    void *chunk = poll(&arena.chunkpool);
    void *user_zone = mark_memarea_and_get_user_ptr(chunk, CHUNKSIZE, SMALL_KIND);

    return (void *)user_zone;
}

/// @brief Receives a freed memory block back from user
/// and add first into the head of the memory pool.
/// @param a is a data struct which contains meta information about
/// the received memory block
void efree_small(Alloc a)
{
    assert(a.size == CHUNKSIZE);

    *(void **)a.ptr = arena.chunkpool;
    arena.chunkpool = a.ptr;
}

/// @brief Initialize a memory pool, transforming it into a linked list
/// of chunks of CHUNKSIZE size. The linked list's head is pointed by arena.chunkpool
void initialize_small_chunks_pool()
{
    unsigned long pool_size_in_bytes = mem_realloc_small();
    new_linked_list(arena.chunkpool, pool_size_in_bytes, CHUNKSIZE);
}

/// @brief Verifies if pool is empty
/// @param pool: Linked list head
/// @return True if there are no chunks left, otherwise returns false.
bool is_pool_empty(void *pool)
{
    return is_linked_list_empty(pool);
}