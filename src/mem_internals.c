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

unsigned long knuth_mmix_one_round(unsigned long in)
{
    return in * 6364136223846793005UL % 1442695040888963407UL;
}

// unsigned long calculate_magic_number(unsigned long )

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

Alloc mark_check_and_get_alloc(void *ptr)
{
    /* ecrire votre code ici */
    Alloc a = {};
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
