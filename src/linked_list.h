

#ifdef __cplusplus
extern "C"
{
#endif
    void *new_linked_list(void **memory_block, unsigned long memory_length, unsigned long element_length);
    void *poll(void **head_ref);
    bool is_linked_list_empty(void *head);
    unsigned long linked_list_length(void *head);

#ifdef __cplusplus
}
#endif