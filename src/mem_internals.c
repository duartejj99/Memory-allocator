/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include "mem.h"
#include "mem_internals.h"

unsigned long knuth_mmix_one_round(unsigned long in)
{
    return in * 6364136223846793005UL % 1442695040888963407UL;
}

const unsigned int SIZE_MARKER_LENGTH = 8;
const unsigned int MAGIC_NUMBER_LENGTH = 8;

/*Size ==n° d'octets +32 (summé déjà dans mem_large.c)
     *ptr= pointeur passé par l'utilisateur
      MemKind k= 0,1,2 == type d'alloc
      return= pointeur du bloc marqué */
/* ecrire votre code ici */

/*
** ptr: Points to the WHOLE memory block beginning (including marks)
** size: Memory block size expressed on number of bytes:
        Required memory + Unused + 4 * 8bytes (2 size marks, 2 magic number, each one 1 byte long)
** k: Large, Medium or Small memory slot kind.
*/
void *mark_memarea_and_get_user_ptr(void *ptr, unsigned long size, MemKind k)
{

    unsigned long magic_number, memory_kind;
    char *whole_memory_area_ptr = ptr; // Por cada aumento en unidad, se mueve en un octeto de memoria

    char *first_size_marker_ptr = ptr;
    char *first_magic_number_ptr = ptr + SIZE_MARKER_LENGTH;

    char *last_magic_number_ptr = ptr + (size - SIZE_MARKER_LENGTH - MAGIC_NUMBER_LENGTH);
    char *last_size_marker_ptr = ptr + (size - SIZE_MARKER_LENGTH);

    magic_number = knuth_mmix_one_round((unsigned long)ptr);

    switch (k)
    {
    case SMALL_KIND:
        memory_kind = 0;
        break;
    case MEDIUM_KIND:
        memory_kind = 1;
        break;
    case LARGE_KIND:
        memory_kind = 2;
        break;
    default:
        break;
    }

    magic_number = (magic_number & ~(0b11UL)) | (memory_kind);

    *(unsigned long *)first_size_marker_ptr = size;
    *(unsigned long *)first_magic_number_ptr = magic_number;
    // printf("%lu \n", *(unsigned long*)whole_memory_area_ptr );

    // Pour chercher la fin du bloc
    *(unsigned long *)last_magic_number_ptr = magic_number; // pour ecrire le marquage à la fin du bloc
    // printf("%lu\n", *(unsigned long*)whole_memory_area_ptr );
    *(unsigned long *)last_size_marker_ptr = size;

    return whole_memory_area_ptr + SIZE_MARKER_LENGTH + MAGIC_NUMBER_LENGTH;
}

Alloc mark_check_and_get_alloc(void *ptr)
{
    /* ecrire votre code ici */
    char *memory_zone_start_ptr = ptr - SIZE_MARKER_LENGTH - MAGIC_NUMBER_LENGTH;

    char *first_size_mark_ptr = memory_zone_start_ptr; // déplacement d'un octet par chaque +1/-1
    char *first_magic_number_ptr = memory_zone_start_ptr + SIZE_MARKER_LENGTH;
    char *last_magic_number_ptr;
    char *last_size_mark_ptr;

    unsigned long memory_kind, first_size_marker, last_size_marker, first_magic_num, last_magic_num;
    MemKind kind;

    first_size_marker = *(unsigned long *)first_size_mark_ptr;
    first_magic_num = *(unsigned long *)first_magic_number_ptr;

    last_magic_number_ptr = memory_zone_start_ptr + first_size_marker - SIZE_MARKER_LENGTH - MAGIC_NUMBER_LENGTH;
    last_size_mark_ptr = memory_zone_start_ptr + first_size_marker - SIZE_MARKER_LENGTH;

    printf("numMag I = %lu\n", first_magic_num);

    last_magic_num = *(unsigned long *)last_magic_number_ptr;

    printf("numMag F = %lu\n", last_magic_num);
    assert(first_magic_num == last_magic_num);

    last_size_marker = *(unsigned long *)last_size_mark_ptr;
    printf("total_size_mark_1 = %lu\n", first_size_marker);
    printf("total_size_mark_2 = %lu\n", last_size_marker);

    assert(first_size_marker == last_size_marker);

    memory_kind = first_magic_num & (0b11UL);

    switch (memory_kind)
    {
    case 0:
        kind = SMALL_KIND;
        break;
    case 1:
        kind = MEDIUM_KIND;
        break;
    case 2:
        kind = LARGE_KIND;
        break;
    default:
        break;
    }

    // printf("Success in mark check");
    Alloc a = {(void *)memory_zone_start_ptr, kind, first_size_marker}; // on reenvoi le pointeur au debut de la zone alloqué
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
    assert(size == (1 << indice));
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
