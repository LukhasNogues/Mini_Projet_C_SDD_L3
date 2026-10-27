#include <string.h>
#include "./tree-avl.h"
#include <stdbool.h>
#include "./min-max.h"

/**
 * Constructeur et destructeur
 */

TreeAVL treeAVL_new(void) {
	return NULL ;
}

void treeAVL_delete(TreeAVL tree, void (*delete)(void*)) {
	if (tree) {
		if (delete) delete(tree->data) ;
		treeAVL_delete(tree->left, delete) ;
		treeAVL_delete(tree->right, delete) ;
		// destruction "descendante" => pas de treeAVL_delete(tree->parent, delete) ;
		free(tree) ;
	}
}

TreeAVL treeAVL_create(const void* data, size_t size) {
	TreeAVL tree = malloc(size + 3 * sizeof(TreeAVL) + sizeof(int)) ;
	if (tree) {
		// Recopie de data en mémoire,
		// pas juste une simple réassignation des pointeurs tree->data = data ; <= non
		memcpy(tree->data, data, size) ;
		tree->left = NULL ;
		tree->right = NULL ;
		tree->parent = NULL ;
		tree->balance = 0 ;
	}
	return tree ;
}



/**
 * Getters
 */

void* tree_get_data(const TreeAVL tree) {
	if (tree) return tree->data ;
	return NULL ;
}

TreeAVL treeAVL_get_left(const TreeAVL tree) {
	if (tree) return tree->left ;
	return NULL ;
}

TreeAVL treeAVL_get_right(const TreeAVL tree) {
	if (tree) return tree->right ;
	return NULL ;
}

TreeAVL treeAVL_get_parent(const TreeAVL tree) {
	if (tree) return tree->parent ;
	return NULL ;
}

int treeAVL_get_balance(const TreeAVL tree) {
	if (tree) return tree->balance ; // return treeAVL_height(tree->right) - treeAVL_height(tree->left) ;
	return 0 ;
}



/**
 * Setters
 */

bool treeAVL_set_data(TreeAVL tree, const void *data, size_t size) {
	if (tree) {
		// Idem que ce qui est décrit dans treeAVL_create : recopie de data en mémoire.
		memcpy(tree->data, data, size) ;
		return true ;
	}
	return false ;
}

bool treeAVL_set_left(TreeAVL tree, TreeAVL left) {
	if (tree) {
		tree->left = left ;
		return true ;
	}
	return false ;
}

bool treeAVL_set_right(TreeAVL tree, TreeAVL right) {
	if (tree) {
		tree->right = right ;
		return true ;
	}
	return false ;
}

bool treeAVL_set_parent(TreeAVL tree, TreeAVL parent) {
	if (tree) {
		tree->parent = parent ;
		return true ;
	}
	return false ;
}



/**
 * Autres
 */

void treeAVL_pre_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) {
	if (!tree) return ;
	(void) extra_data ;
	func(tree->data, &tree->balance) ;
	treeAVL_pre_order(tree->left, func, extra_data) ;
	treeAVL_pre_order(tree->right, func, extra_data) ;
}

void treeAVL_in_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) {
	if (!tree) return ;

	(void) extra_data ;
	treeAVL_in_order(tree->left, func, extra_data) ;
	func(tree->data, &tree->balance) ;
	treeAVL_in_order(tree->right, func, extra_data) ;

}

void treeAVL_post_order(TreeAVL tree, void (*func)(void*, void*), void* extra_data) {
	if (!tree) return ;

	(void) extra_data ;
	treeAVL_post_order(tree->left, func, extra_data) ;
	treeAVL_post_order(tree->right, func, extra_data) ;
	func(tree->data, &tree->balance) ;
}

size_t treeAVL_height(TreeAVL tree) {
	if (!tree) return 0 ;
	return 1 + MAX(treeAVL_height(tree->left), treeAVL_height(tree->right)) ;
}

size_t treeAVL_size(TreeAVL tree) {
	if (!tree) return 0 ;
	return 1 + treeAVL_size(tree->left) + treeAVL_size(tree->right) ;
}

