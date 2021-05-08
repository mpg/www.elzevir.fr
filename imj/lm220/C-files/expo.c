/****************************************/
/*        Exponentiation rapide         */
/****************************************/

/* Je suppose pour simplifier que votre machine travaille sur 32 bits,
 * ce qui n'a d'ailleurs aucune importance, c'est pour fixer les idees.
 * On calculera modulo 2^32 avec des entiers longs non signes.
 * Le fait de calculer dans Z/2^32Z revient a ne pas se preoccuper des debordements de capacite,
 * et permet de tester de grands exposants sans soucis.
 */

#include <stdio.h>
#include <time.h> /* pour comparer */

typedef unsigned long int Ent; /* element de Z/2^32Z */

Ent expo(Ent x, Ent y) { /* eleve rapidement x a la puissance y */
	Ent x_deux_n; /* vaut x^(2^n) au debut de la n-ieme iteration */
	Ent result; /* contiendra le resultat, et les produits partiels en attendant */

	result = 1;
	x_deux_n = x; /* n=0 */
	while (y != 0) {
		if (y & 01) result *= x_deux_n; /* y & 01 represente le dernier bit de y */
		y >>= 1; /* on jette le bit de y qu'on vient d'utiliser en decalant les bits de 1 vers la droite */
		x_deux_n = x_deux_n * x_deux_n; /* prepare la prochaine iteration */
	}
	return result;
}

Ent puiss(Ent x, Ent y) { /* eleve naivement x a la puissance y */
	Ent result;

	for (result = 1; y > 0; y--)
		result *= x;
	return result;
}

int main(void) {
	Ent a = 723; /* la base, n'a pas d'importance */
	Ent b = 123456; /* faites varier l'exposant pour voir */
	Ent r; /* contiendra a^b */

        clock_t t1, t2; /* des instants */
        double temps;  /* une duree */

        int i; /* il faut mieux repeter le calcul pour mesurer un temps significatif */
	int iter = 1000; /* nombre d'iteration pour la mesure du temps, a regler prudemment */

	/* il faudra commenter cette section pour de grands exposants */
	t1 = clock();
	for(i=0; i <= iter; i++)
		r = puiss(a, b);
	t2 = clock();
        temps = 1000 * ( (double)t2 - t1 ) / CLOCKS_PER_SEC;
	printf("Algo simple : %lu en environ %.0f ms.\n", r, temps);
	/* */

	t1 = clock();
	for(i=0; i <= iter; i++)
		r = expo(a, b);
	t2 = clock();
        temps = 1000 * ( (double)t2 - t1 ) / CLOCKS_PER_SEC;
	printf("Algo rapide : %lu en environ %.0f ms.\n", r, temps);

	return 0;
}

