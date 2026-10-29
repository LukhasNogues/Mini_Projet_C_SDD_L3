#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "./tree-avl.h"

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

#define NMAX 100000

// Entiers stockés sur la pile : rien à faire en terme de suppression (durée de vie
// automatique).

size_t intsize = sizeof(int) ;

int int_cmp(const void* data1, const void* data2) {
	if (!data1 || !data2) return -2 ;

	const int n1 = *(const int*) data1 ;
	const int n2 = *(const int*) data2 ;
	return (n1 > n2) - (n1 < n2) ;
}

void int_del(void* data) {
	(void) data ;
}

void int_print(const void* data) {
	if (!data) return ;

	printf("%d", *(const int*) data) ;
}


// Réels flottants stockés sur la pile : rien à faire en terme de suppression (durée de vie
// automatique).

/*size_t dblsize = sizeof(double) ;

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
}*/


// Chaînes non allouées dynamiquement (ni malloc/calloc, ni strdup) : les chaînes utilisées
// sont celles dans le .rodata (read only data), du texte brut, ou celles dans des char[].

int string_cmp(const void* str1, const void* str2) {
	if (!str1 || !str2) return -2 ;

	int cmp = strcmp((const char*) str1, (const char*) str2) ;
	if (cmp < 0) return -1 ;
	else if (!cmp) return 0 ;
	else return 1 ; // cmp > 0
}

void string_del(void* str) {
	(void) str ;
}

void string_print(const void* str) {
	if (!str) return ;

	printf("\"%s\"", (const char*) str) ;
}



typedef struct DicoEntry {
	char key[9] ;
	char value[13] ;
} DicoEntry ;

// DicoEntry stockés sur la pile (à moins d'un stack overflow dans les tests) :
// rien à faire en terme de suppression (durée de vie automatique).

size_t dicoEntrysize = sizeof(DicoEntry) ;

int dicoEntry_cmp(const void* data1, const void* data2) {
	if (!data1 || !data2) return -2 ;

	const DicoEntry* de1 = (const DicoEntry*) data1 ;
	const DicoEntry* de2 = (const DicoEntry*) data2 ;
	int cmp = strcmp(de1->key, de2->key) ;
	// On convient que la clé (key) seule doit permettre d'identifier
	// l'instance de la structure (l'entrée du dictionnaire) de manière unique.
	// Avoir un dictionnaire (un arbre rouge-noir sera utilisé en tant que dictionnaire
	// pour stocker des entrées) avec les DicoEntries suivantes :
	//	{
	//		"Mot" : "définition_1",
	//		"Mot" : "définition 2",
	//	}
	// doit être impossible.
	if (cmp < 0) return -1 ;
	else if (!cmp) return 0 ;
	return 1 ; // cmp > 0
}

void dicoEntry_del(void* data) {
	(void) data ;
}

void dicoEntry_print(const void* data) {
	if (!data) return ;

	const DicoEntry* de = (const DicoEntry*) data ;
	printf("\"%s\" : \"%s\"", de->key, de->value) ;
}


int rand_int(int min, int max) {
	// méthode du rejet (rejection sampling)
	unsigned int range = (unsigned int) (max - min) + 1 ;
	unsigned int limit = (unsigned int) RAND_MAX + 1 ;

	limit -= limit % range ;
	// On retire les valeurs excédentaires pour que limit soit un multiple de range.
	unsigned int r ;
	do {
		r = (unsigned int) rand() ;
	} while (r >= limit) ; // On rejette les valeurs excédentaires.
	return min + (int) (r % range) ; // On ramène le résultat dans l'intervalle.
	// Puisque le nombre de valeurs acceptées est un multiple de range,
	// chaque résultat possible possède le même nombre de correspondances.
	// Attention à deux limites de cette implémentation :
	// - Elle suppose min <= max et que les calculs ne débordent pas.
	// - Si RAND_MAX == UINT_MAX, le calcul de RAND_MAX + 1 peut déborder.
	// Sur les implémentations courantes, ce n'est généralement pas un problème,
	// mais le code n'est pas universellement sûr.
}

char* rand_str(char* clbuffer, size_t clbuffer_cap, size_t min_len, size_t max_len) {
	// Chaîne aléatoire de longueur aléatoire entre min_len et max_len
	static const char charset[] = "0123456789 abcdefghijklmnopqrstuvwxyz\'ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
	if (!clbuffer || min_len > max_len || clbuffer_cap == 0 || max_len >= clbuffer_cap)
		return NULL ;

	size_t len = rand_int((int) min_len, (int) max_len) ;
	size_t charset_len = sizeof(charset) - 1UL ;

	for (size_t i = 0UL ; i < len ; ++i)
		clbuffer[i] = charset[rand_int(0, (int) charset_len - 1)] ;

	clbuffer[len] = '\0' ;
	return clbuffer ;
}

