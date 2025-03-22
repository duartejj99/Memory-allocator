
#ifndef MEM_MEDIUM_H
#define MEM_MEDIUM_H

#include "mem_internals.h"

#ifdef __cplusplus
extern "C"
{
#endif
    struct MemoryBlock
    {
        void *ptr;
        unsigned long size;
    };
    void fragment_block(MemArena *arena, struct MemoryBlock block, unsigned long fragment_size);

#ifdef __cplusplus
}
#endif

#endif