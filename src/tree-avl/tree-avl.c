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


bool treeAVL_add_data(
	TreeAVL* ptree,
	const void* data,
	size_t size,
	int (*compare)(const void*, const void*)
) {
	if (!ptree || !compare) return false ; // Cas où ça ne pointe sur aucun arbre

	// (I) Préparation du noeud contenant la donnée à insérer
	TreeAVL new_node = treeAVL_create(data, size) ;
	if (!new_node) return false ;

	// Cas où ça pointe sur un arbre/noeud vide
	// -> insertion de data = création d'un noeud composé de data
	if (!*ptree) {
		*ptree = new_node ;
		return true ;
	}

	// Sinon : au moins un noeud non NULL : on part de la racine, et on cherche où insérer
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
	//	(II) ÉTABLISSEMENT DES LIENS DE PARENTÉ ENTRE PARENT_OF_NEW_NODE ET NEW_NODE
	//		POUR POUVOIR REMONTER ENSUITE À PARTIR DE NEW_NODE (MAINTENANT QU'ON A
	//		DÉTERMINÉ SON PARENT AVEC LE WHILE PRÉCÉDENT)
	new_node->parent = parent_of_new_node ;
	if (cmp < 0) // le dernier du while
		parent_of_new_node->left = new_node ;
	else
		parent_of_new_node->right = new_node ; // Tolérance des doublons

	// MISE À JOUR RAPIDE DES ÉQUILIBRES DES NOEUDS PARCOURUS (SEULS CEUX LÀ SONT CONCERNÉS)
	// APRÈS L'INSERTION, PENDANT LA REMONTÉE
	TreeAVL p = parent_of_new_node ;
	TreeAVL c = new_node ;
	bool is_left = (c == p->left) ;
	bool is_right = (c == p->right) ;

	while (p) {
		if (is_left) p->balance-- ;
		else if (is_right) p->balance++ ;

		// On ne continue à remonter que s'il y a un déséquilibre par rapport à avant l'insertion
		if (p->balance == 1 || p->balance == -1) {
			c = p ;
			p = p->parent ;
			if (p) {
				is_left = (c == p->left) ;
				is_right = (c == p->right) ;
			}
			continue ; // passe le reste de la boucle pour aller au début du tour suivant
		}
		// Hauteur inchangée par rapport à avant l'insertion : stop.
		else if (p->balance == 0) break ;
		// Déséquilibre à gauche => rotation droite (dans tous les cas).
		else if (p->balance == -2) {
			// Si l'équilibre du fils gauche (s'il existe) est positif : double rotation.
			if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&(p->left)) ;
			_treeAVL_RightRotation(&p) ;
		}
		else if (p->balance == 2) { // Déséquilibre à droite
			// Si l'équilibre du fils droit (s'il existe) est négatif : double rotation.
			if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&(p->right)) ;
			_treeAVL_LeftRotation(&p) ;
		}
		// Après la rotation p pointe désormais sur celui qui a pris la place de l'élément
		// vers lequel p pointait avant la rotation (car la rotation modifie là où pointe p).

		if (p->balance != 0) {
			c = p ;
			p = c->parent ; // p->parent
			if (p) {
				is_left = (c == p->left) ;
				is_right = (c == p->right) ;
			}
			continue ;
		}
		// Sinon, p->balance vaut 0, donc la hauteur n'a pas changé par rapport à avant
		// l'insertion :
		// ce cas se produit TOUJOURS : la rotation a rétabli la hauteur d'avant l'insertion.
		else break ;
	}
	// Maintenant que tout a été rééquilibré, on remonte pour restaurer la racine.
	// On remonte depuis un noeud dont on sait qu'il est non NULL jusqu'à retrouver
	// la nouvelle racine. C'est astucieux et ça évite de devoir gérer séparément
	// le cas où une rotation touche la racine dans le while précédent.
	p = parent_of_new_node ;
	while (p) {
		*ptree = p ; // Récupération au fur et à mesure des ancêtres mis à jour afin d'actualiser la racine.
		p = p->parent ;
	}
	return true ;
}