DicoEntry rand_dicoEntry(void) {
	DicoEntry result ;
	char* rand_key = rand_str(
		(char[9]) {0}, 9, 4, 8
	) ;
	char* rand_value = rand_str(
		(char[13]) {0}, 13, 6, 12
	) ;
	strcpy(result.key, rand_key) ;
	strcpy(result.value, rand_value) ;
	return result ;
}



int main(void) {
	// Pour la simplification de la gestion des erreurs.
	int EXIT_CODE = EXIT_FAILURE ;

	// Initialiser le RNG.
	srand((unsigned int) time(NULL)) ;

	// Déclaration des arbres.
	TreeAVL arbre_entiers ;
	TreeAVL arbre_chaines ;
	TreeAVL arbre_dictionnaire ;



	// Ouverture des fichiers dans lesquels seront renseignés les résultats.
	FILE* treeavl_int_insert = fopen("./TREEAVL_INT_INSERT.txt", "w") ;
	if (!treeavl_int_insert) goto exit_prog ;

	FILE* treeavl_int_search = fopen("./TREEAVL_INT_SEARCH.txt", "w") ;
	if (!treeavl_int_search) goto cleanup_treeavl_int_insert ;

	FILE* treeavl_int_delete = fopen("./TREEAVL_INT_DELETE.txt", "w") ;
	if (!treeavl_int_delete) goto cleanup_treeavl_int_search ;

	fprintf(treeavl_int_insert, "n;T_ms(n)\n") ;
	fprintf(treeavl_int_search, "n;T_ms(n)\n") ;
	fprintf(treeavl_int_delete, "n;T_ms(n)\n") ;

	puts("==== Insertions / recherche / suppressions : arbre AVL d'entiers ====") ;

	for (int n = 100 ; n <= NMAX ; n += 100) {
		// Reset à NULL à chaque début de tour de boucle.
		arbre_entiers = treeAVL_new() ;

		//**
		// * ================ INSERTIONS ================
		// *
		// Génération de n entiers aléatoires avant l'insertion :
		int* rand_int_array = malloc(n * sizeof *rand_int_array) ;
		if (!rand_int_array) goto cleanup_treeavl_int_delete ;
		for (int i = 0 ; i < n ; ++i) rand_int_array[i] = rand_int(0, 10*n) ;

		// Début du chrono avant les n insertions
		clock_t timer_start = clock() ;

		// Les n insertions (100, puis 200, puis 300, ... jusqu'à NMAX)
		for (int k = 1 ; k <= n ; ++k) {
			bool insert_is_successful = treeAVL_add_data(
				&arbre_entiers,
				&(rand_int_array[k - 1]),
				intsize,
				int_cmp
			) ;
			if (!insert_is_successful) goto cleanup_int_then_next_loop_turn ;
		}

		// Fin du chrono après les n insertions
		double elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		double elapsed_in_ms = 1e3 * elapsed_in_s ;

		// Copie du temps écoulé pour les n insertions dans le fichier.
		fprintf(treeavl_int_insert, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ RECHERCHES ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : recherche d'une donnée déjà insérée
				// (mais piochée au hasard dans le tableau des entiers aléatoires générés).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_search(arbre_entiers, &(rand_int_array[rand_i]), int_cmp) ;
			}
			else {
				// Sinon, on recherche une valeur aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_int".
				treeAVL_search(arbre_entiers, &(int) { rand_int(0, 10*n) }, int_cmp) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_int_search, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ SUPPRESSIONS ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : suppression d'une donnée déjà insérée
				// (mais piochée au hasard dans le tableau des entiers aléatoires générés).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_del_data(
					&arbre_entiers,
					&(rand_int_array[rand_i]),
					int_del,
					int_cmp
				) ;
			}
			else {
				// Sinon, on supprime une valeur aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_int".
				treeAVL_del_data(
					&arbre_entiers,
					&(int) { rand_int(0, 10*n) },
					int_del,
					int_cmp
				) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_int_delete, "%d;%.3lf\n", n, elapsed_in_ms) ;

		// free à chaque fin de tour de boucle.
	cleanup_int_then_next_loop_turn :
		free(rand_int_array) ;
		treeAVL_delete(arbre_entiers, int_del) ;
		//printf("%d\n", n) ;
	}



	// Ouverture des fichiers dans lesquels seront renseignés les résultats.
	FILE* treeavl_str_insert = fopen("./TREEAVL_STR_INSERT.txt", "w") ;
	if (!treeavl_str_insert) goto cleanup_treeavl_int_delete ;

	FILE* treeavl_str_search = fopen("./TREEAVL_STR_SEARCH.txt", "w") ;
	if (!treeavl_str_search) goto cleanup_treeavl_str_insert ;

	FILE* treeavl_str_delete = fopen("./TREEAVL_STR_DELETE.txt", "w") ;
	if (!treeavl_str_delete) goto cleanup_treeavl_str_search ;

	fprintf(treeavl_str_insert, "n;T_ms(n)\n") ;
	fprintf(treeavl_str_search, "n;T_ms(n)\n") ;
	fprintf(treeavl_str_delete, "n;T_ms(n)\n") ;

	puts("==== Insertions / recherche / suppressions : arbre AVL de String ====") ;
	
	for (int n = 100 ; n <= NMAX ; n += 100) {
		// Reset à NULL à chaque début de tour de boucle.
		arbre_chaines = treeAVL_new() ;

		//**
		// * ================ INSERTIONS ================
		// *
		// Génération de n chaînes aléatoires de longueur entre 8 et 16 avant l'insertion :
		char (*rand_str_array)[17] = malloc(n * sizeof *rand_str_array) ;
		// Pointeur vers des tableaux de 17 caractères
		if (!rand_str_array) goto cleanup_treeavl_str_delete ;
		for (int i = 0 ; i < n ; ++i)
			rand_str(rand_str_array[i], sizeof rand_str_array[i], 8, 16) ;

		// Début du chrono avant les n insertions
		clock_t timer_start = clock() ;

		// Les n insertions (100, puis 200, puis 300, ... jusqu'à NMAX)
		for (int k = 1 ; k <= n ; ++k) {
			size_t strsize = strlen(rand_str_array[k - 1]) + 1UL ;
			bool insert_is_successful = treeAVL_add_data(
				&arbre_chaines,
				rand_str_array[k - 1],
				strsize,
				string_cmp
			) ;
			if (!insert_is_successful) goto cleanup_str_then_next_loop_turn ;
		}

		// Fin du chrono après les n insertions
		double elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		double elapsed_in_ms = 1e3 * elapsed_in_s ;

		// Copie du temps écoulé pour les n insertions dans le fichier.
		fprintf(treeavl_str_insert, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ RECHERCHES ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : recherche d'une donnée déjà insérée
				// (mais piochée au hasard dans le tableau des chaînes aléatoires générées).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_search(
					arbre_chaines,
					rand_str_array[rand_i],
					string_cmp
				) ;
			}
			else {
				// Sinon, on recherche une chaîne aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_str".
				char* chaine_alea = rand_str((char[17]) { 0 }, 17, 8, 16) ;
				treeAVL_search(
					arbre_chaines,
					chaine_alea,
					string_cmp
				) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_str_search, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ SUPPRESSIONS ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : recherche d'une donnée déjà insérée
				// (mais piochée au hasard dans le tableau des chaînes aléatoires générées).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_del_data(
					&arbre_chaines,
					rand_str_array[rand_i],
					string_del,
					string_cmp
				) ;
			}
			else {
				// Sinon, on supprime une chaîne aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_str".
				char* chaine_alea = rand_str((char[17]) { 0 }, 17, 8, 16) ;
				treeAVL_del_data(
					&arbre_chaines,
					chaine_alea,
					string_del,
					string_cmp
				) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_str_delete, "%d;%.3lf\n", n, elapsed_in_ms) ;

		// free à chaque fin de tour de boucle.
	cleanup_str_then_next_loop_turn :
		free(rand_str_array) ;
		treeAVL_delete(arbre_chaines, string_del) ;
		//printf("%d\n", n) ;
	}



	// Ouverture des fichiers dans lesquels seront renseignés les résultats.
	FILE* treeavl_dicoEntry_insert = fopen("./TREEAVL_DICOENTRY_INSERT.txt", "w") ;
	if (!treeavl_dicoEntry_insert) goto cleanup_treeavl_str_delete ;

	FILE* treeavl_dicoEntry_search = fopen("./TREEAVL_DICOENTRY_SEARCH.txt", "w") ;
	if (!treeavl_dicoEntry_search) goto cleanup_treeavl_dicoEntry_insert ;

	FILE* treeavl_dicoEntry_delete = fopen("./TREEAVL_DICOENTRY_DELETE.txt", "w") ;
	if (!treeavl_dicoEntry_delete) goto cleanup_treeavl_dicoEntry_search ;

	fprintf(treeavl_dicoEntry_insert, "n;T_ms(n)\n") ;
	fprintf(treeavl_dicoEntry_search, "n;T_ms(n)\n") ;
	fprintf(treeavl_dicoEntry_delete, "n;T_ms(n)\n") ;

	puts("==== Insertions / recherche / suppressions : arbre AVL de DicoEntry ====") ;

	for (int n = 100 ; n <= NMAX ; n += 100) {
		// Reset à NULL à chaque début de tour de boucle.
		arbre_dictionnaire = treeAVL_new() ;

		//**
		// * ================ INSERTIONS ================
		// *
		// Génération de n entrées aléatoires avant l'insertion :
		DicoEntry* rand_dicoEntry_array = malloc(n * sizeof *rand_dicoEntry_array) ;
		if (!rand_dicoEntry_array) goto cleanup_treeavl_dicoEntry_delete ;
		for (int i = 0 ; i < n ; ++i) {
			rand_dicoEntry_array[i] = rand_dicoEntry() ;
		}

		// Début du chrono avant les n insertions
		clock_t timer_start = clock() ;

		// Les n insertions (100, puis 200, puis 300, ... jusqu'à NMAX)
		for (int k = 1 ; k <= n ; ++k) {
			bool insert_is_successful = treeAVL_add_data(
				&arbre_dictionnaire,
				&(rand_dicoEntry_array[k - 1]),
				dicoEntrysize,
				dicoEntry_cmp
			) ;
			// copie des pointeurs .key et .value dans le .data du noeud concerné.
			if (!insert_is_successful) goto cleanup_dico_then_next_loop_turn ;
		}

		// Fin du chrono après les n insertions
		double elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		double elapsed_in_ms = 1e3 * elapsed_in_s ;

		// Copie du temps écoulé pour les n insertions dans le fichier.
		fprintf(treeavl_dicoEntry_insert, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ RECHERCHES ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : recherche d'une entrée déjà insérée
				// (mais piochée au hasard dans le tableau des entrées aléatoires générées).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_search(
					arbre_dictionnaire,
					&(rand_dicoEntry_array[rand_i]),
					dicoEntry_cmp
				) ;
			}
			else {
				// Sinon, on recherche une entrée aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_dicoEntry".
				DicoEntry dicoEntry_alea = rand_dicoEntry() ;
				treeAVL_search(
					arbre_dictionnaire,
					&dicoEntry_alea,
					dicoEntry_cmp
				) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_dicoEntry_search, "%d;%.3lf\n", n, elapsed_in_ms) ;

		//**
		// * ================ SUPPRESSIONS ================
		// *
		timer_start = clock() ;
		for (int k = 1 ; k <= n ; ++k) {
			if (k & 1) {
				// Si k est impair : recherche d'une entrée déjà insérée
				// (mais piochée au hasard dans le tableau des entrées aléatoires générées).
				int rand_i = rand_int(0, n - 1) ;
				treeAVL_del_data(
					&arbre_dictionnaire,
					&(rand_dicoEntry_array[rand_i]),
					dicoEntry_del,
					dicoEntry_cmp
				) ;
			}
			else {
				// Sinon, on supprime une entrée aléatoire dans l'arbre,
				// qu'elle y soit ou non, on fait "confiance à rand_dicoEntry".
				DicoEntry dicoEntry_alea = rand_dicoEntry() ;
				treeAVL_del_data(
					&arbre_dictionnaire,
					&dicoEntry_alea,
					dicoEntry_del,
					dicoEntry_cmp
				) ;
			}
		}
		elapsed_in_s = (clock() - timer_start) / (CLOCKS_PER_SEC + 0.) ;
		elapsed_in_ms = 1e3 * elapsed_in_s ;
		fprintf(treeavl_dicoEntry_delete, "%d;%.3lf\n", n, elapsed_in_ms) ;

		// free à chaque fin de tour de boucle.
	cleanup_dico_then_next_loop_turn :
		free(rand_dicoEntry_array) ;
		treeAVL_delete(arbre_dictionnaire, dicoEntry_del) ;
		//printf("%d\n", n) ;
	}



	EXIT_CODE = EXIT_SUCCESS ;

cleanup_treeavl_dicoEntry_delete :
	fclose(treeavl_dicoEntry_delete) ;
cleanup_treeavl_dicoEntry_search :
	fclose(treeavl_dicoEntry_search) ;
cleanup_treeavl_dicoEntry_insert :
	fclose(treeavl_dicoEntry_insert) ;

cleanup_treeavl_str_delete :
	fclose(treeavl_str_delete) ;
cleanup_treeavl_str_search :
	fclose(treeavl_str_search) ;
cleanup_treeavl_str_insert :
	fclose(treeavl_str_insert) ;

cleanup_treeavl_int_delete :
	fclose(treeavl_int_delete) ;
cleanup_treeavl_int_search :
	fclose(treeavl_int_search) ;
cleanup_treeavl_int_insert :
	fclose(treeavl_int_insert) ;

exit_prog :
	return EXIT_CODE ;
}
