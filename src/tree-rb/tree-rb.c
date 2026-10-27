#include "./min-max.h"
#include "./tree-rb.h"
#include <string.h>
#include <stdio.h>

/**
 * Constructeur et destructeur
 */

TreeRB* treeRB_new(void) {
	return NULL ;
}

TreeRB* treeRB_create(const void* data, size_t size) {
	TreeRB* new_node = malloc(sizeof *new_node + size) ; // avec le flexible array member
	if (!new_node) return NULL ;

	new_node->left = new_node->right = new_node->parent = NULL ;
	new_node->color = RED ; // Sauf la racine qui est noire par définition
	memcpy(new_node->data, data, size) ;

	return new_node ;
}

void treeRB_delete(TreeRB* self, void (*data_del)(void*)) {
	if (!self) return ;

	if (data_del) data_del(self->data) ;
	treeRB_delete(self->left, data_del) ;
	treeRB_delete(self->right, data_del) ;
	// Suppression descendante : pas de treeRB_delete(self->parent, data_del) ;
	free(self) ;
}



/**
 * Getters
 */

TreeRB* treeRB_get_left(const TreeRB* self) {
	if (!self) return NULL ;
	return self->left ;
}

TreeRB* treeRB_get_right(const TreeRB* self) {
	if (!self) return NULL ;
	return self->right ;
}

TreeRB* treeRB_get_parent(const TreeRB* self) {
	if (!self) return NULL ;
	return self->parent ;
}

NodeColor treeRB_get_color(const TreeRB* self) {
	if (!self) return BLACK ; // Noeud NULL : noir par convention
	return self->color ;
}

char* treeRB_get_color_name(const TreeRB* self) {
	if (!self) return NULL ;
	switch (self->color) {
		case RED : { return "RED" ; }
		case BLACK : { return "BLACK" ; }
		default : { return NULL ; }
	}
}

void* treeRB_get_data(const TreeRB* self) {
	if (!self) return NULL ;
	return (void*) self->data ;
}

TreeRB* treeRB_get_grandparent(const TreeRB* self) {
	if (!self || !self->parent) return NULL ;
	return self->parent->parent ;
}

TreeRB* treeRB_get_uncle(const TreeRB* self) {
	if (!self) return NULL ;
	TreeRB* gp = treeRB_get_grandparent(self) ;
	if (!gp) return NULL ;
	if (self->parent == gp->left) return gp->right ;
	return gp->left ;
}

/**
 * Setters
 */

bool treeRB_set_left(TreeRB* self, TreeRB* left) {
	if (!self) return false ;
	self->left = left ;
	return true ;
}

bool treeRB_set_right(TreeRB* self, TreeRB* right) {
	if (!self) return false ;
	self->right = right ;
	return true ;
}

bool treeRB_set_parent(TreeRB* self, TreeRB* parent) {
	if (!self) return false ;
	self->parent = parent ;
	return true ;
}

bool treeRB_set_color(TreeRB* self, NodeColor color) {
	if (!self) return false ;
	self->color = color ;
	return true ;
}

bool treeRB_set_data(TreeRB* self, const void* data, size_t size) {
	if (!self) return false ;
	memcpy(self->data, data, size) ;
	return true ;
}

/**
 * Parcours de l'arbre rouge-noir
 */

void treeRB_pre_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) {
	if (!self || !func) return ;

	func((void*) self->data, extra_data) ;
	treeRB_pre_order(self->left, func, extra_data) ;
	treeRB_pre_order(self->right, func, extra_data) ;
}

void treeRB_in_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) {
	if (!self || !func) return ;

	treeRB_in_order(self->left, func, extra_data) ;
	func((void*) self->data, extra_data) ;
	treeRB_in_order(self->right, func, extra_data) ;
}

void treeRB_post_order(const TreeRB* self, void (*func)(void*, void*), void* extra_data) {
	if (!self || !func) return ;

	treeRB_post_order(self->left, func, extra_data) ;
	treeRB_post_order(self->right, func, extra_data) ;
	func((void*) self->data, extra_data) ;
}

