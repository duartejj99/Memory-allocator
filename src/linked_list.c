#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

/// @brief Creates a new linked list of memory chunks of size element_length from a memory block
/// of size memory_length, pointed by memory_block
/// @param memory_block The reference pointing to the memory block to divide into chunks
/// @param memory_length The memory block total size in bytes
/// @param element_length The size of each element/chunk in bytes
/// @return The linked list's head
void *new_linked_list(void **memory_block, unsigned long memory_length, unsigned long element_length)
{
    assert(memory_length % element_length == 0);
    void *head = memory_block;
    unsigned long number_of_elements = memory_length / element_length;

    void **current_element = memory_block;
    void **next_element = NULL;
    for (int element_idx = 0; element_idx < number_of_elements - 1; element_idx++)
    {
        next_element = (void **)((char *)current_element + element_length);
        *current_element = next_element;
        current_element = (void **)next_element;
    }

    *current_element = NULL; // last element

    return head;
}

/// @brief Removes and returns the memory block from the linked list pointed by pool HEAD
/// @param head_ref is the HEAD reference, allowing to modify HEAD.
/// @return The new head value.
void *poll(void **head_ref)
{
    assert(*head_ref != NULL);

    void *old_head = *head_ref;              // here I have value stored on HEAD
    void *next_element = *(void **)old_head; // The value stored on HEAD is a pointer, I read the value stored on that REF, which is the next element
    *head_ref = next_element;
    return old_head;
}

/// @brief Verifies if the linked list is empty
/// @param head The linked list's head
/// @return True if the linked list is empty, false if it isn't
bool is_linked_list_empty(void *head)
{
    return head == NULL;
};

/// @brief Calculates the number of elements on the linked list
/// @param head The linked list's head
/// @return The number of elements on the linked list
unsigned long linked_list_length(void *head)
{
    assert(head != NULL);

    void **iterator = head;
    unsigned long length = 1; // counting the one pointed by head
    while (*iterator != NULL)
    {
        iterator = *(void **)iterator;
        length++;
    }

    return length;
}

/// @brief Verifies if the element is on the Linked list
/// @param head The linked list head
/// @param element The element to look for
/// @return Returns true if it's on the linked list, false if not
bool contains(void *head, void *element)
{
    assert(head != NULL);
    assert(element != NULL);

    void **iterator = head;

    do
    {
        if (iterator == element)
        {
            return true;
        }
        iterator = *(void **)iterator;

    } while (*iterator != NULL);

    return false;
}

// Expects &head and not head
// could be null if element is not present

/// @brief Remove an element from the linked list
/// @param head_ref The linked list's head reference, because head may be changed
/// @param element The element to remove
/// @return The removed element pointer
void *remove_element(void **head_ref, void *element)
{
    assert(head_ref != NULL);
    assert(*head_ref != NULL); // HEAD
    assert(element != NULL);

    void **iterator = *head_ref;

    // Special case
    // First element case (We have to change head)
    if (iterator == element)
    {
        *head_ref = *iterator;
        return iterator;
    }

    // Other elements
    while (*iterator != NULL) // Is the next block null? I already know the actual one is not the element
    {
        // the next block is the element
        if (*iterator == element)
        {
            //  point to the 1st block after element
            *iterator = *(void **)element;
            //  return removed
            return element;
        }
        iterator = *(void **)iterator; // iteration advance one block
    }

    return NULL;
}