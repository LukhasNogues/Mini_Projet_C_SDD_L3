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
	char data[1] ; // peut-être n'importe quoi, pas nécessairement un caractère
	TreeAVL left ;
	TreeAVL right ;
	TreeAVL parent ;
	int balance ; // hauteur fils droit moins hauteur fils gauche
} ;



/**
 * Fonctions sur la structure
 */

// Constructeur et destructeur

extern TreeAVL treeAVL_new() ;

extern void treeAVL_delete(
	TreeAVL tree,
	void (*delete)(void*)
) ;

extern TreeAVL treeAVL_create(const void* data, size_t size) ;



// Getters

extern void* tree_get_data(const TreeAVL tree) ;

extern TreeAVL treeAVL_get_left(const TreeAVL tree) ;

extern TreeAVL treeAVL_get_right(const TreeAVL tree) ;

extern TreeAVL treeAVL_get_parent(const TreeAVL tree) ;

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

extern size_t treeAVL_height(TreeAVL tree) ;

extern size_t treeAVL_size(TreeAVL tree) ;

extern bool treeAVL_isLeaf(TreeAVL tree) ;


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

extern void* treeAVL_search(
	TreeAVL tree,
    const void* data,
    int (*compare)(const void*, const void*)
) ;

extern int treeAVL_sort(
	void* array,
    size_t length,
    size_t size,
    int (*compare)(const void*, const void*)
) ;

//extern void treeAVL_RightRotation(TreeAVL* base_node) ;
//extern void treeAVL_LeftRotation(TreeAVL* base_node) ;

extern void treeAVL_show(const TreeAVL tree) ;

#endif // TREE_AVL_H_
