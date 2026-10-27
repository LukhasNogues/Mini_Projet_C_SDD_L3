#ifndef TREE_RB_H_
#define TREE_RB_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum NodeColor {
	RED,
	BLACK,
} NodeColor ;

typedef struct TreeRB TreeRB ;

struct TreeRB {
	TreeRB* left ;
	TreeRB* right ;
	TreeRB* parent ;
	NodeColor color ;
	char data[] ; // Flexible array member
} ;

/**
 * Constructeur et destructeur
 */

extern TreeRB* treeRB_new(void) ;

extern TreeRB* treeRB_create(const void* data, size_t size) ;

extern void treeRB_delete(TreeRB* self, void (*data_del)(void*)) ;



/**
 * Getters
 */

extern TreeRB* treeRB_get_left(const TreeRB* self) ;

extern TreeRB* treeRB_get_right(const TreeRB* self) ;

extern TreeRB* treeRB_get_parent(const TreeRB* self) ;

extern NodeColor treeRB_get_color(const TreeRB* self) ;

extern char* treeRB_get_color_name(const TreeRB* self) ;

extern void* treeRB_get_data(const TreeRB* self) ;

extern TreeRB* treeRB_get_grandparent(const TreeRB* self) ;

extern TreeRB* treeRB_get_uncle(const TreeRB* self) ;

/**
 * Setters
 */

extern bool treeRB_set_left(TreeRB* self, TreeRB* left) ;

extern bool treeRB_set_right(TreeRB* self, TreeRB* right) ;

extern bool treeRB_set_parent(TreeRB* self, TreeRB* parent) ;

extern bool treeRB_set_color(TreeRB* self, NodeColor color) ;

extern bool treeRB_set_data(TreeRB* self, const void* data, size_t size) ;

/**
 * Parcours d'arbre
 */

extern void treeRB_pre_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) ;

extern void treeRB_in_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) ;

extern void treeRB_post_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) ;

/**
 * Hauteur et cardinal de l'arbre rouge-noir
 */

extern size_t treeRB_height(const TreeRB* self) ;

extern size_t treeRB_size(const TreeRB* self) ;

/**
 * FONCTION D'INSERTION : ajouter la donnée data dans l'arbre rouge-noir
 */

extern bool treeRB_add_data(
	TreeRB** root,
	const void* data,
	size_t size,
	int (*data_cmp)(const void*, const void*)
) ;

/**
 * FONCTION DE SUPPRESSION : supprimer data de l'arbre rouge-noir.
 */

extern bool treeRB_del_data(
	TreeRB** root,
	const void* data,
	int (*data_cmp)(const void*, const void*),
	void (*data_del)(void*)
) ;

/**
 * FONCTION DE RECHERCHE.
 */

extern const TreeRB* /* void* */ treeRB_search(const TreeRB* self, const void* data, int (*cmp)(const void*, const void*)) ;

/**
 * FONCTION POUR L'AFFICHAGE DANS LA CONSOLE (stdout)
 */

extern void treeRB_print(const TreeRB* self, void (*data_print)(const void*)) ;

/**
 * FONCTION POUR VÉRIFIER LA COHÉRENCE DE L'ARBRE
 */

extern bool treeRB_check_black_height(const TreeRB* self) ;

#endif // TREE_RB_H_
