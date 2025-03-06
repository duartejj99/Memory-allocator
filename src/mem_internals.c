/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include "mem_internals.h"
#include <bits/mman-linux.h>

/// @brief Random number generator based on knuth mmix
/// @param in Memory address used as seed
/// @return Random number
unsigned long knuth_mmix_one_round(unsigned long in)
{
    return in * 6364136223846793005UL % 1442695040888963407UL;
}

/// @brief Marks the memory block pointed by ptr with its meta-information
/// such as its size and memory kind, as well as a magic number to detect
/// memory overflows
/// @param ptr Memory block pointer
/// @param size Memory block size in bytes, it takes in count the meta-information size as well
/// @param k Memory block kind
/// @return
void *mark_memarea_and_get_user_ptr(void *ptr, unsigned long size, MemKind k)
{
    // ->[SIZE][MAGIC NUM]-- [USER MEMORY]--[MAGIC NUM][SIZE]
    //  8o 8o user mem 8o 8o
    unsigned long mmix = knuth_mmix_one_round((unsigned long)ptr);
    unsigned long magic_number = (mmix & ~(0b11UL)) | k;

    *(unsigned long *)ptr = size;
    *((unsigned long *)ptr + 1) = magic_number;
    *(unsigned long *)((char *)ptr + size - 8) = size;
    *(unsigned long *)((char *)ptr + size - 16) = magic_number;

    return (void *)((unsigned long *)ptr + 2);
}

/// @brief Verifies memory block integrity by checking its magic number and size markers,
/// as well as obtaining the block meta-information.
/// @param ptr Memory block pointer referencing the begin of its user zone
/// @return Returns an Alloc struct with the memory block meta-information
Alloc mark_check_and_get_alloc(void *ptr)
{
    void *block_ptr = ptr - 16;

    unsigned long size = *(unsigned long *)block_ptr;
    unsigned long magic_number = *(unsigned long *)(block_ptr + 8);

    unsigned long size_2 = *(unsigned long *)(block_ptr + size - 8);
    unsigned long magic_number_2 = *(unsigned long *)(block_ptr + size - 16);

    assert(size == size_2);
    assert(magic_number == magic_number_2);

    MemKind mem_kind = magic_number & 0b11UL;

    printf("Memory size: %lu\n", *(unsigned long *)block_ptr);
    Alloc a = {
        block_ptr,
        mem_kind,
        size};
    return a;
}

unsigned long
mem_realloc_small()
{
    assert(arena.chunkpool == 0);
    unsigned long size = (FIRST_ALLOC_SMALL << arena.small_next_exponant);
    arena.chunkpool = mmap(0,
                           size,
                           PROT_READ | PROT_WRITE | PROT_EXEC,
                           MAP_PRIVATE | MAP_ANONYMOUS,
                           -1,
                           0);
    if (arena.chunkpool == MAP_FAILED)
        handle_fatalError("small realloc");
    arena.small_next_exponant++;
    return size;
}

unsigned long
mem_realloc_medium()
{
    uint32_t indice = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant;
    assert(arena.TZL[indice] == 0);
    unsigned long size = (FIRST_ALLOC_MEDIUM << arena.medium_next_exponant);
    arena.TZL[indice] = mmap(0,
                             size * 2, // twice the size to allign
                             PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1,
                             0);
    if (arena.TZL[indice] == MAP_FAILED)
        handle_fatalError("medium realloc");
    // align allocation to a multiple of the size
    // for buddy algo
    arena.TZL[indice] += (size - (((intptr_t)arena.TZL[indice]) % size));
    arena.medium_next_exponant++;
    return size; // lie on allocation size, but never free
}

// used for test in buddy algo
unsigned int
nb_TZL_entries()
{
    int nb = 0;

    for (int i = 0; i < TZL_SIZE; i++)
        if (arena.TZL[i])
            nb++;

    return nb;
}

void pointer_info()
{
    printf("DATA TYPE TESTS \n");
    printf("========================\n");

    printf("char size: %li\n", sizeof(char));
    printf("integer size: %li\n", sizeof(int));
    printf("unsigned long integer size: %li\n", sizeof(unsigned long));

    printf("char size pointer:  %li\n", sizeof(char *));
    printf("integer size pointer: %li\n", sizeof(int *));
    printf("unsigned long integer size pointer: %li\n", sizeof(unsigned long *));

    printf("========================\n");
    printf("DATA TYPE TESTS \n");
}