static TreeAVL _treeAVL_get_leftmost_of_right_side(const TreeAVL node) {
	if (!node || !node->right) return NULL ;

	TreeAVL current = node->right ;
	while (current->left) current = current->left ;
	return current ;
}

static TreeAVL _treeAVL_get_rightmost_of_left_side(const TreeAVL node) {
	if (!node || !node->left) return NULL ;

	TreeAVL current = node->left ;
	while (current->right) current = current->right ;
	return current ;
}


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
	// Plusieurs scénarios :
	// Cas où tgt a 0 fils (feuille) : rien à gauche ET rien à droite.
	if (treeAVL_isLeaf(tgt_node)) { // <=> !tgt->left && !tgt->right
		TreeAVL parent = tgt_node->parent ;
		if (!parent) {
			// Feuille qui n'a pas de parent : c'est la racine !
			// On détruit le noeud alloué, on actualise *root à NULL, et fin de l'histoire.
			treeAVL_delete(tgt_node, data_delete) ;
			*ptree = NULL ;
			return true ;
		}
		else {
			// Si je supprime une feuille, l'équilibre de son parent va forcément changer (++ ou --).
			// On détermine de quel côté tgt est de parent AVANT de le supprimer.
			bool is_left = (tgt_node == parent->left) ;
			bool is_right = (tgt_node == parent->right) ;
			treeAVL_delete(tgt_node, data_delete) ;

			// Attention : parent->left ou parent->right pointe maintenant vers une zone libérée. Donc :
			if (is_left) parent->left = NULL ;
			else if (is_right) parent->right = NULL ;

			// Ensuite : remontée à partir de parent pour mettre à jour les valeurs des équilibres, et
			// rééquilibrer avec des rotations si besoin.
			TreeAVL p = parent ;
			TreeAVL c = NULL ;
			while (p) {
				if (is_left) p->balance++ ;
				else if (is_right) p->balance-- ;

				// Si p->balance est revenu à 0 en perdant son fils, c'est que c'était son unique fils.
				// Donc après suppression la hauteur depuis p a changé, et donc on doit continuer à propager.
				if (p->balance == 0) {
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}

				// Sinon, si p->balance vaut 1 ou -1 après le ++ ou -- c'est qu'il était équilibré des deux côtés,
				// donc avait un autre fils, donc la hauteur depuis p n'a pas changé, donc on s'arrête.
				else if (p->balance == -1 || p->balance == 1) break ;

				// Sinon, si p->balance vaut -2, alors déséquilibre à gauche et il faut immédiatement effectuer
				// une rotation (simple ou double suivant l'équilibre de son fils gauche).
				else if (p->balance == -2) {
					// Déséquilibre à gauche => rotation droite dans tous les cas.
					// Si l'équilibre du fils gauche (s'il existe) est positif : double rotation.
					if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&(p->left)) ;
					_treeAVL_RightRotation(&p) ;
				}
				else if (p->balance == 2) {
					// Déséquilibre à droite => rotation gauche dans tous les cas.
					// Si l'équilibre du fils droit (s'il existe) est négatif : double rotation.
					if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&(p->right)) ;
					_treeAVL_LeftRotation(&p) ;
				}

				// Après la rotation p pointe désormais sur celui qui a pris la place de l'élément
				// vers lequel p pointait avant la rotation (car la rotation modifie là où pointe p).
				if (p->balance == 0) { // Si p->balance vaut 0, la hauteur a changé.
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}
				else break ; // Sinon, p->bal vaut -1 ou 1, donc la hauteur n'a pas changé.
			}
			// On reremonte pour retrouver la racine.
			p = parent ;
			while (p) {
				*ptree = p ; // Récupération au fur et à mesure des ancêtres mis à jour afin d'actualiser la racine.
				p = p->parent ;
			}
			return true ;
		}
	}
	// Cas où tgt a exactement 1 fils (rien à gauche OU EXCLUSIF rien à droite : si on arrive
	// à ce else if, c'est nécessairement un ou exclusif).
	else if (!tgt_node->left || !tgt_node->right) {
		// Si l'élément à enlever n'a qu'un fils, alors ce fils ne peut pas avoir de fils
		// (c'est une feuille), sinon ce ne serait pas un arbre AVL.
		TreeAVL son = (tgt_node->left) ? tgt_node->left : tgt_node->right ;
		TreeAVL parent = tgt_node->parent ;
		bool son_is_left = (son == tgt_node->left) ;
		bool son_is_right = (son == tgt_node->right) ;

		if (!parent) {
			// Si tgt n'a pas de parent : vu l'analyse un peu plus haut, la seule configuration est que cet arbre
			// AVL n'est constitué que de deux éléments : tgt et son noeud fils.
			// Cas facile : son fils le remplace et fin de l'histoire.
			// D'abord, on coupe les liens entre tgt et son fils :
			if (son_is_left) tgt_node->left = NULL ;
			else if (son_is_right) tgt_node->right = NULL ;
			son->parent = NULL ;
			// On supprime tgt :
			treeAVL_delete(tgt_node, data_delete) ;
			// La racine doit pointer sur le fils qui prend alors la place de tgt :
			*ptree = son ;
			return true ;
		}
		else {
			// Pareil, tgt est remplacé par son fils : on coupe les liens entre tgt et son fils et son père.
			bool is_left = (tgt_node == parent->left) ; // Nécessaire pour savoir où placer le fils de tgt
			bool is_right = (tgt_node == parent->right) ; // vu qu'il doit prendre la place de tgt.

			if (son_is_left) tgt_node->left = NULL ;
			else if (son_is_right) tgt_node->right = NULL ; // Ou mettre tout à NULL dans tgt (pour faire simple) plutôt que de séparer en cas.

			tgt_node->parent = NULL ;
			// On détruit tgt :
			treeAVL_delete(tgt_node, data_delete) ;
			// On raccorde le fils de tgt avec le père de tgt :
			son->parent = parent ;
			if (is_left) parent->left = son ;
			else if (is_right) parent->right = son ;

			// Ensuite : remontée à partir de parent pour mettre à jour les valeurs des équilibres.
			TreeAVL p = parent ;
			TreeAVL c = son ; // Peu importe en fait
			while (p) {
				// Si tgt était à gauche de son père : comme on l'enlève, le côté droit pèse plus, donc ++.
				if (is_left) p->balance++ ;
				// Si tgt était à droite de son père : comme on l'enlève, le côté droit pèse moins, donc --.
				else if (is_right) p->balance-- ;

				// Si p->balance est revenu à 0 suite à ce petit calcul, c'est que :
				// avant la suppression un côté était plus haut que l'autre, et que la suppression a alors
				// ramené les deux hauteurs (gauche et droite) à égalité, DONC la hauteur depuis p a changé,
				// et donc on continue de propager le recalcul de l'équilibre aux ancêtres.
				if (p->balance == 0) {
					c = p ;
					p = p->parent ;
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}

				// Sinon, si p->balance vaut 1 ou -1 après le ++ ou -- c'est qu'il était équilibré des deux côtés,
				// donc avait un autre fils, donc la hauteur depuis p n'a pas changé, donc on s'arrête.
				else if (p->balance == -1 || p->balance == 1) break ;

				// Sinon, si p->balance vaut -2, alors déséquilibre à gauche et il faut immédiatement effectuer
				// une rotation (simple ou double suivant l'équilibre de son fils gauche).
				else if (p->balance == -2) {
					// Déséquilibre à gauche => rotation droite dans tous les cas.
					// Si l'équilibre du fils gauche (s'il existe) est positif : double rotation.
					if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&(p->left)) ;
					_treeAVL_RightRotation(&p) ;
				}
				else if (p->balance == 2) {
					// Déséquilibre à droite => rotation gauche dans tous les cas.
					// Si l'équilibre du fils droit (s'il existe) est négatif : double rotation.
					if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&(p->right)) ;
					_treeAVL_LeftRotation(&p) ;
				}

				// Après la rotation p pointe désormais sur celui qui a pris la place de l'élément
				// vers lequel p pointait avant la rotation (car la rotation modifie là où pointe p).
				if (p->balance == 0L) { // Si p->bal vaut 0, la hauteur a changé.
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}
				else break ; // Sinon, p->bal vaut -1 ou 1, donc la hauteur n'a pas changé.
			}
			// On remonte une nouvelle fois depuis parent, pour éventuellement rééquilibrer
			// avec des rotations selon les valeurs des équilibres actualisés.
			p = parent ;
			while (p) {
				// Récupération au fur et à mesure des ancêtres mis à jour afin d'actualiser la racine.
				*ptree = p ;
				p = p->parent ;
			}
			return true ;
		}
	}
	// Cas où tgt a 2 fils : le successeur infixe prend sa place
	// (attention au cas où le successeur infixe est directement le fils droit !)
	else {
		TreeAVL succ = _treeAVL_get_leftmost_of_right_side(tgt_node) ; // On aurait pu prendre le prédécesseur infixe aussi.

		if (succ == tgt_node->right) {
			TreeAVL parent = tgt_node->parent ; // éventuellement NULL
			TreeAVL left = tgt_node->left ; // nécessairement non NULL à ce stade

			bool is_left = false ; // car succ n'est pas à gauche de tgt
			bool is_right = true ; // car succ est à droite de tgt

			// succ prend la place de tgt :
			succ->left = left ; // donc il prend le fils gauche de tgt
			left->parent = succ ; // le fils gauche de tgt change donc de parent pour succ

			succ->parent = parent ; // il prend le parent de tgt
			if (parent && tgt_node == parent->left) parent->left = succ ;
			else if (parent && tgt_node == parent->right) parent->right = succ ;
			succ->balance = tgt_node->balance ; // et TRÈS important pour la suite : il récupère sa valeur d'équilibre.

			// On déconnecte ensuite tgt de ses deux noeuds fils :
			tgt_node->left = tgt_node->right = NULL ;
			// Puis on le supprime
			treeAVL_delete(tgt_node, data_delete) ;
			// Point de départ de la remontée : succ.
			TreeAVL p = succ ;
			TreeAVL c = NULL ; // peu importe
			while (p) {
				if (is_left) // Forcément faux au début, mais c'est après que ça peut se gâter.
					p->balance++ ;
				else if (is_right) // Vrai au début, mais peut être différent au tour d'après.
					p->balance-- ;

				// Même logique que précédemment :
				// balance qui revient à 0 <=> hauteur d'un des deux côtés qui
				// a changé, donc on propage le recalcul des balances.
				if (p->balance == 0) {
					c = p ;
					p = p->parent ;
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}

				else if (p->balance == -1 || p->balance == 1) break ;

				else if (p->balance == -2) {
					if (p->left && p->left->balance > 0L) _treeAVL_LeftRotation(&(p->left)) ;
					_treeAVL_RightRotation(&p) ;
				}
				else if (p->balance == 2) {
					if (p->right && p->right->balance < 0L) _treeAVL_RightRotation(&(p->right)) ;
					_treeAVL_LeftRotation(&p) ;
				}

				if (p->balance == 0) { // Si p->balance vaut 0, la hauteur a changé.
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						is_left = (c == p->left) ;
						is_right = (c == p->right) ;
					}
					continue ;
				}
				else break ; // Sinon, p->balance vaut -1 ou 1, donc la hauteur n'a pas changé.
			}
			p = succ ;
			while (p) {
				*ptree = p ;
				p = p->parent ;
			}
			return true ;
		}
		else {
			// Le point de départ de la remontée (dans le cas où successeur != tgt->rson) :
			TreeAVL succ_parent = succ->parent ;

			// Si l'élément le plus à gauche du côté droit de tgt a un fils,
			// il n'en a qu'un et est forcément à sa droite, donc succ->lson == NULL.
			TreeAVL succ_son = succ->right ; // éventuellement NULL

			// succ prend la place de tgt : il récupère toutes ses propriétés
			// SAUF la valeur de data.
			TreeAVL left = tgt_node->left ;
			TreeAVL right = tgt_node->right ;
			TreeAVL parent = tgt_node->parent ; // éventuellement NULL (si c'est le cas, tgt est la racine)

			succ->left = left ;
			succ->right = right ;
			succ->parent = parent ;
			left->parent = right->parent = succ ;
			if (parent && parent->left == tgt_node) parent->left = succ ;
			else if (parent && parent->right == tgt_node) parent->right = succ ;
			succ->balance = tgt_node->balance ; // Capital

			// Isolement de tgt, pour la suppression.
			tgt_node->left = tgt_node->right = NULL ;
			treeAVL_delete(tgt_node, data_delete) ;

			// Le fils du successeur doit prendre la place du successeur !
			// succ était forcément à gauche de son parent (car succ est par définition le plus à gauche), donc :
			succ_parent->left = succ_son ;
			if (succ_son) succ_son->parent = succ_parent ;

			// Point de départ de la remontée : succ_parent.
			TreeAVL p = succ_parent ;
			TreeAVL c = succ_son ; // Peu importe
			bool was_left = true ; // succ était à gauche de son parent
			bool was_right = false ; // succ n'était pas à droite de son parent
			while (p) {
				if (was_left) p->balance++ ;
				else if (was_right) p->balance-- ;

				if (p->balance == 0) {
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						was_left = (c == p->left) ;
						was_right = (c == p->right) ;
					}
					continue ;
				}

				else if (p->balance == -1 || p->balance == 1) break ;

				else if (p->balance == -2) {
					if (p->left && p->left->balance > 0) _treeAVL_LeftRotation(&(p->left)) ;
					_treeAVL_RightRotation(&p) ;
				}
				else if (p->balance == 2) {
					if (p->right && p->right->balance < 0) _treeAVL_RightRotation(&(p->right)) ;
					_treeAVL_LeftRotation(&p) ;
				}

				if (p->balance == 0) { // Si p->balance vaut 0, la hauteur a changé.
					c = p ;
					p = c->parent ; // p->parent
					if (p) {
						was_left = (c == p->left) ;
						was_right = (c == p->right) ;
					}
					continue ;
				}
				else break ; // Sinon, p->balance vaut -1 ou 1, donc la hauteur n'a pas changé.
			}
			p = succ_parent ;
			while (p) {
				*ptree = p ;
				p = p->parent ;
			}
			return true ;
		}
	}
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
			//P->balance = P->balance - MAX(1 + MAX(bal_4p, 0), -bal_2) + MAX(bal_4, 0) + 1 - MIN(bal_2, 0) ;
			// ou P->balance = P->balance + bal_4p - MIN(bal_4, 0) + MIN(bal_2, -1 - MAX(bal_4p, 0)) ;
		}
		else if (*base_node == P->right) {
			// Sinon, si _4 était à droite de P
			P->right = _2 ; // _2 devient le nouveau fils droit de P
			//P->balance = P->balance + MAX(1 + MAX(bal_4p, 0), -bal_2) - MAX(bal_4, 0) - 1 + MIN(bal_2, 0) ;
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
			//P->balance = P->balance - bal_2p + MAX(bal_2, 0) - MAX(bal_4, 1 - MIN(0, bal_2p)) ;
		}
		else if (*base_node == P->right) {
			P->right = _4 ;
			// P->balance = treeAVL_height(P->right) - treeAVL_height(P->left) ;
			//P->balance = P->balance + bal_2p - MAX(bal_2, 0) + MAX(bal_4, 1 - MIN(0, bal_2p)) ;
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