bool treeAVL_isLeaf(TreeAVL tree) {
	if (!tree) return false ;
	return !tree->left && !tree->right ; // feuille <=> rien à gauche et rien à droite
}

static void _treeAVL_update_balance(TreeAVL node) {
	if (!node) return ;
	node->balance = treeAVL_height(node->right) - treeAVL_height(node->left) ;
}

// déclarations ici pour éviter l'erreur de déclaration implicite.
static void _treeAVL_RightRotation(TreeAVL* base_node) ;
static void _treeAVL_LeftRotation(TreeAVL* base_node) ;

// https://www.cs.usfca.edu/~galles/visualization/AVLtree.html

// (i) recherche du lieu d'insertion en respectant l'ordre défini dans compare
// (ii) insertion
// (iii) on remonte à partir du noeud inséré (le long du chemin parcouru) jusqu'au noeud parent
//		pour voir si l'insertion a provoqué un déséquilibre
// (iv) éventuellement, on rééquilibre au noeud actuel (qui est le parent de celui qu'on a inséré)
// (v) : on remonte ainsi jusqu'à la racine
bool treeAVL_add_data(
	TreeAVL* ptree,
	const void* data,
	size_t size,
	int (*compare)(const void*, const void*)
) {
	if (!ptree) return false ; // Cas où ça ne pointe sur aucun arbre

	if (!*ptree) { // Cas où ça pointe sur un arbre/noeud vide
		TreeAVL new_tree = treeAVL_create(data, size) ;
		if (!new_tree) return false ;
		*ptree = new_tree ;
		return true ;
	}

	// RECHERCHE DE L'EMPLACEMENT POUR L'INSERTION
	TreeAVL node = *ptree ;
	TreeAVL parent_of_new_node ;
	int cmp ;
	while (node) {
		parent_of_new_node = node ;
		// à la fin du dernier tour : node == NULL & parent == dernier node non NULL 
		cmp = compare(data, node->data) ;
		if (cmp < 0) node = node->left ;
		else node = node->right ;
	}

	// INSERTION
	//	(I) CRÉATION DU NOEUD AVEC LES DONNÉES (similaire au cas if (!*ptree) au début)
	TreeAVL new_node = treeAVL_create(data, size) ;
	if (!new_node) return false ;

	//	(II) ÉTABLISSEMENT DES LIENS DE PARENTÉ ENTRE PARENT_OF_NEW_NODE ET NEW_NODE
	//		POUR POUVOIR REMONTER ENSUITE À PARTIR DE NEW_NODE (MAINTENANT QU'ON A
	//		DÉTERMINÉ SON PARENT AVEC LE WHILE PRÉCÉDENT)
	new_node->parent = parent_of_new_node ;
	if (cmp < 0) // le dernier du while
		parent_of_new_node->left = new_node ;
	else
		parent_of_new_node->right = new_node ;

	// MISE À JOUR DES BALANCES DES NOEUDS PARCOURUS (SEULS CEUX LÀ SONT CONCERNÉS) APRÈS L'INSERTION
	TreeAVL p = parent_of_new_node ;
	while (p) {
		_treeAVL_update_balance(p) ;
		p = p->parent ;
	}

	// REMONTÉE
	p = parent_of_new_node ;
	while (p) {
		int bal = p->balance ;
		if (bal < -1) { // déséquilibre à gauche => rotation droite
			if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&p->left) ;
			_treeAVL_RightRotation(&p) ;
		}
		else if (bal > 1) { // déséquilibre à droite => rotation gauche
			if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&p->right) ;
			_treeAVL_LeftRotation(&p) ;
		}
		p = p->parent ;
	}
	
	// RÉTABLISSEMENT DE LA RACINE D'ORIGINE
	p = parent_of_new_node ;
	TreeAVL root ;
	while (p) {
		root = p ;
		p = p->parent ;
	}
	*ptree = root ; // dernier parent non NULL
	return true ;
}


