#include <stddef.h>
#include <assert.h>
#include <stdio.h>
// should I make a struct?
//  I don't think so, there is no an object to retain
//  but rather just memory

// In bytes
void *create_linked_list(void **memory_block, unsigned long memory_length, unsigned long element_length)
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
void *generic_poll(void **head_ref)
{
    assert(*head_ref != NULL);

    void *old_head = *head_ref;              // here I have value stored on HEAD
    void *next_element = *(void **)old_head; // The value stored on HEAD is a pointer, I read the value stored on that REF, which is the next element
    *head_ref = next_element;
    return old_head;
}

void is_empty() {

};

unsigned long linked_list_length(void *head)
{
    void **iterator = head;
    unsigned long length = 1; // counting the one pointed by head
    while (*iterator != NULL)
    {
        iterator = *(void **)iterator;
        length++;
    }

    return length;
}