/**
 * Hauteur et cardinal de l'arbre rouge-noir
 */

size_t treeRB_height(const TreeRB* self) {
	if (!self) return 0UL ;
	return 1ul + MAX(treeRB_height(self->left), treeRB_height(self->right)) ;
}

size_t treeRB_size(const TreeRB* self) {
	if (!self) return 0Ul ;
	return 1uL + treeRB_size(self->left) + treeRB_size(self->right) ;	
}

/**
 * FONCTIONS AUXILIAIRES POUR L'INSERTION
 */

static void _treeRB_insert_case2(TreeRB* self) ;
static void _treeRB_insert_case3(TreeRB* self) ;
static void _treeRB_insert_case4(TreeRB* self) ;
static void _treeRB_insert_case5(TreeRB* self) ;

static void _treeRB_rotate_left(TreeRB** base_node) ;
static void _treeRB_rotate_right(TreeRB** base_node) ;

static void _treeRB_insert_case1(TreeRB* self) {
	if (!self) return ;
	if (!self->parent) self->color = BLACK ;
	else _treeRB_insert_case2(self) ;
}

static void _treeRB_insert_case2(TreeRB* self) {
	if (!self || !self->parent || self->parent->color == BLACK) return ;
	_treeRB_insert_case3(self) ;
}

static void _treeRB_insert_case3(TreeRB* self) {
	if (!self || !self->parent) return ;
	TreeRB* uncle = treeRB_get_uncle(self) ;
	TreeRB* gp ;
	if (uncle && uncle->color == RED) {
		self->parent->color = BLACK ;
		uncle->color = BLACK ;
		gp = treeRB_get_grandparent(self) ;
		if (!gp) return ;
		gp->color = RED ;
		_treeRB_insert_case1(gp) ;
	}
	else _treeRB_insert_case4(self) ;
}

static void _treeRB_insert_case4(TreeRB* self) {
	if (!self || !self->parent) return ;
	TreeRB* gp = treeRB_get_grandparent(self) ;
	if (!gp) return ;

	if (self == self->parent->right && self->parent == gp->left) {
		_treeRB_rotate_left(&gp->left) ; // et non &self->parent !
		self = self->left ;
	}
	else if (self == self->parent->left && self->parent == gp->right) {
		_treeRB_rotate_right(&gp->right) ; // et non &self->parent !
		self = self->right ;
	}
	_treeRB_insert_case5(self) ;
}

static void _treeRB_insert_case5(TreeRB* self) {
	if (!self || !self->parent) return ;
	TreeRB* gp = treeRB_get_grandparent(self) ;
	if (!gp) return ;
	self->parent->color = BLACK ;
	gp->color = RED ;
	if (self == self->parent->left && self->parent == gp->left) _treeRB_rotate_right(&gp) ;
	else if (self == self->parent->right && self->parent == gp->right) _treeRB_rotate_left(&gp) ;
}

static void _treeRB_rotate_left(TreeRB** base_node) {
	if (!base_node || !*base_node || !(*base_node)->right) return ;
	TreeRB* _2 = *base_node ;
	TreeRB* _4 = _2->right ;
	TreeRB* _3 = _4->left ; // peut être NULL
	TreeRB*  P = _2->parent ; // peut être NULL

	_4->parent =  P ;
	_2->parent = _4 ;
	if (_3) _3->parent = _2 ;
	_4->left = _2 ;
	_2->right = _3 ;

	if (P) {
		if (_2 == P->left) P->left = _4 ;
		else if (_2 == P->right) P->right = _4 ;
	}

	*base_node = _4 ;
}

