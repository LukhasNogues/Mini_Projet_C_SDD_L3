#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "./tree-avl.h"

int int_cmp(const void* a, const void* b) {
    int A = *(int*) a ;
    int B = *(int*) b ;
    return (A > B) - (A < B) ; // A < B vrai => -1
}

void node_println(void* data, void* pbal) {
    printf("Valeur du noeud : %d [Balance : %d]\n", *(int*) data, *(int*) pbal) ;
}

/**
 * Test réalisés pour les arbres binaires
 * Affichage des résultats des tris
 */
void testArbresAVL(void) {
    int i = 5, j = 10, k = 15, m = 20 ;
    size_t sizeInt = sizeof(int) ;

    TreeAVL racine = treeAVL_create(&i, sizeInt) ;
    printf("==== Affichage racine de départ ====\n") ; treeAVL_show(racine) ;
    
    TreeAVL noeud1 = treeAVL_create(&j, sizeInt) ;
    TreeAVL noeud2 = treeAVL_create(&k, sizeInt) ;
    TreeAVL noeud3 = treeAVL_create(&m, sizeInt) ;
    TreeAVL noeud4 = treeAVL_create(&i, sizeInt) ;

    treeAVL_add_data(&racine, noeud1->data, sizeInt, int_cmp) ;
    printf("==== Insertion noeud1 ====\n") ; treeAVL_show(racine) ;

    treeAVL_add_data(&racine, noeud2->data, sizeInt, int_cmp) ;
    printf("==== Insertion noeud2 ====\n") ; treeAVL_show(racine) ; // Changement de racine

    treeAVL_add_data(&racine, noeud3->data, sizeInt, int_cmp) ;
    printf("==== Insertion noeud3 ====\n") ; treeAVL_show(racine) ;

    treeAVL_add_data(&racine, noeud4->data, sizeInt, int_cmp) ;
    printf("==== Insertion noeud4 ====\n") ; treeAVL_show(racine) ;



    printf("Affichage pre-order : \n") ;
    treeAVL_pre_order(racine, node_println, NULL) ;
    // node_println(NOEUD->data : void*, &NOEUD->balance : int* -> void*) ;

    printf("\nAffichage post-order : \n") ;
    treeAVL_post_order(racine, node_println, NULL) ;

    printf("\nAffichage in-order : \n") ;
    treeAVL_in_order(racine, node_println, NULL) ;

    // Les données sont sur la pile (ou "stack", les variables ijkm sont locales).
    // Pas besoin d'appeler free donc pointeur nul.
    treeAVL_delete(racine, NULL) ;
}



/**
 * ======== AVANT TOUTE CHOSE... ========
 * 
 * > cd [...]/debug/tree-avl
 * 
 * 
 * ======== PREPARATION : CMAKE ========
 * 
 * > cmake ../../src/tree-avl/ -DCMAKE_INSTALL_PREFIX=../tmp -DCMAKE_BUILD_TYPE=Debug
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
 * > ./test-tree-avl
 */



int main(void) {
    testArbresAVL() ;

    TreeAVL A = treeAVL_create(&(int) {4}, sizeof(int)) ; // Insertions : 4 5 2 3 1
    printf("==== Affichage racine initiale ====\n") ;
    treeAVL_show(A) ;

    printf("==== Insertion 5 ====\n") ;
    treeAVL_add_data(&A, &(int) {5}, sizeof(int), int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Insertion 2 ====\n") ;
    treeAVL_add_data(&A, &(int) {2}, sizeof(int), int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Insertion 3 ====\n") ;
    treeAVL_add_data(&A, &(int) {3}, sizeof(int), int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Insertion 1 ====\n") ;
    treeAVL_add_data(&A, &(int) {1}, sizeof(int), int_cmp) ;
    treeAVL_show(A) ;

    //printf("==== Test Rotation Droite ====\n") ;
    //treeAVL_RightRotation(&A) ;
    //treeAVL_show(A) ;

    //printf("==== Test Rotation Gauche ====\n") ;
    //treeAVL_LeftRotation(&A) ;
    //treeAVL_show(A) ;

    printf("==== Insertion 0 ====\n") ;
    treeAVL_add_data(&A, &(int) {0}, sizeof(int), int_cmp) ; // 0
    treeAVL_show(A) ;

    printf("==== Insertion -1 ====\n") ;
    treeAVL_add_data(&A, &(int) {-1}, sizeof(int), int_cmp) ; // -1
    treeAVL_show(A) ;

    printf("==== Suppression de 4 (exception : fils gauche prend la place) ====\n") ;
    treeAVL_del_data(&A, &(int) {4}, NULL, int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Suppression de 2 (application de la convention du fils droit) ====\n") ;
    treeAVL_del_data(&A, &(int) {2}, NULL, int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Suppression de 3 ====\n") ;
    treeAVL_del_data(&A, &(int) {3}, NULL, int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Suppression de 0 ====\n") ;
    treeAVL_del_data(&A, &(int) {0}, NULL, int_cmp) ;
    treeAVL_show(A) ;

    printf("==== Suppression de 1 (réapplication de la convention du fils droit) ====\n") ;
    treeAVL_del_data(&A, &(int) {1}, NULL, int_cmp) ;
    treeAVL_show(A) ;

    treeAVL_delete(A, NULL) ;
    return EXIT_SUCCESS ;
}
