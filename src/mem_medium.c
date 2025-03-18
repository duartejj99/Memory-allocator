/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "mem.h"
#include "mem_internals.h"
#include "linked_list.h"

struct MemoryBlock
{
    void *ptr;
    unsigned long size;
};
const struct MemoryBlock NULLMEMORYBLOCK = {NULL, 0};

bool bigger_pool_exists(unsigned int pool_index);
void fragment_block(struct MemoryBlock block, unsigned long fragment_size);
struct MemoryBlock find_next_bigger_block(unsigned int pool_index);

unsigned int puiss2(unsigned long size)
{
    unsigned int p = 0;
    size = size - 1; // allocation start in 0
    while (size)
    { // get the largest bit
        p++;
        size >>= 1;
    }
    if (size > (1 << p)) // WHEN THIS IS TRUE?????? SIZE is always 0 at the end of loop,
        p++;
    return p;
}

/// @brief Allocates a medium memory block, removing it from a pool on the Free zone array
/// (TZL on original french).
/// @param size is the size of the demanded block.
/// @warning Medium allocations allocates a block length equals to the closest power of two bigger than size.
/// @return Gives back the user zone pointer to the allocated memory block
void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);

    unsigned int pool_index = puiss2(size);
    void *allocated_block;

    if (is_pool_empty(arena.TZL[pool_index]))
    {
        struct MemoryBlock block = find_next_bigger_block(pool_index);

        if (block.ptr == NULL)
        {
            block.size = mem_realloc_medium();
            block.ptr = arena.TZL[FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant - 1];
        }
        unsigned long fragment_size = 1 << pool_index;
        fragment_block(block, fragment_size);
    }

    allocated_block = poll(&arena.TZL[pool_index]);
    void *user_ptr = mark_memarea_and_get_user_ptr(allocated_block, 1 << pool_index, MEDIUM_KIND);

    return user_ptr;
}
/// @brief Realizes a binary fragmentation on a memory block until obtaining a memory chunk of size fragment_size
/// @param block the pointer of the memory block to fragment
/// @param fragment_size the memory chunk length
void fragment_block(struct MemoryBlock block, unsigned long fragment_size)
{
    unsigned int block_to_cut_index = puiss2(block.size);
    unsigned long block_size = block.size;
    unsigned long binary_fragment_size;

    assert(block.ptr != NULL);
    assert((block_size % fragment_size) == 0); // THIS IS ALWAYS TRUE, should this function receives block_size?

    unsigned int index = puiss2(fragment_size);
    void *head = &arena.TZL[block_to_cut_index];

    while (arena.TZL[index] == NULL && index < block_to_cut_index)
    {
        block_size = 1 << block_to_cut_index;
        binary_fragment_size = block_size >> 1;

        arena.TZL[block_to_cut_index - 1] = poll(head);
        block_to_cut_index--;
        new_linked_list(arena.TZL[block_to_cut_index], block_size, binary_fragment_size);
        head = &arena.TZL[block_to_cut_index];
    }
}

/// @brief Finds the next memory block, bigger than size, available on the array of free zone pools.
///
/// It iterates the array from the pool of the memory blocks with the minimum required size
/// until finding a pool with an available block
/// @param size The size of the memory block searched
/// @return Returns the index of the next available pool, if any.
/// @warning Returns NULLMEMORYBLOCK if there are no bigger blocks available on any pool
struct MemoryBlock find_next_bigger_block(unsigned int size)
{
    unsigned int pool_index = puiss2(size);

    struct MemoryBlock pool = NULLMEMORYBLOCK;
    while (bigger_pool_exists(pool_index))
    {
        if (!is_pool_empty(arena.TZL[pool_index]))
        {
            pool.ptr = arena.TZL[pool_index];
            pool.size = 1 << pool_index;

            return pool;
        }
        pool_index++;
    }

    return pool;
}

/// @brief Verifies if there still are bigger pools on the Free zone array than
/// a reference pool signalated by index pool_index
/// @param pool_index is the reference pool index
/// @return True if there are still bigger pools to check after the reference pool
bool bigger_pool_exists(unsigned int pool_index)
{
    return pool_index < FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant;
}

void efree_medium(Alloc a)
{

    // 1.Buddy check
    // 2. IF body is found, fusion, else go back to the linked list (PUSH)
    // 3. IF fusion result has a buddy goto Step 1.
}
