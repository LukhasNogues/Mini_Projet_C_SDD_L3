#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "./tree.h"

void monPrintF(void* a, void* b) {
    printf("Valeur du noeud : %d\n", *(int*) a) ;
}

/**
 * Test réalisés pour les arbres binaires
 * Affichage des résultats des tris
 */
void testArbresBinaires(void) {
    int i = 5, j = 10, k = 15, m = 20 ;
    size_t sizeInt = sizeof(int) ;

    Tree racine = tree_create(&i, sizeInt) ;
    Tree fils1 = tree_create(&j, sizeInt) ;
    Tree fils2 = tree_create(&k, sizeInt) ;

    tree_set_left(racine, fils1) ;
    tree_set_right(racine, fils2) ;

    Tree fils1fils1 = tree_create(&m, sizeInt) ;
    Tree fils2fils1 = tree_create(&i, sizeInt) ;

    tree_set_left(fils1, fils1fils1) ;
    tree_set_right(fils1, fils2fils1) ;

    printf("Tri pre-order : \n") ;
    tree_pre_order(racine, monPrintF, NULL) ;

    printf("\nTri post-order : \n") ;
    tree_post_order(racine, monPrintF, NULL) ;

    printf("\nTri in-order : \n") ;
    tree_in_order(racine, monPrintF, NULL) ;
    
    // Les données sont sur le stack (variables locales i, j, k, m).
    // Pas besoin d'appeler free donc pointeur nul.
    tree_delete(racine, 0) ;
}



/**
 * ======== AVANT TOUTE CHOSE... ========
 * 
 * > cd [...]/debug/tree
 * 
 * 
 * ======== PREPARATION : CMAKE ========
 * 
 * > cmake ../../src/tree/ -DCMAKE_INSTALL_PREFIX=../tmp -DCMAKE_BUILD_TYPE=Debug
 * 
 * 
 * ======== COMPILATION : MAKE ========
 * 
 * > make
 * (> make install)
 * 
 * 
 * ======== RUN TEST ========
 * 
 * > ./test-tree
 */

int main(void) {
    testArbresBinaires() ;

    return EXIT_SUCCESS ;
}