static void _treeRB_rotate_right(TreeRB** base_node) {
	if (!base_node || !*base_node || !(*base_node)->left) return ;
	TreeRB* _4 = *base_node ;
	TreeRB* _2 = _4->left ;
	TreeRB* _3 = _2->right ; // peut être NULL
	TreeRB*  P = _4->parent ; // peut être NULL
	
	_2->parent =  P ;
	_4->parent = _2 ;
	if (_3) _3->parent = _4 ;
	_2->right = _4 ;
	_4->left = _3 ;

	if (P) {
		if (_4 == P->left) P->left = _2 ;
		else if (_4 == P->right) P->right = _2 ;
	}

	*base_node = _2 ;
}

// https://www.cs.usfca.edu/~galles/visualization/RedBlack.html

/**
 * FONCTION D'INSERTION : ajouter la donnée data dans l'arbre rouge-noir
 */

bool treeRB_add_data(
	TreeRB** root,
	const void* data,
	size_t size,
	int (*data_cmp)(const void*, const void*)
) {
	if (!root || !data_cmp) return false ;

	// Préparation du nouveau noeud enveloppant la donnée
	TreeRB* new_node = treeRB_create(data, size) ;
	if (!new_node) return false ;

	if (!*root) {
		new_node->color = BLACK ;
		*root = new_node ;
		return true ;
	}

	// Recherche de la position d'insertion : position de départ = *root
	TreeRB* down_goer = *root ;
	TreeRB* new_node_parent = NULL ;
	while (down_goer) {
		new_node_parent = down_goer ;
		int cmp = data_cmp(data, down_goer->data) ;
		if (cmp < 0) down_goer = down_goer->left ;
		else down_goer = down_goer->right ;
	}
	// Insertion effective
	new_node->parent = new_node_parent ;
	int cmp = data_cmp(data, new_node_parent->data) ;
	if (cmp < 0) new_node_parent->left = new_node ;
	else new_node_parent->right = new_node ;

	// Rééquilibrage avec les appels successifs :
	_treeRB_insert_case1(new_node) ;

	// Actualisation de la racine suite au rééquilibrage : point de départ = new_node
	TreeRB* up_goer = new_node ;
	TreeRB* new_root = NULL ;
	while (up_goer) {
		new_root = up_goer ;
		up_goer = up_goer->parent ;
	}
	*root = new_root ;
	return true ;
}



/**
 * FONCTIONS AUXILIAIRES POUR LA SUPPRESSION
 */

static void _treeRB_delete_rotate_left(TreeRB** root, TreeRB* node) {
	if (!root || !node) return ;

	if (!node->parent) _treeRB_rotate_left(root);
	else if (node == node->parent->left) _treeRB_rotate_left(&node->parent->left) ;
	else if (node == node->parent->right) _treeRB_rotate_left(&node->parent->right) ;
}

static void _treeRB_delete_rotate_right(TreeRB** root, TreeRB* node) {
    if (!root || !node) return ;

    if (!node->parent) _treeRB_rotate_right(root) ;
    else if (node == node->parent->left) _treeRB_rotate_right(&node->parent->left) ;
    else if (node == node->parent->right) _treeRB_rotate_right(&node->parent->right) ;
}

static void _treeRB_delete_case2(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) ;
static void _treeRB_delete_case3(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) ;
static void _treeRB_delete_case4(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) ;
static void _treeRB_delete_case5(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) ;
static void _treeRB_delete_case6(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) ;


static void _treeRB_delete_case1(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root) return ;

	// Cas 1 : le déficit de noirceur atteint la racine.
	if (!parent) {
		if (node) {
			node->color = BLACK ;
			node->parent = NULL ;
		}
		*root = node ;
		return ;
	}
	_treeRB_delete_case2(root, node, parent, node_is_left) ;
}

static void _treeRB_delete_case2(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root || !parent) return ;

	TreeRB* bro = node_is_left ? parent->right : parent->left ;

	// Cas 2 : le frère est rouge.
	if (treeRB_get_color(bro) == RED) {
		bro->color = BLACK ;
		parent->color = RED ;

		if (node_is_left) _treeRB_delete_rotate_left(root, parent) ;
		else _treeRB_delete_rotate_right(root, parent) ;
		// Le frère est maintenant noir : on continue le rééquilibrage.
		_treeRB_delete_case3(root, node, parent, node_is_left) ;
	}
	else _treeRB_delete_case3(root, node, parent, node_is_left) ;
}

