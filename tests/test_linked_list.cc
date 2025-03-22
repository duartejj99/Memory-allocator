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

TEST(Linked_list, length)
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
  void *head = new_linked_list((void **)memory_block, size, element_size);
  // Act
  unsigned long length = linked_list_length(head);
  // Verify # of elements on linked list
  ASSERT_EQ(length, 4);
}

TEST(Linked_list, poll)
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
  void *head = new_linked_list((void **)memory_block, size, element_size);
  // Act
  unsigned long length = linked_list_length(head);
  for (unsigned long i = 0; i < length; i++)
  {
    poll(&head);
  }
  // Assert
  ASSERT_EQ(head, (void *)NULL);
}

TEST(Linked_list, remove_element)
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
  void *head = new_linked_list((void **)memory_block, size, element_size);
  void *second_element = (char *)memory_block + 1 * element_size;
  void *third_element = (char *)memory_block + 2 * element_size;
  void *fourth_element = (char *)memory_block + 3 * element_size;

  // Act
  void *removed_block = remove_element(&head, third_element);

  // Assert
  ASSERT_EQ(*(void **)second_element, fourth_element);
  ASSERT_EQ(removed_block, third_element);
}