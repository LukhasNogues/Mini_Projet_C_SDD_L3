#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include "./tree-rb.h"

/**
 * ======== AVANT TOUTE CHOSE... ========
 * 
 * > cd [...]/debug/tree-rb
 * 
 * 
 * ======== PREPARATION : CMAKE ========
 * 
 * > cmake ../../src/tree-rb/ -DCMAKE_INSTALL_PREFIX=../tmp -DCMAKE_BUILD_TYPE=Debug
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
 * > ./test-tree-rb
 */



int int_cmp(const void* pn, const void* pm) {
	if (!pn || !pm) return -2 ;

	const int n = *(const int*) pn ;
	const int m = *(const int*) pm ;
	return (n > m) - (n < m) ;
}

void int_del(void* pn) {
	(void) pn ;
	// Entiers stockés sur la pile : rien à faire en terme de suppression (durée de vie
	// automatique).
}

void int_print(const void* data) {
	if (!data) return ;

	printf("%d", *(const int*) data) ;
}


int double_cmp(const void* data1, const void* data2) {
	if (!data1 || !data2) return -2 ;

	const double x1 = *(const double*) data1 ;
	const double x2 = *(const double*) data2 ;
	return (x1 > x2) - (x1 < x2) ;
}

void double_del(void* data) {
	(void) data ;
}

void double_print(const void* data) {
	if (!data) return ;

	printf("%.3lf", *(const double*) data) ;
}


int string_cmp(const void* str1, const void* str2) {
	if (!str1 || !str2) return -2 ;

	int cmp = strcmp((const char*) str1, (const char*) str2) ;
	if (cmp < 0) return -1 ;
	else if (!cmp) return 0 ;
	else return 1 ; // cmp > 0
}

// Chaînes non allouées dynamiquement (ni malloc/calloc, ni strdup) : chaînes utilisées
// dans le .rodata (read only data), texte brut.
void string_del(void* str) {
	(void) str ;
}

void string_print(const void* str) {
	if (!str) return ;

	printf("\"%s\"", (const char*) str) ;
}



static int verifier_arbre(TreeRB* node, TreeRB* parent, long long min, long long max, bool* valide) ;
static void verifier_arbre_complet(TreeRB* racine) ;
static void tester_suppressions(
    const char* nom,
    const int* insertions,
    size_t nb_insertions,
    const int* suppressions,
    size_t nb_suppressions
) ;



int main(void) {
	clock_t chrono_start = clock() ;

	TreeRB* arbre_test = treeRB_new() ;

	printf("======== Insertion 1 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {1}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 6 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {6}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 8 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {8}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 11 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {11}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 13 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {13}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 15 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {15}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 17 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {17}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 22 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {22}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 25 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {25}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Insertion 27 ========\n") ;
	treeRB_add_data(&arbre_test, &(int) {27}, sizeof(int), int_cmp) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Suppression 22 ========\n") ;
	treeRB_del_data(&arbre_test, &(int) {22}, int_cmp, NULL) ;
	treeRB_print(arbre_test, int_print) ;

	printf("======== Libération des ressources allouées ========\n") ;
	treeRB_delete(arbre_test, NULL) ;

	// https://rbt-visualizer.netlify.app/
	const int insertions1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 } ;
	const int suppressions1[] = { 1, 15, 8, 4, 12, 6, 10, 2, 14, 7, 9, 11, 13, 3, 5 } ;
	tester_suppressions(
		"Insertion ordonnee, suppressions variees",
		insertions1, sizeof insertions1 / sizeof insertions1[0],
		suppressions1, sizeof suppressions1 / sizeof suppressions1[0]
	) ;

	const int insertions2[] = { 8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15 } ;
	const int suppressions2[] = { 1, 3, 2, 5, 7, 6, 4, 9, 11, 10, 13, 15, 14, 12, 8 } ;
	tester_suppressions(
		"Insertion equilibree, suppressions par groupes",
		insertions2, sizeof insertions2 / sizeof insertions2[0],
		suppressions2, sizeof suppressions2 / sizeof suppressions2[0]
	) ;

	const int insertions3[] = { 10, 5, 15, 2, 7, 12, 18, 1, 3, 6, 8, 11, 13, 17, 20 } ;
	const int suppressions3[] = { 10, 5, 15, 2, 18, 7, 12, 1, 20, 3, 6, 8, 11, 13, 17 } ;
	tester_suppressions(
		"Troisieme arbre, suppression de la racine et des sous-arbres",
		insertions3, sizeof insertions3 / sizeof insertions3[0],
		suppressions3, sizeof suppressions3 / sizeof suppressions3[0]
	) ;

	// Test Droite - Gauche
	puts("Test Droite - Gauche") ;
	TreeRB* arbre_test2 = treeRB_new() ;
	treeRB_add_data(&arbre_test2, &(double) {  5. }, sizeof(double), double_cmp) ;
	treeRB_add_data(&arbre_test2, &(double) { 10. }, sizeof(double), double_cmp) ;
	treeRB_add_data(&arbre_test2, &(double) { 7.5 }, sizeof(double), double_cmp) ;
	// Si le programme tourne en boucle : voir _treeRB_insert_case4
	treeRB_print(arbre_test2, double_print) ;
	treeRB_delete(arbre_test2, NULL) ;

	// Test Gauche - Droite
	puts("Test Gauche - Droite") ;
	TreeRB* arbre_test3 = treeRB_new() ;
	treeRB_add_data(&arbre_test3, &(double) { 5. }, sizeof(double), double_cmp) ;
	treeRB_add_data(&arbre_test3, &(double) { 1. }, sizeof(double), double_cmp) ;
	treeRB_add_data(&arbre_test3, &(double) { 3. }, sizeof(double), double_cmp) ;
	// Si le programme tourne en boucle : voir _treeRB_insert_case4
	treeRB_print(arbre_test3, double_print) ;
	treeRB_delete(arbre_test3, NULL) ;


	puts("Test chaînes de caractères") ;
	TreeRB* arbre_test4 = treeRB_new() ;
	treeRB_add_data(&arbre_test4, "42", sizeof "42", string_cmp) ;
	treeRB_add_data(&arbre_test4, "69", sizeof "69", string_cmp) ;
	treeRB_add_data(&arbre_test4, "100", sizeof "100", string_cmp) ;
	treeRB_add_data(&arbre_test4, "17", sizeof "17", string_cmp) ;
	treeRB_add_data(&arbre_test4, "2000", sizeof "2000", string_cmp) ;
	treeRB_print(arbre_test4, string_print) ;
	// L'affichage laisse penser que "2000" est le fils gauche de "69". Vérification :
	const TreeRB* node_having_2000 = treeRB_search(arbre_test4, "2000", string_cmp) ;
	TreeRB* parent_of_n2000 = treeRB_get_parent(node_having_2000) ;
	string_print(treeRB_get_data(parent_of_n2000)) ; // Normalement "17"

	treeRB_delete(arbre_test4, string_del) ;

	printf("\nTous les tests sont passés !\n") ;

	double elapsed_time = (clock() - chrono_start) / (CLOCKS_PER_SEC + 0.) ;
	printf("Temps d'exécution = %.6lf s\n", elapsed_time) ;

	return EXIT_SUCCESS ;
}

