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
	TreeAVL tree = malloc(3 * sizeof(TreeAVL) + sizeof(int) + size) ;
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

const void* tree_get_data(const TreeAVL tree) {
	if (tree) return (const void*) tree->data ;
	return NULL ;
}

const TreeAVL treeAVL_get_left(const TreeAVL tree) {
	if (tree) return (const TreeAVL) tree->left ;
	return NULL ;
}

const TreeAVL treeAVL_get_right(const TreeAVL tree) {
	if (tree) return (const TreeAVL) tree->right ;
	return NULL ;
}

const TreeAVL treeAVL_get_parent(const TreeAVL tree) {
	if (tree) return (const TreeAVL) tree->parent ;
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

int treeAVL_height(const TreeAVL tree) {
	if (!tree) return 0 ;
	return 1 + MAX(treeAVL_height(tree->left), treeAVL_height(tree->right)) ;
}

size_t treeAVL_size(const TreeAVL tree) {
	if (!tree) return 0 ;
	return 1 + treeAVL_size(tree->left) + treeAVL_size(tree->right) ;
}

bool treeAVL_isLeaf(const TreeAVL tree) {
	if (!tree) return false ;
	return !tree->left && !tree->right ; // feuille <=> rien à gauche et rien à droite
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

/* Optimisation trop dure à implémenter, abandon
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

	// MISE À JOUR DES BALANCES DES NOEUDS PARCOURUS (SEULS CEUX LÀ SONT CONCERNÉS) APRÈS L'INSERTION, PENDANT LA REMONTÉE
	TreeAVL p = parent_of_new_node ;
	TreeAVL child = new_node ;
	bool rebalance = true ;

	while (p) {
		if (rebalance) {
			if (child == p->left) p->balance-- ;
			else p->balance++ ;

			if (p->balance == 0) rebalance = false ;
			else if (p->balance == -1 || p->balance == 1) ; // La hauteur a augmenté : on continue.
			else {
				if (p->balance == -2) { // déséquilibre à gauche => rotation droite
					if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&p->left) ;
					_treeAVL_RightRotation(&p) ;
				}
				else if (p->balance == 2) { // déséquilibre à droite => rotation gauche
					if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&p->right) ;
					_treeAVL_LeftRotation(&p) ;
				}
				rebalance = false ;
			}
		}
		if (!p->parent) *ptree = p ; // RÉTABLISSEMENT DE LA RACINE D'ORIGINE (dernier parent non NULL)
		child = p ;
		p = p->parent ;
	}
	return true ;
}
*/

static TreeAVL* _treeAVL_get_link(TreeAVL* ptree, TreeAVL node) {
    if (!node->parent) return ptree ;
    if (node->parent->left == node) return &node->parent->left ;
    return &node->parent->right ;
}

static void _treeAVL_rotate_left_insert(TreeAVL* link) {
	TreeAVL x = *link ;
	TreeAVL y = x->right ;
	TreeAVL p = x->parent ;

	x->right = y->left ;
	if (y->left) y->left->parent = x ;
	y->left = x ;
	x->parent = y ;
	y->parent = p ;

	*link = y ;
	// Rotation gauche : balance = hauteur droite - hauteur gauche
	x->balance = 0 ;
	y->balance = 0 ;
}

static void _treeAVL_rotate_right_insert(TreeAVL* link) {
    TreeAVL x = *link ;
    TreeAVL y = x->left ;
    TreeAVL p = x->parent ;

    x->left = y->right ;
    if (y->right) y->right->parent = x ;
    y->right = x ;
    x->parent = y ;
    y->parent = p ;

    *link = y ;
    // Rotation droite : balance = hauteur droite - hauteur gauche
    x->balance = 0 ;
    y->balance = 0 ;
}

static void _treeAVL_rotate_left_right_insert(TreeAVL* link) {
    TreeAVL x = *link ;
    TreeAVL y = x->left ;
    TreeAVL z = y->right ;
    TreeAVL p = x->parent ;

    y->right = z->left ;
    if (z->left) z->left->parent = y ;

    x->left = z->right ;
    if (z->right) z->right->parent = x ;

    z->left = y ;
    z->right = x ;

    y->parent = z ;
    x->parent = z ;
    z->parent = p ;

    *link = z ;

    if (z->balance < 0) {
        x->balance = 1 ;
        y->balance = 0 ;
    } else if (z->balance > 0) {
        x->balance = 0 ;
        y->balance = -1 ;
    } else {
        x->balance = 0 ;
        y->balance = 0 ;
    }
    z->balance = 0 ;
}

static void _treeAVL_rotate_right_left_insert(TreeAVL* link) {
    TreeAVL x = *link ;
    TreeAVL y = x->right ;
    TreeAVL z = y->left ;
    TreeAVL p = x->parent ;

    x->right = z->left ;
    if (z->left) z->left->parent = x ;

    y->left = z->right ;
    if (z->right) z->right->parent = y ;

    z->left = x ;
    z->right = y ;

    x->parent = z ;
    y->parent = z ;
    z->parent = p ;

    *link = z ;

    if (z->balance > 0) {
        x->balance = -1 ;
        y->balance = 0 ;
    } else if (z->balance < 0) {
        x->balance = 0 ;
        y->balance = 1 ;
    } else {
        x->balance = 0 ;
        y->balance = 0 ;
    }
    z->balance = 0 ;
}

bool treeAVL_add_data( // Optimisation made by ChatGPT
	TreeAVL* ptree,
	const void* data,
	size_t size,
	int (*compare)(const void*, const void*)
) {
    if (!ptree || !data || !compare) return false ;

    TreeAVL parent = NULL ;
    TreeAVL current = *ptree ;

    // Recherche de la position d'insertion.
    while (current) {
        int cmp = compare(data, current->data) ;
        //if (cmp == 0) return false ; // Doublon : insertion refusée.
        parent = current ;
        current = (cmp < 0) ? current->left : current->right ;
    }

    // Création du nouveau noeud.
    TreeAVL node = treeAVL_create(data, size) ;
    if (!node) return false ;

    node->parent = parent ;

    if (!parent) {
        *ptree = node ;
        return true ;
    }

    if (compare(data, parent->data) < 0) parent->left = node ;
    else parent->right = node ;

    // Remontée dans l'arbre.
    // balance = hauteur droite - hauteur gauche.
    TreeAVL child = node ;
    TreeAVL x = parent ;

    while (x) {
        if (child == x->left) x->balance-- ;
        else x->balance++ ;

        // La hauteur de ce sous-arbre n'a pas augmenté.
        if (x->balance == 0) break ;

        // Le sous-arbre reste AVL et sa hauteur a augmenté.
        if (x->balance == -1 || x->balance == 1) {
            child = x ;
            x = x->parent ;
            continue ;
        }

        // Déséquilibre à gauche : LL ou LR.
        if (x->balance == -2) {
            TreeAVL* link = _treeAVL_get_link(ptree, x) ;
            if (x->left->balance <= 0) {
                _treeAVL_rotate_right_insert(link) ;
            } else {
                _treeAVL_rotate_left_right_insert(link) ;
            }
            break ;
        }

        // Déséquilibre à droite : RR ou RL.
        if (x->balance == 2) {
            TreeAVL* link = _treeAVL_get_link(ptree, x) ;
            if (x->right->balance >= 0) {
                _treeAVL_rotate_left_insert(link) ;
            } else {
                _treeAVL_rotate_right_left_insert(link) ;
            }
            break ;
        }
        break ;
    }
    return true ;
}



static TreeAVL _treeAVL_get_leftmost_of_right_side(const TreeAVL node) {
	if (!node || !node->right) return NULL ;

	TreeAVL current = node->right ;
	while (current->left) current = current->left ;
	return current ;
}

/*Optimisation trop dure à implémenter, abandon
bool treeAVL_del_data(
	TreeAVL* ptree,
	const void* data,
	void (*data_delete)(void*),
	int (*data_cmp)(const void*, const void*)
) {
	// Vérification de base
	if (!ptree || !*ptree || !data_cmp) return false ;

	// Recherche sans faire appel à search pour éviter de parcourir plusieurs fois l'arbre...
	TreeAVL tgt_node = *ptree ; // Initialisation de la target (cible) : nécessairement non NULL à ce stade
	int cmp ;
	while (tgt_node && (cmp = data_cmp(data, tgt_node->data))) {
		// Deux possibilités pour briser le while :
		// tgt_node == NULL OU (INCLUSIF) cmp == 0
		// si tgt_node vaut NULL, c'est que la donnée n'est pas présente, dans ce cas, rien à faire.
		// si cmp vaut 0, on a trouvé la donnée.
		if (cmp < 0) tgt_node = tgt_node->left ;
		else if (cmp > 0) tgt_node = tgt_node->right ;
	}
	if (!tgt_node) return false ;
	// Sinon, la donnée est là et pointée par tgt_node

	// ======== SUPPRESSION ========
	TreeAVL
	tgtP = tgt_node->parent,
	tgtL = tgt_node->left,
	tgtR = tgt_node->right ;

	TreeAVL p_init = NULL ;
	bool left_shrank = false ;
	int tgtBal = tgt_node->balance ;
	// 8 cas en tout.

	//TreeAVL tgtL_rmost = _treeAVL_get_rightmost_of_left_side(tgt_node) ;

	// Sauvegarde des références avant les déplacements des pointeurs...
	// La sauvegarde doit donc être effectuée ici en anticipation de tout !

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
			// on isole tgt_node pour le supprimer ensuite (rôle du goto) :
			goto node_destroy ;
		}
		else if (!tgtR) { // tgtL prend la place de tgt_node : cas symétrique du précédent
			if (tgtP) {
				if (tgt_node == tgtP->left) tgtP->left = tgtL ;
				else						tgtP->right = tgtL ;
			}
			tgtL->parent = tgtP ;
			// le bi-lien entre tgtP et tgtL étant établi,
			// on isole tgt_node pour le supprimer ensuite (rôle du goto) :
			goto node_destroy ;
		}
	}

	// À ce stade, tgtR ET tgtL sont non NULL.
	// Dichotomie :
	// si tgt_node == tgtP->left :
	//		tgtL < tgt < tgtR < tgtP
	//	=>	tgtL	<	 tgtR < tgtP
	//
	//			 tgtP				 tgtP
	//			/					/
	//		 tgt <---	=>		tgtR
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
	//		 tgt <---	=>		 tgtR
	//		/	\	 ⦙			 /
	//	tgtL	 tgtR		 tgtL
	//
	// (seule exception à la règle "fils droit gagne sur fils gauche", alors que mettre tgtR à la place de tgt ne brise pas l'ordre...)

	// Que tgt_node soit la racine ou non (càd tgtP == NULL ou non), on convient que le plus
	// à gauche de son côté droit, càd son successeur (qui ne peut pas avoir de fils gauche),
	// le remplace.
	TreeAVL succ = _treeAVL_get_leftmost_of_right_side(tgt_node) ;
	if (succ == tgtR) {
		// tgtR prend la place de tgt_node, récupération de son fils gauche :
		succ->left = tgtL ;
		// le fils gauche change de parent :
		tgtL->parent = succ ;

		succ->parent = tgtP ;
		if (tgtP && tgtP->left == tgt_node) tgtP->left = succ ;
		else if (tgtP && tgtP->right == tgt_node) tgtP->right = succ ;
		goto node_destroy ;
	}

	TreeAVL succP = succ->parent ; // seulement si succ != tgtR
	TreeAVL succR = succ->right ; // Si succ a un fils, il est forcément à droite.

	// Le fils de successeur (NULL ou non) remplace successeur.
	succP->left = succR ; // succ était forcément à gauche de son parent
	if (succR) succR->parent = succP ;

	// succ prend ensuite la place de tgt_node
	succ->left = tgtL ;
	succ->right = tgtR ;
	tgtL->parent = tgtR->parent = succ ;

	succ->parent = tgtP ; // qu'il soit NULL ou non
	if (tgtP && tgtP->left == tgt_node) tgtP->left = succ ;
	else if (tgtP && tgtP->right == tgt_node) tgtP->right = succ ;

	node_destroy :
		// isolement de tgt_node avant destruction :
		tgt_node->left = tgt_node->right = NULL ;
		treeAVL_delete(tgt_node, data_delete) ;


	// ======== RÉÉQUILIBRAGE DE L'ARBRE APRÈS LA SUPPRESSION ========
	TreeAVL p_init = NULL ;
	// Si 2 fils :
	if (tgtL && tgtR) {
		if (succ != tgtR) p_init = succP ;
		else p_init = tgtR ;
	}
	// Sinon, si 0 ou 1 noeud fils...
	// ... si parent : départ = lui
	else if (tgtP) p_init = tgtP ;
	// ... sinon : il n'y avait que deux noeuds (tgt_node et son unique noeud fils)
	else p_init = (tgtL) ? tgtL : tgtR ;

	// MISE À JOUR DES BALANCES DES NOEUDS PARCOURUS (SEULS CEUX LÀ SONT CONCERNÉS)
	// APRÈS LA SUPPRESSION, ET PENDANT LA REMONTÉE
	TreeAVL p = p_init ;
	TreeAVL root = NULL ;
	while (p) {
		// ASTUCE : comme les rotations s'annulent entre elles, et qu'elles mettent
		// à jour les balances sans récursion ET en temps constant... autant en profiter !
		_treeAVL_LeftRotation(&p) ;
		_treeAVL_RightRotation(&p) ;

		int bal = p->balance ;
		if (bal < -1) { // déséquilibre à gauche => rotation droite
			if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&p->left) ;
			_treeAVL_RightRotation(&p) ;
		}
		else if (bal > 1) { // déséquilibre à droite => rotation gauche
			if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&p->right) ;
			_treeAVL_LeftRotation(&p) ;
		}
		root = p ; // Après les rotations qui changent p, mais avant le p = p->parent !
		p = p->parent ;
	}

	// ======== RÉTABLISSEMENT DE LA RACINE (MÀJ POINTEUR VERS LA RACINE) ========
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
*/

bool treeAVL_del_data( // Optimisation made by Gemini
    TreeAVL* ptree,
    const void* data,
    void (*data_delete)(void*),
    int (*data_cmp)(const void*, const void*)
) {
	if (!ptree || !*ptree || !data_cmp) return false ;

	TreeAVL node = *ptree ;
	int cmp ;
	while (node && (cmp = data_cmp(data, node->data))) node = (cmp < 0) ? node->left : node->right ;
	if (!node) return false ;

	TreeAVL parent = node->parent ;
	TreeAVL left = node->left ;
	TreeAVL right = node->right ;
	TreeAVL p_init = NULL ;
	bool left_shrank = false ;

	// --- CAS 1 : 0 ou 1 fils ---
	if (!left || !right) {
		TreeAVL child = left ? left : right ;
		left_shrank = parent && (parent->left == node) ;
		if (parent) {
			if (parent->left == node) parent->left = child ;
			else parent->right = child ;
		}
		else *ptree = child ;

		if (child) child->parent = parent ;
		p_init = parent ;
		node->left = node->right = node->parent = NULL ;
		treeAVL_delete(node, data_delete) ;
	}
	// --- CAS 2 : 2 fils ---
	else {
		TreeAVL succ = _treeAVL_get_leftmost_of_right_side(node) ;
		TreeAVL succ_parent = succ->parent ;
		TreeAVL succ_right = succ->right ;

		if (succ == right) {
			succ->left = left ;
			left->parent = succ ;

			succ->parent = parent ;
			if (parent) {
				if (parent->left == node) parent->left = succ ;
				else parent->right = succ ;
			}
			else *ptree = succ ;
			succ->balance = node->balance - 1 ;
			p_init = succ ;
			left_shrank = false ;
		}
		else {
			succ_parent->left = succ_right ;
			if (succ_right) succ_right->parent = succ_parent ;
			succ->left = left ;
			succ->right = right ;
			left->parent = succ ;
			right->parent = succ ;
			succ->parent = parent ;
			if (parent) {
				if (parent->left == node) parent->left = succ ;
				else parent->right = succ ;
			}
			else *ptree = succ ;
			succ->balance = node->balance ;
			p_init = succ_parent ;
			left_shrank = true ;
		}
		node->left = node->right = node->parent = NULL ;
		treeAVL_delete(node, data_delete) ;
	}
	// --- RÉÉQUILIBRAGE ---
	TreeAVL p = p_init ;
	while (p) {
		p->balance += left_shrank ? 1 : -1 ;
		if (p->balance == -1 || p->balance == 1) break ; // La hauteur n'a pas changé globalement
		if (p->balance == 0) {
			TreeAVL child = p ;
			p = p->parent ;
			if (p) left_shrank = (p->left == child) ;
			continue ;
		}
		TreeAVL* link = p->parent
			? ((p->parent->left == p) ? &(p->parent->left) : &(p->parent->right))
			: ptree ;

		if (p->balance == 2) {
			int b = p->right ? p->right->balance : 0 ;
			if (b < 0) _treeAVL_RightRotation(&(p->right)) ;
			_treeAVL_LeftRotation(link) ;
		}
		else if (p->balance == -2) {
			int b = p->left ? p->left->balance : 0 ;
			if (b > 0) _treeAVL_LeftRotation(&(p->left)) ;
			_treeAVL_RightRotation(link) ;
		}
		TreeAVL new_root = *link ;
		if (!new_root) break ;

		if (new_root->balance == 0) {
			p = new_root->parent ;
			if (p) left_shrank = (p->left == new_root) ;
		}
		else break ; // Hauteur stabilisée
	}
	// Sécurité de mise à jour de la racine
	TreeAVL root = *ptree ;
	while (root && root->parent) root = root->parent ;
	if (root) *ptree = root ;
	return true ;
}



const TreeAVL /* void* */ treeAVL_search(
	const TreeAVL tree,
	const void* data,
	int (*compare)(const void*, const void*)
) {
	if (!tree) return NULL ;
	switch (compare(data, tree->data)) {
		case -1 :
			return treeAVL_search(tree->left, data, compare) ;
		case 0 :
			return tree ; //tree->data ;
		case 1 :
			return treeAVL_search(tree->right, data, compare) ;
		default :
			return NULL ; // Rajout sinon warning
	}
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


static void _treeAVL_show_level(const TreeAVL node, int level, int target_level, int spaces, void (*data_print)(const void*)) {
    if (level == target_level) {
        if (node) {
			data_print(node->data) ;
			printf("(%s%d)", node->balance < 0 ? "" : "+", node->balance) ;
		}
        else putchar(' ') ;
        for (int i = 0; i < spaces; i++) putchar(' ') ;
        return ;
    }
    _treeAVL_show_level(node ? node->left : NULL, level + 1, target_level, spaces, data_print) ;
    _treeAVL_show_level(node ? node->right : NULL, level + 1, target_level, spaces, data_print) ;
}

void treeAVL_show(const TreeAVL tree, void (*data_print)(const void*)) {
    if (!tree) {
		puts("(arbre vide)") ;
		return ;
    }

    int height = treeAVL_height(tree) ;

    for (int level = 0 ; level < height ; ++level) {
        int spaces_before = (1 << (height - level)) - 1 ;
        int spaces_between = (1 << (height - level + 1)) - 1 ;

        for (int i = 0 ; i < spaces_before ; ++i) putchar(' ') ;
        _treeAVL_show_level(tree, 0, level, spaces_between, data_print) ;
        putchar(10) ;
    }
}
