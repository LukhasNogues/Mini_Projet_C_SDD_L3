#ifndef TREE_AVL_H_
#define TREE_AVL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * Définition de la structure
 */

typedef struct _TreeAVLNode* TreeAVL ;

struct _TreeAVLNode {
	TreeAVL left ;
	TreeAVL right ;
	TreeAVL parent ;
	int balance ; // hauteur fils droit moins hauteur fils gauche
	char data[] ; // Flexible Array Member : peut être n'importe quoi, pas nécessairement un caractère
} ;



/**
 * Fonctions sur la structure
 */

// Constructeur et destructeur

extern TreeAVL treeAVL_new(void) ;

extern void treeAVL_delete(TreeAVL tree, void (*delete)(void*)) ;

extern TreeAVL treeAVL_create(const void* data, size_t size) ;



// Getters

extern const void* tree_get_data(const TreeAVL tree) ;

extern const TreeAVL treeAVL_get_left(const TreeAVL tree) ;

extern const TreeAVL treeAVL_get_right(const TreeAVL tree) ;

extern const TreeAVL treeAVL_get_parent(const TreeAVL tree) ;

extern int treeAVL_get_balance(const TreeAVL tree) ;

// Setters

extern bool treeAVL_set_data(TreeAVL tree, const void* data, size_t size) ;

extern bool treeAVL_set_left(TreeAVL tree, TreeAVL left) ;

extern bool treeAVL_set_right(TreeAVL tree, TreeAVL right) ;

extern bool treeAVL_set_parent(TreeAVL tree, TreeAVL parent) ;

// Autres

extern void treeAVL_pre_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) ;

extern void treeAVL_in_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) ;

extern void treeAVL_post_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) ;

extern int treeAVL_height(const TreeAVL tree) ;

extern size_t treeAVL_size(const TreeAVL tree) ;

extern bool treeAVL_isLeaf(const TreeAVL tree) ;


// INSERTION ÉQUILIBRÉE

extern bool treeAVL_add_data(
	TreeAVL* ptree,
	const void* data,
	size_t size,
	int (*compare)(const void*, const void*)
) ;

extern bool treeAVL_del_data(
	TreeAVL* ptree,
	const void* data,
	void (*data_delete)(void*),
	int (*data_cmp)(const void*, const void*)
) ;

// Retourne le noeud dans lequel est contenue la donnée data recherchée
extern const TreeAVL /* void* */ treeAVL_search(
	const TreeAVL tree,
    const void* data,
    int (*compare)(const void*, const void*)
) ;

//extern void treeAVL_RightRotation(TreeAVL* base_node) ;
//extern void treeAVL_LeftRotation(TreeAVL* base_node) ;

extern void treeAVL_show(const TreeAVL tree, void (*data_print)(const void*)) ;

#endif // TREE_AVL_H_
