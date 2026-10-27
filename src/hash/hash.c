#include "./hash.h"
//#include "./hash.inc"

#include <stdlib.h>
#include <stdbool.h>   
#include <string.h> 

const static size_t hashtable_primes[] = {
    11,
    19,
    37,
    73,
    109,
    163,
    251,
    367,
    557,
    823,
    1237,
    1861,
    2777,
    4177,
    6247,
    9371,
    14057,
    21089,
    31627,
    47431,
    71143,
    106721,
    160073,
    240101,
    360163,
    540217,
    810343,
    1215497,
    1823231,
    2734867,
    4102283,
    6153409,
    9230113,
    13845163,
} ;

static const double HASHTABLE_MIN_COLLISIONS = 0.3 ;
static const double HASHTABLE_MAX_COLLISIONS = 3.0 ;

static const size_t HASHTABLE_NB_PRIMES = sizeof(hashtable_primes) / sizeof(size_t) ;

#define HASHTABLE_MIN_SIZE hashtable_primes[0]
#define HASHTABLE_MAX_SIZE hashtable_primes[HASHTABLE_NB_PRIMES - 1]

static HashTable* _hashtable_new(
    size_t length,
    size_t size,
    size_t (*hash)(const void*, size_t),
    int (*compare)(const void*, const void*)
) {
    HashTable* table = (HashTable*) malloc(
        3 * sizeof(size_t) +
        2 * sizeof(void*) +
        length * sizeof(List)
    ) ;

    if (table) {
        size_t i ;

        table->hash = hash ;
        table->compare = compare ;
        table->length = length ;
        table->nb = 0ULL ;
        table->size = size ;
        for (i = 0 ; i < length ; ++i) table->list[i] = NULL ;
    }
    return table ;
}

HashTable* hashtable_new(
    size_t size,
    size_t (*hash)(const void*, size_t),
    int (*compare)(const void*, const void*)
) {
    return _hashtable_new(HASHTABLE_MIN_SIZE, size, hash, compare) ;
}

static void _hashtable_resize(HashTable** ptable) {
    double ratio = (*ptable)->nb / (double) (*ptable)->length ;

    if (
        (*ptable)->nb >= HASHTABLE_MIN_SIZE &&
        (*ptable)->nb <= HASHTABLE_MAX_SIZE &&
        (ratio > HASHTABLE_MAX_COLLISIONS || ratio < HASHTABLE_MIN_COLLISIONS)
    ) {
        size_t i = 1 ;
        HashTable* new ;

        while (hashtable_primes[i] < (*ptable)->nb) ++i ;

        new = _hashtable_new(
            hashtable_primes[i],
            (*ptable)->size,
            (*ptable)->hash,
            (*ptable)->compare
        ) ;

        if (new) {
            new->nb = (*ptable)->nb ;

            size_t j ;
            for (j = 0 ; j < (*ptable)->length ; ++j) {
                List list = (*ptable)->list[j], next ;

                while (list) {
                    size_t h = new->hash(list->data, new->length) ;

                    next = list->next ;
                    list->next = new->list[h] ;
                    new->list[h] = list ;
                    list = next ;
                }
            }

            free(*ptable) ;
            *ptable = new ;
        }
    }
}



void hashtable_apply(
    HashTable* table,
    void (*func)(void*, void*),
    void* extra_data
) {
    // À COMPLETER :
    //      applique la fonction func sur tous les éléments de la table en utilisant extra_data.
    if (!table || !func) return ;
    size_t i ;
    for (i = 0 ; i < table->length ; ++i) list_foreach(table->list[i], func, extra_data) ;
    // for (i = 0 ; i < table->length ; ++i) func(table->list[i], extra_data) ; // NON
}



void hashtable_print(
    HashTable* table,
    void (*print)(void*, FILE*),
    FILE* stream
) {
    // À COMPLETER :
    //      écrit la table dans un fichier...
    if (!table || !print || !stream) return ;

    hashtable_apply(table, (void (*)(void*, void*)) print, stream) ;
    // cast de print de "void (*)(void*, FILE*)" vers "void (*)(void*, void*)"
    // Ce n'est en fin de compte qu'un genre de cas particulier de la fonction hashtable_apply
    // où func = print, et extra_data = stream (en l'occurrence un appel à printf ou fprintf
    // et stdout - la sortie standard - pour les tests).
}



void hashtable_delete(HashTable* table, void (*delete)(void*)) {
    // À COMPLETER :
    //      supprime tous les éléments de la table.
    if (!table) return ;

    size_t i ;
    for (i = 0 ; i < table->length ; ++i) {
        list_delete(&table->list[i], delete) ;
        table->list[i] = NULL ;
    }
    free(table) ;
}



int hashtable_insert(HashTable** ptable, void* data, void (*delete)(void*)) {
    // À COMPLETER :
    //      insère l'élément data dans la table ptable, accompagné de son destructeur.
    if (!ptable || !*ptable || !data) return 0 ;

    size_t index = (*ptable)->hash(data, (*ptable)->length) ;
    void* tmp = list_find_data((*ptable)->list[index], data, (*ptable)->compare) ;

    if (tmp) { // S'il y a quelque chose à cet indice, si la donnée est déjà présente.
        if (delete) delete(tmp) ;
        memcpy(tmp, data, (*ptable)->size) ;
        return true ;
    }
    else { // S'il n'y a pas la donnée...
        if (list_prepend((*ptable)->list + index, data, (*ptable)->size)) {
            // list_prepend : crée liste avec la donnée qu'on veut insérer.
            ++(*ptable)->nb ;
            _hashtable_resize(ptable) ;
            return true ;
        }
        return false ;
    }
}



int hashtable_retract(HashTable** ptable, void* data, void (*delete)(void*)) {
    // À COMPLETER :
    //      supprime un élément d'une table.
    if (!ptable || !*ptable || !data) return false ;

    HashTable* table = *ptable ;
    size_t h = table->hash(data, table->length) ;

    if (!table->list[h]) return false ;

    int idx = list_index_data(table->list[h], data, table->compare) ;

    if (idx < 0) return false ;

    if (
        !list_remove_nth(&table->list[h], (size_t) idx, delete)
    ) return false ;

    --table->nb ;
    _hashtable_resize(ptable) ;
    return true ;
}



void* hashtable_lookup(HashTable* table, void* data) {
    // À COMPLETER :
    //      recherche d'un élément dans une table.
    if (!table || !data) return NULL ;

    size_t h = table->hash(data, table->length) ;
    if (!table->list[h]) return NULL ;

    return list_find_data(table->list[h], data, table->compare) ;
}