// Convention ici : lorsqu'on supprime la racine,
// son fils droit gagne sur le fils gauche pour prendre la place.

static TreeAVL _treeAVL_get_rightmost_of_left_side(const TreeAVL node) {
	if (!node || !node->left) return NULL ;

	TreeAVL current = node->left ;
	while (current->right) current = current->right ;
	return current ;
}

static TreeAVL _treeAVL_get_leftmost_of_right_side(const TreeAVL node) {
	if (!node || !node->right) return NULL ;

	TreeAVL current = node->right ;
	while (current->left) current = current->left ;
	return current ;
}

bool treeAVL_del_data(
	TreeAVL* ptree,
	const void* data,
	void (*data_delete)(void*),
	int (*data_cmp)(const void*, const void*)
) {
	// Vérification de base
	if (!ptree || !*ptree) return false ;

	// Recherche sans faire appel à search pour éviter de parcourir plusieurs fois l'arbre...
	TreeAVL tgt_node = *ptree ; // Initialisation de la target (cible) : nécessairement non NULL à ce stade
	int cmp = data_cmp(data, tgt_node->data) ;
	while (tgt_node && cmp) {
		// Deux possibilités pour briser le while :
		// tgt_node == NULL OU (INCLUSIF) cmp == 0
		// si tgt_node vaut NULL, c'est que la donnée n'est pas présente, dans ce cas, rien à faire.
		// si cmp vaut 0, on a trouvé la donnée.
		cmp = data_cmp(data, tgt_node->data) ;
		if (cmp < 0) tgt_node = tgt_node->left ;
		else if (cmp > 0) tgt_node = tgt_node->right ;
	}
	if (!tgt_node) return false ;
	// Sinon, la donnée est là et pointée par tgt_node

	/* ======== SUPPRESSION ======== */
	TreeAVL
	tgtP = tgt_node->parent,
	tgtL = tgt_node->left,
	tgtR = tgt_node->right ;
	// 8 cas en tout.

	// 6 / 8 :
	if (!tgtL || !tgtR) {
		if (!tgtL && !tgtR) {
			if (tgtP) { // tgt_node a un père, mais pas de fils
				if (tgt_node == tgtP->left) tgtP->left = NULL ;
				else tgtP->right = NULL ;
			}
			// dans tous les cas, on supprime le noeud, qu'il soit isolé ou feuille
			goto node_destroy ;
		}
		// Sinon, seul l'un des deux est NULL
		else if (!tgtL) { // tgtR prend la place de tgt_node
			if (tgtP) {
				if (tgt_node == tgtP->left) tgtP->left = tgtR ;
				else						tgtP->right = tgtR ;
			}
			tgtR->parent = tgtP ;
			// le bi-lien entre tgtP et tgtR étant établi,
			// on isole tgt_node pour le supprimer ensuite :

			tgt_node->right = NULL ;
			// avec cette affectation, tgt_node est au pire une feuille :
			// pas besoin de faire tgt_node->parent = NULL ; le treeAVL_delete ne remonte pas.
			goto node_destroy ;
		}
		else if (!tgtR) { // tgtL prend la place de tgt_node : cas symétrique du précédent
			if (tgtP) {
				if (tgt_node == tgtP->left) tgtP->left = tgtL ;
				else						tgtP->right = tgtL ;
			}
			tgtL->parent = tgtP ;
			// le bi-lien entre tgtP et tgtL étant établi,
			// on isole tgt_node pour le supprimer ensuite :

			tgt_node->left = NULL ;
			// avec cette affectation, tgt_node est au pire une feuille :
			// pas besoin de faire tgt_node->parent = NULL ; le treeAVL_delete ne remonte pas.
			goto node_destroy ;
		}
	}

	// À ce stade, tgtR ET tgtL sont non NULL.
	// Dichotomie :
	// si tgt_node == tgtP->left :
	//		tgtL < tgt < tgtR < tgtP
	//	=>	tgtL	<	 tgtR < tgtP
	//
	//			 tgtP				  tgtP
	//			/					 /
	//		 tgt <---	=>		 tgtR
	//		/	\	 ⦙			/
	//	tgtL	 tgtR		tgtL
	//
	//
	//
	// si tgt_node == tgtP->right :
	//		tgtP < tgtL < tgt < tgtR
	//	=>	tgtP < tgtL    <	tgtR
	//
	//	tgtP				tgtP
	//		\					\ 
	//	---> tgt		=>		 tgtL
	//	⦙	/	\					 \ 
	//	tgtL	 tgtR				  tgtR
	//
	// (seule exception à la règle "fils droit gagne sur fils gauche", alors que mettre tgtR à la place de tgt ne brise pas l'ordre...)

	TreeAVL tgtL_rmost_for_rebalance = _treeAVL_get_rightmost_of_left_side(tgt_node) ;
	// Sauvegarde de la référence avant les déplacements des pointeurs...
	// La sauvegarde doit donc être effectuée ici en anticipation.

	if (!tgtP) {
		// Si pas de parent pour tgt_node, c'est que c'est la racine, et on convient que son fils
		// droit le remplace ** APPLICATION DE LA CONVENTION **
		tgtR->parent = NULL ;

		TreeAVL tgtL_rmost = _treeAVL_get_rightmost_of_left_side(tgt_node) ;
		// Par définition, tgtL_rmost->right == NULL, et il nous faut un endroit où caser
		// tgtR->left, car tgtL doit devenir le nouveau tgtR->left.
		// tgtL_rmost->right est garanti d'être libre à droite, et y insérer là le fils gauche de
		// tgtR respecte l'ordre d'un arbre binaire :
		// l'élément le plus à droite de la partie gauche - notons-le D+ - est nécessairement plus
		// petit que n'importe quel élément de la partie droite,
		// donc placer le sous-arbre dont la racine est tgtR->left "en bas" à droite de D+, fait
		// d'une part perdre le statut d'"élément le plus à droite de la moitié gauche",
		// mais d'autre part préserve bien l'ordre.
		tgtL_rmost->right = tgtR->left ;
		if (tgtR->left) tgtR->left->parent = tgtL_rmost ;

		tgtR->left = tgtL ;   // \_
		tgtL->parent = tgtR ; // /- bi-lien entre tgtR et tgtL
	}
	else {
		// tgt_node a un parent
		if (tgt_node == tgtP->left) { // tgtR remplace tgt_node
			tgtP->left = tgtR ;		// \_
			tgtR->parent = tgtP ;	// /- bi-lien entre tgtP et tgtR

			TreeAVL tgtL_rmost = _treeAVL_get_rightmost_of_left_side(tgt_node) ;
			tgtL_rmost->right = tgtR->left ;
			if (tgtR->left) tgtR->left->parent = tgtL_rmost ;

			tgtR->left = tgtL ;   // \_
			tgtL->parent = tgtR ; // /- bi-lien entre tgtR et tgtL
		}
		else { // tgtL remplace tgt_node : symétrique à celui du dessus
			tgtP->right = tgtL ;	// \_
			tgtL->parent = tgtP ;	// /- bi-lien entre tgtP et tgtL

			TreeAVL tgtR_lmost = _treeAVL_get_leftmost_of_right_side(tgt_node) ;
			tgtR_lmost->left = tgtL->right ;
			if (tgtL->right) tgtL->right->parent = tgtR_lmost ;

			tgtL->right = tgtR ;	// \_
			tgtR->parent = tgtL ;	// /- bi-lien entre tgtL et tgtR
		}
	}
	// isolement de tgt_node :
	tgt_node->left = tgt_node->right = NULL ;

	node_destroy :
		treeAVL_delete(tgt_node, data_delete) ;


	/* ======== RÉÉQUILIBRAGE DE L'ARBRE APRÈS LA SUPPRESSION ======== */

	// MISE À JOUR DES BALANCES DES NOEUDS PARCOURUS (SEULS CEUX LÀ SONT CONCERNÉS)
	// APRÈS LA SUPPRESSION
	TreeAVL p_init = (tgtL && tgtR) ? tgtL_rmost_for_rebalance : tgtP ;
	// ** CORRECTION ** du point de départ pour le rééquilibrage :
	// on détermine dans chaque cas à partir de quel(s) noeud(s) il y a du déséquilibre (quels sont
	// les balances qui ont changé par rapport à avant la suppression) => niveau le plus bas où il y
	// a du déséquilibre / niveau le plus haut où l'équilibre est resté le même qu'avant.
	// ** APPLICATION DE LA CONVENTION ** (indirectement) dans le cas où tgtL et tgtR sont valides.
	TreeAVL p = p_init ;
	while (p) {
		_treeAVL_update_balance(p) ;
		p = p->parent ;
	}

	// REMONTÉE
	p = p_init ;
	while (p) {
		int bal = p->balance ;
		if (bal < -1) { // déséquilibre à gauche => rotation droite
			if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&p->left) ;
			_treeAVL_RightRotation(&p) ;
		}
		else if (bal > 1) { // déséquilibre à droite => rotation gauche
			if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&p->right) ;
			_treeAVL_LeftRotation(&p) ;
		}
		p = p->parent ;
	}

	/* ======== RÉTABLISSEMENT DE LA RACINE (MÀJ POINTEUR VERS LA RACINE) ======== */

	p = p_init ;
	TreeAVL root = NULL ;
    while (p) {
		root = p ;
		p = p->parent ;
	}
	*ptree = root ;
	// Si p_init = NULL, alors c'est que plus rien n'a pu remplacer les éléments supprimés,
	// l'arbre est alors vide, donc le while est ignoré et *ptree reçoit NULL : on change
	// directement le TreeAVL passé à la fonction par référence quand on l'appelle :
	// treeAVL_del_data(&A, ...) => à la fin du corps de la fonction, on a "*ptree = root ;".
	// Ça fait directement une réaffectation A = autre_chose, avec A une variable existant
	// en dehors de la fonction.
	// Important, car A n'est pas l'arbre en entier, c'est la référence de la racine, et avec
	// toutes les insertions / suppressions -- dans une structure qui s'auto-rééquilibre --
	// celle-ci est très susceptible (very likely) de changer.
	return true ;
}