/**
 * Vérifier récursivement les propriétés de l'arbre rouge-noir.
 * Retourne la hauteur noire du sous-arbre.
 */
static int verifier_arbre(TreeRB* node, TreeRB* parent, long long min, long long max, bool* valide) {
	if (!node) return 1 ;

	int valeur = *(int*) treeRB_get_data(node) ;
	NodeColor couleur = treeRB_get_color(node) ;

	/* Propriété de l'arbre binaire de recherche. */
	if (valeur <= min || valeur >= max) *valide = false ;

	/* Cohérence des pointeurs parent. */
	if (treeRB_get_parent(node) != parent) *valide = false ;

	TreeRB* gauche = treeRB_get_left(node) ;
	TreeRB* droite = treeRB_get_right(node) ;

	/* Un nœud rouge ne peut pas avoir de fils rouge. */
	if (couleur == RED) {
		if (treeRB_get_color(gauche) == RED || treeRB_get_color(droite) == RED)
			*valide = false ;
	}
	int hg = verifier_arbre(gauche, node, min, valeur, valide) ;
	int hd = verifier_arbre(droite, node, valeur, max, valide) ;
	/* Les deux sous-arbres doivent avoir la même hauteur noire. */
	if (hg != hd) *valide = false ;
	return hg + (couleur == BLACK) ;
}

static void verifier_arbre_complet(TreeRB* racine) {
	if (racine && treeRB_get_color(racine) != BLACK) {
		fprintf(stderr, "ERREUR : la racine n'est pas noire !\n") ;
		abort() ;
	}
	bool valide = true ;

	verifier_arbre(racine, NULL, LLONG_MIN, LLONG_MAX, &valide) ;

	if (!valide) {
		fprintf(stderr, "ERREUR : arbre rouge-noir invalide !\n") ;
		treeRB_print(racine, int_print) ;
		abort() ;
	}
}

static void tester_suppressions(
	const char* nom,
	const int* insertions, size_t nb_insertions,
	const int* suppressions, size_t nb_suppressions
) {
	printf("\n========================================\n") ;
	printf("TEST : %s\n", nom) ;
	printf("========================================\n") ;

	TreeRB* arbre = treeRB_new() ;

	/* Construction de l'arbre. */
	for (size_t i = 0 ; i < nb_insertions ; ++i) {
		assert(treeRB_add_data(&arbre, &insertions[i], sizeof(int), int_cmp)) ;
		verifier_arbre_complet(arbre) ;
	}

	printf("\nArbre initial :\n") ;
	treeRB_print(arbre, int_print) ;

	/* Suppressions successives. */
	for (size_t i = 0 ; i < nb_suppressions ; ++i) {
		int valeur = suppressions[i] ;

		printf("\n----------------------------------------\n") ;
		printf("Suppression de %d\n", valeur) ;
		printf("----------------------------------------\n") ;

		assert(treeRB_search(arbre, &valeur, int_cmp) != NULL) ;
		bool succes = treeRB_del_data(&arbre, &valeur, int_cmp, int_del) ;

		assert(succes) ;
		/* La valeur supprimée ne doit plus être présente. */
		assert(treeRB_search(arbre, &valeur, int_cmp) == NULL) ;

		/* Vérification complète après chaque suppression. */
		verifier_arbre_complet(arbre) ;
		treeRB_print(arbre, int_print) ;

		/* Les autres valeurs qui ne sont pas encore supprimées
			doivent toujours être présentes. */
		for (size_t j = 0 ; j < nb_insertions ; ++j) {
			int candidate = insertions[j] ;
			bool deja_supprimee = false ;

			for (size_t k = 0 ; k <= i ; ++k) {
				if (suppressions[k] == candidate) {
					deja_supprimee = true ;
					break ;
				}
			}
			if (!deja_supprimee) assert(treeRB_search(arbre, &candidate, int_cmp) != NULL) ;
		}
	}
	assert(arbre == NULL) ;
	printf("\nTEST REUSSI : %s\n", nom) ;
	treeRB_delete(arbre, int_del) ;
}