static void _treeRB_delete_case3(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root || !parent) return ;

	TreeRB* bro = node_is_left ? parent->right : parent->left ;

	NodeColor col_bro = treeRB_get_color(bro) ;
	NodeColor col_lson_bro = treeRB_get_color(bro ? bro->left : NULL) ;
	NodeColor col_rson_bro = treeRB_get_color(bro ? bro->right : NULL) ;
	NodeColor col_par = parent->color ;
	// Cas 3 : frère noir, parent noir, enfants du frère noirs.
	if (col_bro == BLACK && col_lson_bro == BLACK && col_rson_bro == BLACK && col_par == BLACK) {
		if (bro) bro->color = RED ;
		// Le déficit remonte au parent.
		TreeRB* grandparent = parent->parent ;
		bool parent_is_left = (grandparent && (parent == grandparent->left)) ;

		_treeRB_delete_case1(root, parent, grandparent, parent_is_left) ;
	}
	else _treeRB_delete_case4(root, node, parent, node_is_left) ;
}

static void _treeRB_delete_case4(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root || !parent) return ;

	TreeRB* bro = node_is_left ? parent->right : parent->left ;

	// Cas 4 : frère noir, enfants du frère noirs, et parent rouge.
	NodeColor col_bro = treeRB_get_color(bro) ;
	NodeColor col_lson_bro = treeRB_get_color(bro ? bro->left : NULL) ;
	NodeColor col_rson_bro = treeRB_get_color(bro ? bro->right : NULL) ;
	NodeColor col_par = parent->color ;
	if (col_bro == BLACK && col_lson_bro == BLACK && col_rson_bro == BLACK && col_par == RED) {
		parent->color = BLACK ;
		if (bro) bro->color = RED ;
		return ;
	}
	_treeRB_delete_case5(root, node, parent, node_is_left) ;
}

static void _treeRB_delete_case5(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root || !parent) return ;

	TreeRB* bro = node_is_left ? parent->right : parent->left ;
	if (!bro) { // Frère absent => noir.
		_treeRB_delete_case3(root, node, parent, node_is_left) ;
		return ;
	}

	// Cas 5 : frère noir, son enfant proche est rouge, et son enfant éloigné est noir.
	NodeColor col_bro = bro->color ;
	NodeColor col_lson_bro = treeRB_get_color(bro->left) ;
	NodeColor col_rson_bro = treeRB_get_color(bro->right) ;
	if (node_is_left) {
		if (col_bro == BLACK && col_lson_bro == RED && col_rson_bro == BLACK) {
			bro->left->color = BLACK ;
			bro->color = RED ;
			_treeRB_delete_rotate_right(root, bro) ;
		}
	}
	else { // node_is_right
		if (col_bro == BLACK && col_lson_bro == BLACK && col_rson_bro == RED) {
			bro->right->color = BLACK ;
			bro->color = RED ;
			_treeRB_delete_rotate_left(root, bro) ;
		}
	}
	_treeRB_delete_case6(root, node, parent, node_is_left) ;
}

static void _treeRB_delete_case6(TreeRB** root, TreeRB* node, TreeRB* parent, bool node_is_left) {
	if (!root || !parent) return ;

	TreeRB* bro = node_is_left ? parent->right : parent->left ;

	if (!bro) { // En cas d'arbre invalide ou de cas limite.
		_treeRB_delete_case3(root, node, parent, node_is_left) ;
		return ;
	}

	TreeRB* far_child = node_is_left ? bro->right : bro->left ;

	// Cas 6 : l'enfant éloigné du frère est rouge.
	// Le frère récupère la couleur du parent.
	bro->color = parent->color ;
	parent->color = BLACK ;
	if (far_child) far_child->color = BLACK ;

	// La rotation termine le rééquilibrage.
	if (node_is_left) _treeRB_delete_rotate_left(root, parent) ;
	else _treeRB_delete_rotate_right(root, parent) ;
}