void* treeAVL_search(
	TreeAVL tree,
	const void* data,
	int (*compare)(const void*, const void*)
) {
	if (!tree) return NULL ;
	switch (compare(data, tree->data)) {
		case -1 :
			return treeAVL_search(tree->left, data, compare) ;
		case 0 :
			return tree->data ;
		case 1 :
			return treeAVL_search(tree->right, data, compare) ;
		default :
			return NULL ; // Rajout sinon warning
	}
}

static void _set(void* data, void* array) {
	static size_t size ;
	static size_t offset ;
	if (data) {
		memcpy(array + offset, data, size) ;
		offset += size ;
	} else {
		offset = 0 ;
		size = *(size_t*) array ;
	}
}

int treeAVL_sort(
	void *array,
	size_t length,
	size_t size,
	int (*compare)(const void*, const void*)
) {
	size_t i ;
	TreeAVL tree = treeAVL_new() ;
	void* pointer ;

	pointer = array ;
	for (i = 0 ; i < length ; i++) {
		if (treeAVL_add_data(&tree, pointer, size, compare)) pointer += size ;
		else {
			treeAVL_delete(tree, NULL) ;
			return false ;
		}
	}
	_set(NULL, &size) ;
	treeAVL_in_order(tree, _set, array) ;
	treeAVL_delete(tree, NULL) ;
	return true ;
}

