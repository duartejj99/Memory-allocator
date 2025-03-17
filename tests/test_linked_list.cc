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

TEST(General, linked_list)
{

  // Arrange
  //  I need a block of mem
  unsigned long size = 1 << 17;
  unsigned long element_size = 1 << 15;
  void *memory_block = mmap(0,
                            size * 2, // twice the size to allign
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS,
                            -1,
                            0);
  void *head = create_linked_list((void **)memory_block, size, element_size);
  // Act
  unsigned long length = linked_list_length(head);
  // Verify # of elements on linked list
  ASSERT_EQ(length, 4);
}

TEST(General, poll)
{

  // Arrange
  //  I need a block of mem
  unsigned long size = 1 << 17;
  unsigned long element_size = 1 << 15;
  void *memory_block = mmap(0,
                            size * 2, // twice the size to allign
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS,
                            -1,
                            0);
  void *head = create_linked_list((void **)memory_block, size, element_size);
  // Act
  unsigned long length = linked_list_length(head);
  for (unsigned long i = 0; i < length; i++)
  {
    generic_poll(&head);
  }
  // Assert
  ASSERT_EQ(head, (void *)NULL);
}