/**
 * FONCTION DE SUPPRESSION : supprimer data de l'arbre rouge-noir.
 */

bool treeRB_del_data(
	TreeRB** root,
	const void* data,
	int (*data_cmp)(const void*, const void*),
	void (*data_del)(void*)
) {
	if (!root || !*root || !data_cmp) return false ;

	TreeRB* tgt = *root ;
	int cmp ;
	while (tgt && (cmp = data_cmp(data, tgt->data))) {
		if (cmp < 0) tgt = tgt->left ;
		else if (cmp > 0) tgt = tgt->right ;
	}
	if (!tgt) return false ; // la donnée n'est pas présente, il n'y a rien à supprimer.
	// Sinon tgt est non NULL, et comme on est sorti du while, cmp vaut nécessairement 0,
	// donc la donnée est là, dans le noeud pointé par tgt.

	// ======== SUPPRESSION EFFECTIVE ========
	// Infos pour la suppression puis le rééquilibrage
	TreeRB* tgt_sub = NULL ;
	TreeRB* tgt_sub_parent = NULL ;
	NodeColor deleted_node_color ;
	bool tgt_sub_is_left = false ;

	TreeRB* tgtl = tgt->left ;
	TreeRB* tgtr = tgt->right ;
	TreeRB* tgtp = tgt->parent ;

	if (!tgtl) { // Pas de fils gauche : le fils droit remplace tgt
		tgt_sub = tgtr ;
		tgt_sub_parent = tgtp ;
		deleted_node_color = tgt->color ;

		if (tgtp) {
			tgt_sub_is_left = (tgt == tgtp->left) ;
			if (tgt == tgtp->left) tgtp->left = tgt_sub ;
			else if (tgt == tgtp->right) tgtp->right = tgt_sub ;
		} else *root = tgt_sub ;

		if (tgt_sub) tgt_sub->parent = tgtp ;
	}
	else if (!tgtr) { // Pas de fils droit : le fils gauche remplace tgt
		tgt_sub = tgtl ;
		tgt_sub_parent = tgtp ;
		deleted_node_color = tgt->color ;

		if (tgtp) {
			tgt_sub_is_left = (tgt == tgtp->left) ;
			if (tgt == tgtp->left) tgtp->left = tgt_sub ;
			else if (tgt == tgtp->right) tgtp->right = tgt_sub ;
		} else *root = tgt_sub ;

		if (tgt_sub) tgt_sub->parent = tgtp ;
	}
	else { // DEUX FILS : on transplante le successeur de tgt
		TreeRB* successor = tgtr ;
		while (successor->left) successor = successor->left ;

		deleted_node_color = successor->color ;
		tgt_sub = successor->right ;

		if (successor->parent == tgt) { // Le successeur est le fils droit de tgt
			tgt_sub_parent = successor ;
			tgt_sub_is_left = false ;
			if (tgt_sub) tgt_sub->parent = successor ;
		}
		else { // Détachement du successeur de son ancienne position
			tgt_sub_parent = successor->parent ;
			tgt_sub_is_left = true ;
			tgt_sub_parent->left = tgt_sub ;

			if (tgt_sub) tgt_sub->parent = tgt_sub_parent ;

			// Le successeur récupère le sous-arbre droit de tgt
			successor->right = tgtr ;
			successor->right->parent = successor ;
		}

		// Le successeur prend la place de tgt
		if (tgtp) {
			if (tgt == tgtp->left) tgtp->left = successor ;
			else if (tgt == tgtp->right) tgtp->right = successor ;
		} else *root = successor ;

		successor->parent = tgtp ;

		successor->left = tgtl ;
		successor->left->parent = successor ;

		// Il conserve la couleur du noeud qui occupait la place de tgt
		successor->color = tgt->color ;
	}

	// Isoler tgt avant de le libérer
	tgt->left = tgt->right = NULL ;
	treeRB_delete(tgt, data_del) ;

	// Rééquilibrage
	if (deleted_node_color == BLACK) {
		if (tgt_sub && tgt_sub->color == RED) {
			// Fils rouge remplace un noeud noir.
			tgt_sub->color = BLACK ;
		}
		else _treeRB_delete_case1(root, tgt_sub, tgt_sub_parent, tgt_sub_is_left) ;
		// Le déficit de noirceur doit être corrigé.
	}

	// Actualisation de la racine

	TreeRB* up_goer = *root ;
	if (up_goer) while (up_goer->parent) up_goer = up_goer->parent ;

	*root = up_goer ;
	return true ;
}



