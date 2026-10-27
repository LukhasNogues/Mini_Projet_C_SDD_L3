Détail et preuve des calculs présents dans les fonctions de rotation.

Non détaillé sur les figures, mais 1, 3 et 5 ne sont pas nécessairement des feuilles.
H : hauteur.
B : équilibre (balance).



Avant rotation droite :
			[P]
		|		\
		4		[F]
	/		\
	2		5
/		\
1		3

Après rotation droite :
	[P' = P]
	|		\
	2'		[F' = F]
/		\
1'		4'
	/		\
	3'		5'

B(2) = H(3) - H(1) [***]
B(4) = H(5) - H(2)
et H(2) = 1 + max({ H(1) ; H(3) })

Si 4 = fils droit de P	: B(P) = H(4) - H(F)
Si 4 = fils gauche de P	: B(P) = H(F) - H(4)


B(4')	= H(5') - H(3')
	= H(5) - H(3) [******]
	= H(5) - H(2) + H(2) - H(3)
	= B(4)		  + H(2) - H(3)
	= B(4) + 1 + max({ H(1) ; H(3) }) - H(3)
	= B(4) + 1 + max({ H(1) - H(3) ; H(3) - H(3) })	// par "linéarité" du max => compatibilité avec la translation => par translation du max
	= B(4) + 1 + max({ H(1) - H(3) ; 0 })
	= B(4) + 1 + max({ -B(2) ; 0 })			// H(1) - H(3) = - (H(3) - H(1)) = - B(2) cf [***]
	= B(4) + 1 - min({  B(2) ; 0 })			// (d'après le lemme plus bas)

B(2')	= H(4') - H(1')
	= H(4')											- H(1)
	= 1 + max({ H(5') ; H(3') })					- H(1)
	= 1 + max({ H(5) ; H(3) })						- H(1)
	= 1 + max({ H(5) - H(3) + H(3) ; 0 + H(3) })	- H(1)
	= 1 + max({ H(5) - H(3) ; 0 }) + H(3)			- H(1)	// par "linéarité" du max
	= 1 + max({ B(4') ; 0 }) + B(2)							// d'après [******] et [***]
	= B(2) + 1 + max({ B(4') ; 0 })

Après rotation, l'autre fils de P reste inchangé.

Si 2' = fils droit de P' = P, c'est que 4 était fils droit de P		: B(P') = H(2') - H(F' = F) (i)
Si 2' = fils gauche de P' = P, c'est que 4 était fils gauche de P	: B(P') = H(F' = F) - H(2') (ii)

(i) :
	B(P') = H(2') - H(F)
	B(P') - B(P) = H(2') - H(F) - ( H(4) - H(F) )
				= H(2') - H(F) - H(4) + H(F)
				= H(2') - H(4)
				= 1 + max({ H(4') ; H(1') }) - 1 - max({ H(5) ; H(2) })
				= max({ H(4') ; H(1) }) - max({ H(5) ; H(2) })			// H(1') = H(1)
				= max({
					1 + max({ H(5) - H(3) ; 0 }) + H(3) ;				// H(4') = ... (cf plus haut)
					H(1)
				}) - max({ H(5) - H(2) + H(2) ; 0 + H(2) })
				= max({
					1 + max({ H(5) - H(3) ; 0 }) + H(3) ;
					H(1) - H(3) + H(3)							// or H(1) - H(3) = -B(2)
				}) - ( max({ H(5) - H(2) ; 0 }) + H(2) )		// or H(5) - H(2) = B(4)
				= max({
					1 + max({ H(5) - H(3) ; 0 }) ;				// H(5) - H(3) = B(4')
					H(1) - H(3)									// or H(1) - H(3) = -B(2)
				}) + H(3) - max({ H(5) - H(2) ; 0 }) - H(2)		// or H(5) - H(2) = B(4)
				= max({
					1 + max({ B(4') ; 0 }) ;				// H(5) - H(3) = B(4')
					-B(2)									// or H(1) - H(3) = -B(2)
				}) + H(3) - max({ B(4) ; 0 }) - H(2)		// or H(5) - H(2) = B(4)
				= max({
					1 + max({ B(4') ; 0 }) ;				// H(5) - H(3) = B(4')
					-B(2)									// or H(1) - H(3) = -B(2)
				}) - max({ B(4) ; 0 }) + H(3) - H(2)		// or H(5) - H(2) = B(4)
				= max({
					1 + max({ B(4') ; 0 }) ;
					-B(2)
				}) - max({ B(4) ; 0 }) + H(3) - 1 - max({ H(1) ; H(3) })	// H(2) = 1 + max({ H(1) ; H(3) })
				= max({
					1 + max({ B(4') ; 0 }) ;
					-B(2)
				}) - max({ B(4) ; 0 }) + H(3) - 1 + min({ -H(1) ; -H(3) })	// (lemme : -max(.) => min(-.))
				= max({
					1 + max({ B(4') ; 0 }) ;
					-B(2)
				}) - max({ B(4) ; 0 }) - 1 + min({ -H(1) ; -H(3) })	+ H(3)
				= max({
					1 + max({ B(4') ; 0 }) ;
					-B(2)
				}) - max({ B(4) ; 0 }) - 1 + min({ H(3) - H(1) ; 0 })	// B(2) = H(3) - H(1)
				= max({ 1 + max({ B(4') ; 0 }) ; -B(2) }) - max({ B(4) ; 0 }) - 1 + min({ B(2) ; 0 })
DONC :
	B(P') = B(P) + max({ 1 + max({ B(4') ; 0 }) ; -B(2) }) - max({ B(4) ; 0 }) - 1 + min({ B(2) ; 0 })

(ii) :
	B(P') = H(F) - H(2')
	B(P') - B(P) = H(F) - H(2') - ( H(F) - H(4) )
				 = H(F) - H(2') - H(F) + H(4)
				 = H(F) - H(F) - H(2') + H(4)
				 = H(4) - H(2')
				 = - ( max({ 1 + max({ B(4') ; 0 }) ; -B(2) }) - max({ B(4) ; 0 }) - 1 + min({ B(2) ; 0 }) )
DONC :
	B(P') = B(P) - max({ 1 + max({ B(4') ; 0 }) ; -B(2) }) + max({ B(4) ; 0 }) + 1 - min({ B(2) ; 0 })



max, min, sup, inf :
	* propriété de translation, par exemple max(A + x) = max(A) + x
	* homogénéité positive : max(k A) = k max(A) pour k >= 0
	Pas d'homogénéité négative : max(k A) = k min(A) pour k < 0


Lemme : Soit A une partie non vide et bornée de ℝ (de cette manière, A admet une borne supérieure et une borne inférieure (cf construction de ℝ par les coupures de Dedekind)).
Alors :
	* l'ensemble -A = {-a | a ∈ A} est borné.
	* sup(-A) = -inf(A) (et inf(-A) = -sup(A)).
	* en particulier si les bornes sont atteintes : max(-A) = -min(A) et min(-A) = -max(A).

Preuve :
* "l'ensemble -A = {-a | a ∈ A} = {x ∈ ℝ | ∃ a ∈ A ; x = -a} est borné."
Soit a_{0}^{-} ∈ -A.
Il existe alors a_{0} ∈ A tel que a_{0}^{-} = - a_{0} (par définition de -A)
Or comme A est borné (notons s_A = sup(A) et i_A = inf(A)), on a :
	i_A <= a_{0} <= s_A
<=>
	-s_A <= -a_{0} <= -i_A
<=>
	-s_A <= a_{0}^{-} <= -i_A

a_{0}^{-} étant pris arbitrairement dans -A, cet encadrement est donc valable pour tous les éléments de -A.

Ainsi -A est minoré (un exemple de minorant est -s_A) et majoré (un exemple de majorant est -i_A), donc -A est borné.
De plus, -A est non vide.
Par conséquent, -A admet une borne supérieure et une borne inférieure.



* "sup(-A) = -inf(A)."
Notons s_{-A} = sup(-A) (et i_{-A} = inf(-A) mais ça n'a pas d'importance ici...)

Nous avons donc :
	∀ x ∈ -A, x <= s_{-A}

Or, nous avons établi plus haut que :
	∀ x ∈ -A, x <= -i_A

Le réel -i_A est donc un majorant de -A, donc est plus grand que le plus petit des majorants de -A, à savoir s_{-A}.
Donc :
	-i_A >= s_{-A}	[XXX]

Pour tout x ∈ A, on a :
	i_A <= x <= s_A

Soit x dans A. Le réel -x étant trivialement dans -A (un élément a de A tel que -x vaut son opposé -a est... a = x), on a :
	i_{-A} <= -x <= s_{-A}
donc :
	-s_{-A} <= x <= -i_{-A}		(*(-1))

Le réel x étant pris arbitrairement dans A, cet encadrement vaut pour tout élément de A.
Par conséquent, -s_{-A} minore A et -i_{-A} majore A.
Donc -s_{-A} est plus petit que le plus grand minorant de A, à savoir i_A :
	-s_{-A} <= i_A

Donc
	s_{-A} >= -i_A
Or on avait déjà :
	-i_A >= s_{-A} (d'après [XXX])
Conclusion :
	s_{-A} = -i_A
c'est-à-dire :
	sup(-A) = -inf(A)
CQFD
