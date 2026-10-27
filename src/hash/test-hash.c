#include <stdio.h>
#include <string.h>
#include "./hash.h"
//#include "./hash.inc"

typedef struct _Couple {
    char* mot ;
    char* def ;
} Couple ;



size_t hash_string(char* str) {
	unsigned int hash = 0 ;
	int c = *str ;
	while (c) {
        c = *str++ ; // ++ prioritaire sur le déréférencement
        // (sinon ça changerait le contenu de la chaîne)
        // => *str++ = *(str++) :
        // lit le contenu de str (*str), puis avance dans la chaîne (str++)
	    hash += c ;
    }
	return hash ;
}



size_t couple_hash(const void* p, size_t n) {
    if (!p) return -1 ;

    return hash_string(((Couple*) p)->mot) % n ;
}

int couple_cmp(const void* p1, const void* p2) {
    if (!p1 || !p2) return -2 ;

    int cmp = strcmp(((Couple*) p1)->mot, ((Couple*) p2)->mot) ;
    if (cmp < 0) return -1 ;
    else if (!cmp) return 0 ;
    return 1 ; // cmp > 0
}

void couple_del(void* data) {
    if (!data) return ;
    free(((Couple*) data)->mot) ;
    free(((Couple*) data)->def) ;
}

void couple_print(void* data, FILE* stream) {
    if (!data || !stream) return ;
    fprintf(stream, "%s : %s\n", ((Couple*) data)->mot, ((Couple*) data)->def) ;
}


size_t int_hash(const void* p, size_t n) {
    if (!p) return -1 ;

    int value = *(int*) p ;
    int hash = value % (int) n ;
    if (hash < 0) hash += (int) n ;
    return (size_t) hash ;
}

int int_cmp(const void* ptr_vers_int1, const void* ptr_vers_int2) {
    if (!ptr_vers_int1 || !ptr_vers_int2) return -2 ;

    if (*(int*) ptr_vers_int1 < *(int*) ptr_vers_int2) return -1 ;
    if (*(int*) ptr_vers_int1 > *(int*) ptr_vers_int2) return  1 ;
    return 0 ;
}

void int_del(void* data) {
    (void) data ;
}

void int_print(void* data, FILE* stream) {
    if (!data || !stream) return ;
    fprintf(stream, "%d\n", *(int*) data) ;
}


// Chaînes allouées dynamiquement
size_t str_hash(const void* str, size_t capacite_table) {
    if (!str) return -1 ;

    return hash_string((char*) str) % capacite_table ;
}

int str_cmp(const void* str1, const void* str2) {
    if (!str1 || !str2) return -2 ;

    int cmp = strcmp((const char*) str1, (const char*) str2) ;
    if (cmp < 0) return -1 ;
    else if (!cmp) return 0 ;
    else return 1 ; // cmp > 0
}

void str_del(void* data) {
    if (!data) return ;
    free(data) ;
}

void str_print(void* data, FILE* stream) {
    if (!data || !stream) return ;
    fprintf(stream, "\"%s\"\n", (const char*) data) ;
}

/**
 * RUN TEST-HASH (test-hash) :
 * (1) GO TO FOLDER "debug/list/"
 * (2) MIGHT NEED TO CLEAN EVERYTHING UP WITH rm -rf ./*
 * (3) cmake ../../src/list/ -DCMAKE_INSTALL_PREFIX=../tmp -DCMAKE_BUILD_TYPE=Debug
 * (4) COMPILE WITH make THEN make install
 * 
 * (5) EXECUTE ./test-list (POINTLESS)
 * (6) THEN echo $? TO CHECK EXIT CODE (0 IF EXITED NORMALLY)
 * 
 * (7) GO TO FOLDER "debug/hash/"
 * (8) MIGHT NEED TO CLEAN EVERYTHING UP WITH rm -rf ./*
 * (9) cmake ../../src/hash/ -DCMAKE_INSTALL_PREFIX=../tmp -DCMAKE_BUILD_TYPE=Debug
 * (10) COMPILE WITH make THEN make install
 * 
 * (11) EXECUTE WITH ./test-hash
 */