/**
 * Rotations
 */



/**
 * (i) AVANT ROTATION DROITE :
 *		  P(>5)		OU		P(<1)
 *		 / \			   / \
 * BASE:4	F(>P)	  (P>)F	  4:BASE
 *	   / \					 / \
 *	  2   5					2	5
 *	 / \				   / \
 *	1	3				  1   3
 *
 * Éventuellement NULL :			1, 3, 5, P (si P NULL, pas de F et 4 est la racine) et F
 * Éventuellement des feuilles :	1, 3, 5 et F
 * Éventuellement des sous-arbres : 1, 3, 5 et F
 *
 * EXÉCUTION DE LA ROTATION DROITE :
 *
 *		  |
 *	  2	  4
 *	 / \   \	(coupure du bi-lien entre [4] et [2] : [4]->left = NULL et [2]->parent = NULL)
 *	1	3	5	(non représenté dans les affectations ci-dessous)
 *
 *		  |
 *	  2   4		(création du bi-lien entre [4] et [3] : [4]->left = [3] et [3]->parent = [4])
 *	 / \ / \
 *	1	3	5
 *
 *		  |
 *	  2   4
 *	 /	 / \	(coupure du lien de [2] vers [3] : [2]->right = NULL)
 *	1	3	5	(non représenté dans les affectations ci-dessous)
 *
 *	  |
 *	  2   4
 *	 /	 / \	(transfert du parent de [4] vers [2] : [2]->parent = [4]->parent
 *	1	3	5	puis [4]->parent = NULL (non représenté dans les affectations plus bas))
 *
 *	  |
 *	  2
 *	 / \
 *	1	4		(création du bi-lien entre [2] et [4] : [2]->right = [4] et [4]->parent = [2])
 *	   / \
 *	  3   5
 *
 *
 * (ii) APRÈS ROTATION DROITE :
 *
 *			P'(>5)		OU		P'(<1)
 *		   / \				   / \
 *NEWBASE:2'  F'(>P')	 (P'>)F'  2':NEWBASE
 *		 / \					 / \
 *		1'	4'					1'  4'
 *		   / \					   / \
 *		  3'  5'				  3'  5'
 *
 */

