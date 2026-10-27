#ifndef HASH_H_
#define HASH_H_

#include <stdlib.h> 
#include <list.h> 
#include <stdio.h> 


typedef struct _HashTable {
    size_t (*hash)(const void*, size_t) ;
    int (*compare)(const void*, const void*) ;
    size_t length ; // Capacité de la table : length List en tout
    size_t nb ; // Nombre réel de données dans la table
    size_t size ; // taille du type de données qu'on veut insérer (4 (int), 8 (pointeur)...)
    // toutes les données doivent avoir la même taille (plus strict : doivent être du même type)
    List list[1] ;
} HashTable ;



HashTable* hashtable_new(
    size_t size,
    size_t (*hash)(const void*, size_t),
    int (*compare)(const void*, const void*)
) ;

void hashtable_apply(
    HashTable* table,
    void (*func)(void*, void*),
    void* extra_data
) ;

void hashtable_print(
    HashTable* table,
    void (*print)(void*, FILE*),
    FILE* stream
) ;

void hashtable_delete(HashTable* table, void (*delete)(void*)) ;

int hashtable_insert(HashTable** ptable, void* data, void (*delete)(void*)) ;

int hashtable_retract(HashTable** ptable, void* data, void (*delete)(void*)) ;

void* hashtable_lookup(HashTable* table, void* data) ;

#endif // HASH_H_