int main(void) {
    Couple c, c1, c2, c3, c4 ;
    HashTable* table = hashtable_new(sizeof(Couple), couple_hash, couple_cmp) ;

    size_t i ;
    size_t nb = 10 ;

    c1.mot = malloc(10) ;
	c1.def = malloc(10) ;
    strcpy(c1.mot, "mot1") ;
    strcpy(c1.def, "def1") ;
    // OU JUSTE :
    //c1.mot = strdup("mot1") ;
    //c1.def = strdup("def1") ;
    printf("----------\n") ;

    hashtable_insert(&table, &c1, couple_del) ;
    hashtable_print(table, couple_print, stdout) ;
    printf("----------\n") ;

    c2.mot = malloc(10) ;
    c2.def = malloc(10) ;
    strcpy(c2.mot, "mot2") ;
    strcpy(c2.def, "def2") ;

    hashtable_insert(&table, &c2, couple_del) ;
	hashtable_print(table, couple_print, stdout) ;
    printf("----------\n") ;

    c3.mot = malloc(10) ;
    c3.def = malloc(10) ;
    strcpy(c3.mot, "mot3") ;
    strcpy(c3.def, "def3") ;

    hashtable_insert(&table, &c3, couple_del) ;
	hashtable_print(table, couple_print, stdout) ;
    printf("----------\n") ;

    c4.mot = malloc(10) ;
    c4.def = malloc(10) ;
    strcpy(c4.mot, "mot4") ;
    strcpy(c4.def, "def4") ;

    hashtable_insert(&table, &c4, couple_del) ;
    hashtable_print(table, couple_print, stdout) ;
    printf("----------\n") ;



    hashtable_retract(&table, &c1, couple_del) ; // doit enlever c1 de la table
    hashtable_print(table, couple_print, stdout) ;
    printf("----------\n") ;


    Couple* target = hashtable_lookup(table, &c3) ;
    if (target)
        printf(
            "Recherche définition de %s : %s\n"
            "----------\n",
            target->mot,
            target->def
        ) ;
    else
        printf(
            "Cible non trouvée\n"
            "----------\n"
        ) ;

    hashtable_delete(table, couple_del) ;

//printf("%p\n", table) ;
    HashTable* table_d_entiers = hashtable_new(sizeof(int), int_hash, int_cmp) ;
//printf("%p\n", table_d_entiers) ; // même adresse ???
    int n1 = -91 ;
    int n2 = 2147483648 ; // integer overflow (dépassement d'entier)
    int n3 = 0 ;
    int n4 = 'a' ;

    // Entiers : pas créés avec malloc
    // => stockés sur la pile (stack) et non sur le tas (heap)
    // => pas de free !
    // => delete = NULL
    hashtable_insert(&table_d_entiers, &n1, NULL) ;
    hashtable_insert(&table_d_entiers, &n2, NULL) ;
    hashtable_insert(&table_d_entiers, &n3, NULL) ;
    hashtable_insert(&table_d_entiers, &n4, NULL) ;

    hashtable_print(table_d_entiers, int_print, stdout) ;
    // on se rend compte que l'ordre d'insertion n'est pas nécessairement conservé
    printf("----------\n") ;

    int n5 = -91 ;
    int* trouve = hashtable_lookup(table_d_entiers, &n5) ;
    if (trouve)
        printf(
            "Entier trouvé : %d\n"
            "----------\n",
            *trouve
        ) ;

    hashtable_retract(&table_d_entiers, &n1, NULL) ;
    hashtable_print(table_d_entiers, int_print, stdout) ;
    printf("----------\n") ;

    hashtable_delete(table_d_entiers, NULL) ;

    return EXIT_SUCCESS ;
}
