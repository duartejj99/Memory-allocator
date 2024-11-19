/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

unsigned int puiss2(unsigned long size) {
    unsigned int p=0;
    size = size -1; /* allocation start in 0 - ex =>  input =128; 0-127
                        */
    while(size) {  // get the largest bit
	p++;
	size >>= 1;
    }
    if (size > (1 << p)) // useless??
	p++;
    return p;
}


void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);

    void **ptrBloque = 0;
    int puiss = puiss2(size); //Potencia que necesito segun la potencia entregada por el usuario [minimo ind de TZL que 
    //necesito para allocar mi bloque]
    int posTZL = puiss; 


    while(posTZL<(FIRST_ALLOC_MEDIUM_EXPOSANT +arena.medium_next_exponant)){ //busco el minimo bloque (recorre desde el indice puis hasta TZL_SIZE (48))
        if(arena.TZL[posTZL]!=0){
            ptrBloque = arena.TZL[posTZL]; /* direccion de la lista de bloques que vamos 
                                            a cortar (bloque de menor tamaño posible) */
            
            
            break; 
        }

        posTZL++;

        if(posTZL==(FIRST_ALLOC_MEDIUM_EXPOSANT +arena.medium_next_exponant)){
             int size = mem_realloc_medium();
                /*mem realloc medium tambien actualiza la direccion del puntero de
                la casilla que va a apuntar al bloque creado*/

             posTZL=puiss2(size); //posicion en la tabla de zonas libres de la nueva  

             ptrBloque=arena.TZL[posTZL]; // Puntero del bloque creado

        }
    }


   if(posTZL!=puiss){ //COMPARACION DE SI EL BLOQUE ES DEL TAMAÑO QUE NECESITAMOS O MAYOR

        ptrBloque = coupage(ptrBloque,posTZL,puiss);
    }else{
        arena.TZL[posTZL] = *(void **)arena.TZL[posTZL]; // actualización de la cabeza de la lista
    }



    void *p = mark_memarea_and_get_user_ptr(ptrBloque, 1<<puiss, MEDIUM_KIND);
    return p;
   
   
}

void **coupage(void **ptrBloque,int posTZL,int puiss){ // *ptr= dir bloq a cortar, posTZL= potActual, puiss=pot Deseada
    
    unsigned long desp;
 

    do{
        

        arena.TZL[posTZL] = *(void **)arena.TZL[posTZL]; //actualizar la cabezera de la lista

        desp = (1<<(posTZL-1))/8; //div entre 8 porque el puntero se mueve 8oct por cada ++ --- PORQUE ES UN PUNTERO DOBLE VOID **

        ptrBloque += desp;      // Dir 2do bloque resultante
        *ptrBloque = arena.TZL[posTZL-1]; 
        arena.TZL[posTZL-1] = (void *)ptrBloque; //insertion en tête

        ptrBloque-=desp; //Dir 1er bloque resultante
        *ptrBloque = arena.TZL[posTZL-1]; //
        arena.TZL[posTZL-1] = (void *)ptrBloque; //insertion en tête
        
        
        posTZL--;

    }while(puiss!=posTZL);



    arena.TZL[posTZL]=*(void **)arena.TZL[posTZL]; //*
    return ptrBloque; //ptrBloque tiene la direccion del ultimo bloque insertado

   
        
}


void efree_medium(Alloc a) {




    void **aux;
    int puiss = puiss2(a.size);
     *(unsigned long *)a.ptr = 0;



    if(arena.TZL[puiss]==0){  
        
        //no hay ningun elemento en la lista
        arena.TZL[puiss] = a.ptr;
        aux=(void **)arena.TZL[puiss];
        assert(aux!=0);
    }else{

        void ** aux;
        void * addrBuddy;
        void ** ptrRef;
        void ** ptrAnterior; 
        unsigned long valAddrBuddy;
        valAddrBuddy = (unsigned long)( (unsigned long)a.ptr ^a.size); //Dir bloq XOR taille
        addrBuddy = (unsigned long *)valAddrBuddy;

        //se recorre la lista 

        
        int buddyLista = 0; // boolean de confirmación o negación
        ptrRef = arena.TZL[puiss];
        ptrAnterior = arena.TZL[puiss];


        // Busqueda del buddy dentro de la lista

        do{
            if(ptrRef == addrBuddy){
                buddyLista = 1;
                break;
            }
            ptrAnterior = ptrRef;
            ptrRef = *ptrRef;
        }while(ptrRef!=0);
        
        if(buddyLista==1){
            // se hace la fusion 
            
            //se saca el buddy de la lista
            if(ptrAnterior==ptrRef){ 
                //saber si el buddy es el primero en la lista
                arena.TZL[puiss] = *(void **)arena.TZL[puiss];

            }else{ 
                //el buddy esta en medio de la lista
                *ptrAnterior = *(void **)ptrRef;  /* encadeno el
                                                  el bloque anterior al buddy con el 
                                                  bloque posterior al buddy */
            }

            //se fusiona el buddy con el bloque 
            if((unsigned long)ptrRef>(unsigned long)a.ptr){
                //el buddy es el primer bloque 
                aux = a.ptr;
            }else{
                //el buddy es el segundo bloque
                aux = ptrRef;
            }
            *aux=arena.TZL[puiss+1];
            arena.TZL[puiss+1] = aux;
            
        }else{ //no se encuentra el body en la lista
            //se agrega a la lista
            aux = a.ptr;
            *aux = arena.TZL[puiss];
            arena.TZL[puiss] = a.ptr;
        }

     
    }



}


