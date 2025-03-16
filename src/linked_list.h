

#ifdef __cplusplus
extern "C"
{
#endif
    void *create_linked_list(void **memory_block, unsigned long memory_length, unsigned long element_length);
    void *generic_poll(void **head_ref);
    void is_empty();
    unsigned long linked_list_length(void *head);

#ifdef __cplusplus
}
#endif