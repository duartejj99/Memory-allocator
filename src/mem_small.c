/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

void *
emalloc_small(unsigned long size)
{
    void **l=NULL; // J'suis dans un arq de x64 ---- void * se mueve 1 por 1 incremento y void ** se mueve 8 por cada incremento

    if(arena.chunkpool==NULL){
        int sizeBloq;
        sizeBloq=mem_realloc_small();
        createListCh(arena.chunkpool,sizeBloq/96);
    }

    l= (void **)arena.chunkpool;

    arena.chunkpool= *l; //chainage - on actualize la tête de la liste
    void *p = mark_memarea_and_get_user_ptr((void *)l, 96, SMALL_KIND);
    return p;


}

void createListCh(void **p, int numchunk){

    for(int i=1;i<numchunk;i++){

        *p=p+12; // 12*8=96 oct -- Los void ** se mueven 8 oct con cada aumento unitario 
        p=p+12;


    }
    *p=0;

}

void efree_small(Alloc a){
    

    void **aux;
    aux = a.ptr;
    *aux= arena.chunkpool;
    arena.chunkpool = aux;
}