static void _treeAVL_RightRotation(TreeAVL* base_node) {
	if (!base_node || !*base_node || !(*base_node)->left) return ;

	TreeAVL _2 = (*base_node)->left ;
	// Le nouveau noeud "principal" (2' sur la figure (ii) ci-dessus) est le 2 de la figure (i).

	TreeAVL _3 = (*base_node)->left->right ;
	// Le "fils mobile". ATTENTION : peut être NULL !

	TreeAVL _4 = *base_node ;
	// (Conceptuellement : TreeAVL& _4 = base_node ; du C++ si base_node était passé en TreeAVL&)
	// 4 (fig (i)) devient le noeud droit du nouveau noeud "principal" 2' (ii).

	TreeAVL P = (*base_node)->parent ;
	// Ne pas oublier la gestion du parent pour après (NULL si on est à la racine) !
	// Sauvergarde du parent avant toute modification :
	// pas indispensable, mais alors pour le traitement du parent plus bas, il faudra penser
	// à utiliser _2->parent et non _4->parent pour s'adresser à l'ancêtre original.
	// À ce stade, c'est correct, car le parent de *base_node (_4) est bien l'ancêtre original.

	int bal_4 = (*base_node)->balance ;
	int bal_2 = (*base_node)->left->balance ;

	// [ÉTAPE II]
	_4->left = _3 ;
	if (_3) _3->parent = _4 ;

	// [ÉTAPE IV]
	_2->parent = _4->parent ;

	// [ÉTAPE V]
	_2->right	= _4 ;
	_4->parent	= _2 ;

	int bal_4p	= 1 + bal_4 - MIN(0, bal_2)  ; // B(4') = 1 + B(4) - ...
	int bal_2p	= 1 + bal_2 + MAX(0, bal_4p) ; // B(2') = 1 + B(2) + ...

	_2->balance	= bal_2p ;
	_4->balance	= bal_4p ;

	// Mise à jour des fils (car parent est toujours attaché à l'ancien noeud de base)
	// et de balance du parent :
	if (P) { // Attention au cas où on est à la racine => parent = NULL
		if (*base_node == P->left) {
			// Si _4 était à gauche de P (qui est désormais _2->parent)...
			// (base_node pointe toujours vers _4, mais _4 a changé de parent pour _2,
			// donc *base_node aussi, c'est pourquoi "TreeAVL P = (*base_node)->parent ;"
			// doit être placé plus haut dans le code)
			P->left = _2 ; // 2 devient le nouveau fils gauche de P
			P->balance = P->balance - MAX(1 + MAX(bal_4p, 0), -bal_2) + MAX(bal_4, 0) + 1 - MIN(bal_2, 0) ;
			// ou P->balance = P->balance + bal_4p - MIN(bal_4, 0) + MIN(bal_2, -1 - MAX(bal_4p, 0)) ;
		}	
		else if (*base_node == P->right) {
			// Sinon, si _4 était à droite de P
			P->right = _2 ; // _2 devient le nouveau fils droit de P
			P->balance = P->balance + MAX(1 + MAX(bal_4p, 0), -bal_2) - MAX(bal_4, 0) - 1 + MIN(bal_2, 0) ;
			// ou P->balance = P->balance - bal_4p + MIN(bal_4, 0) - MIN(bal_2, -1 - MAX(bal_4p, 0)) ;
		}
	}
	*base_node = _2 ;
}

