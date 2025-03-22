/******************************************************
 * Copyright Grégory Mounié 2008-2018                 *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <gtest/gtest.h>
#include <string.h>
#include <assert.h>
#include "../src/linked_list.h"
#include <sys/mman.h>
#include "../src/mem_internals.h"
#include "../src/mem_medium.h"

unsigned long
mem_realloc_medium_mock(MemArena *arena)
{
  uint32_t indice = FIRST_ALLOC_MEDIUM_EXPOSANT + arena->medium_next_exponant;
  assert(arena->TZL[indice] == 0);
  unsigned long size = (FIRST_ALLOC_MEDIUM << arena->medium_next_exponant);
  arena->TZL[indice] = mmap(0,
                            size * 2, // twice the size to allign
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS,
                            -1,
                            0);
  if (arena->TZL[indice] == MAP_FAILED)
    handle_fatalError("medium realloc");
  // align allocation to a multiple of the size
  // for buddy algo
  arena->TZL[indice] = (void *)((intptr_t)arena->TZL[indice] + (size - (((intptr_t)arena->TZL[indice]) % size)));
  arena->medium_next_exponant++;
  return size; // lie on allocation size, but never free
}

const void *NULL_TZL[TZL_SIZE] = {NULL};
const MemArena arena_initialized = {NULL, NULL_TZL, 0, 0};

TEST(General, pool_fragmentation)
{

  // Arrange
  MemArena arena = arena_initialized;
  mem_realloc_medium_mock(&arena);
  unsigned long fragment_size = 1 << 7;

  unsigned int big_block_idx = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant - 1;
  void *block_ptr = arena.TZL[big_block_idx];
  unsigned long size = 1 << big_block_idx;
  MemoryBlock block = {block_ptr, size};

  // Act
  fragment_block(&arena, block, fragment_size);

  // Assert
  for (unsigned int idx = 7; idx < big_block_idx; idx++)
  {

    ASSERT_NE(arena.TZL[idx], (void *)NULL);
  }

  ASSERT_EQ(arena.TZL[big_block_idx], (void *)NULL);
}