/**
 * FONCTION DE RECHERCHE.
 */

const TreeRB* /* void* */ treeRB_search(const TreeRB* self, const void* data, int (*cmp)(const void*, const void*)) {
	if (!self) return NULL ;

	switch (cmp(data, self->data)) {
		case -1 :
			return treeRB_search(self->left, data, cmp) ;
		case 0 :
			return self ; //(void*) self->data ;
		case 1 :
			return treeRB_search(self->right, data, cmp) ;
		default :
			return NULL ; // Rajout pour éviter le warning
	}
}



/**
 * FONCTIONS POUR L'AFFICHAGE DANS LA CONSOLE (stdout)
 */

static void _treeRB_printLvl(const TreeRB* node, int level, int target_level, int spaces, void (*data_print)(const void*)) {
    if (level == target_level) {
        if (node) {
			if (node->color == RED) {
				fputs("\x1b[1;31mR(", stdout) ;
				data_print(node->data) ;
				fputs(")\x1b[0m", stdout) ;
			}
			else if (node->color == BLACK) {
				fputs("\x1b[1;30mN(", stdout) ;
				data_print(node->data) ;
				fputs(")\x1b[0m", stdout) ;
			}
		}
        else putchar(' ') ;
        for (int i = 0 ; i < spaces ; ++i) putchar(' ') ;
        return ;
    }
    _treeRB_printLvl(node ? node->left : NULL, level + 1, target_level, spaces, data_print) ;
    _treeRB_printLvl(node ? node->right : NULL, level + 1, target_level, spaces, data_print) ;
}

void treeRB_print(const TreeRB* self, void (*data_print)(const void*)) {
    if (!self || !data_print) {
		puts("(arbre vide)") ;
		return ;
    }

    int height = treeRB_height(self) ;

    for (int level = 0 ; level < height ; ++level) {
        int spaces_before = (1 << (height - level)) - 1 ;
        int spaces_between = (1 << (height - level + 1)) - 1 ;

        for (int i = 0 ; i < spaces_before ; ++i) putchar(' ') ;
        _treeRB_printLvl(self, 0, level, spaces_between, data_print) ;
        putchar('\n') ;
    }
}

/**
 * FONCTIONS POUR VÉRIFIER LA COHÉRENCE DE L'ARBRE
 */

static bool _treeRB_black_height(const TreeRB* node, size_t* height) {
    // Une feuille NIL est noire.
    if (!node) {
        *height = 1 ;
        return true ;
    }

    size_t left_height ;
    size_t right_height ;

    // Vérifie récursivement le sous-arbre gauche.
    if (!_treeRB_black_height(node->left, &left_height)) return false ;

    // Vérifie récursivement le sous-arbre droit.
    if (!_treeRB_black_height(node->right, &right_height)) return false ;

    // Les deux sous-arbres doivent avoir la même hauteur noire.
    if (left_height != right_height) return false ;

    // Ajoute 1 si le nœud actuel est noir.
    *height = left_height + (node->color == BLACK ? 1 : 0) ;

    return true ;
}

bool treeRB_check_black_height(const TreeRB* self) {
    size_t height ;
    return _treeRB_black_height(self, &height) ;
}