// Par symétrie : 2 <-> 4 ET right <-> left

static void _treeAVL_LeftRotation(TreeAVL* base_node) {
	if (!base_node || !*base_node || !(*base_node)->right) return ;

	TreeAVL _4 = (*base_node)->right ;
	TreeAVL _3 = (*base_node)->right->left ;
	TreeAVL _2 = *base_node ;
	TreeAVL P = (*base_node)->parent ;

	int bal_2 = (*base_node)->balance ;
	int bal_4 = (*base_node)->right->balance ;

	_2->right = _3 ; // ou _4->left
	if (_3) _3->parent = _2 ;
	_4->parent = _2->parent ;
	_4->left  = _2 ;
	_2->parent = _4 ;

	int bal_2p = bal_2 - 1 - MAX(bal_4, 0)  ; // B(2') = H(3') - H(1') = H(3) - H(1)
	int bal_4p = bal_4 - 1 + MIN(bal_2p, 0) ; // B(4') = H(5') - H(2')

	_4->balance = bal_4p ;
	_2->balance = bal_2p ;

	if (P) {
		if (*base_node == P->left) {
			P->left = _4 ;
			// P->balance = treeAVL_height(P->right) - treeAVL_height(P->left) ;
			P->balance = P->balance - bal_2p + MAX(bal_2, 0) - MAX(bal_4, 1 - MIN(0, bal_2p)) ;
		}
		else if (*base_node == P->right) {
			P->right = _4 ;
			// P->balance = treeAVL_height(P->right) - treeAVL_height(P->left) ;
			P->balance = P->balance + bal_2p - MAX(bal_2, 0) + MAX(bal_4, 1 - MIN(0, bal_2p)) ;
		}
	}
	*base_node = _4 ;
}



static void _treeAVL_show_level(TreeAVL node, int level, int target_level, int spaces) {
    if (level == target_level) {
        if (node) printf("%d", *(int*) node->data) ;
        else printf(" ") ;
        for (int i = 0; i < spaces; i++) printf(" ") ;
        return ;
    }
    _treeAVL_show_level(node ? node->left : NULL, level + 1, target_level, spaces) ;
    _treeAVL_show_level(node ? node->right : NULL, level + 1, target_level, spaces) ;
}

void treeAVL_show(TreeAVL tree) {
    if (!tree) {
		printf("(arbre vide)\n") ;
		return ;
    }

    int height = treeAVL_height(tree) ;

    for (int level = 0 ; level < height ; ++level) {
        int spaces_before = (1 << (height - level)) - 1 ;
        int spaces_between = (1 << (height - level + 1)) - 1 ;

        for (int i = 0 ; i < spaces_before ; ++i) printf(" ") ;
        _treeAVL_show_level(tree, 0, level, spaces_between) ;
        printf("\n") ;
    }
}
