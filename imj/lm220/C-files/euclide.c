/************************************************************/
/*       Variations sur l'algorithme d'Euclide etendu       */
/************************************************************/

#define MAX 50 /* un majorant du nombre d'iterations possibles dans l'algorithme d'Euclide */
/* La valeur ci-dessus suppose que les entiers sont codes sur 32 bits sur votre machine.
 * S'ils sont codes sur 64 bits, et pour tester l'algorithme sur de "grands" entiers,
 * remplacer 50 par 100 ci-dessus (en effet, la complexite d'Euclide est lineaire). */

#define DIV div /* division euclidienne de la bibliotheque standard */
/* La division euclidienne de la bibliotheque standard renvoie un reste
 * de meme signe que le dividende, et inferieur en valeur absolue au diviseur.
 * On peut utiliser une autre convention pour le reste en decidant qu'il soit
 * plus petit en valeur absolue que la moitie du diviseur.
 * C'est ce qu'on appelle division euclidienne a reste centre.
 * En moyenne, l'algorithme d'Euclide converge en moins d'iterations avec cette division.
 * Pour utiliser la division avec reste centre plutot que la division standard,
 * decommenter la ligne suivante (et commenter la precedente pour eviter un avertissement). */
/* #define DIV div_centre */

#include <stdio.h>
#include <stdlib.h>

/* rappel, defini dans stdlib.h :
 * typedef struct {
 *         int quot; (quotient)
 *         int rem; (reste)
 * } div_t
 * (type renvoye par la fonction de division euclidienne) */

typedef struct  {
	int u;
	int v;
	int d;
} bezout_t;

/* variables globales, pas tres propre en general, mais pratique ici */
int r[MAX], q[MAX], u[MAX], v[MAX]; /* les suites obtenues par l'algo d'Euclide etendu */
int n; /* nombre d'iterations de l'alog, et rang du dernier reste non nul */
int calculs_faits = 0; /* dit si les valeurs de r[], q[], u[], v[] ont deja un sens ou pas */


/********************************/
/*    fontions mathematiques    */
/********************************/

div_t div_centre(int num, int den) { /* division euclidienne a reste centre */
	div_t result;

	result = div(num, den); /* division normale pour commencer */
	if  (result.rem > (den / 2))  result.rem -= den; /* si le reste est trop grand, on le recentre */
	return result;
}

void euclide_complet(int a, int b) {
	div_t divis;

	q[0] = 0; /* inutile, juste pour eviter une valeur aleatoire */
	r[0] = a; r[1] = b;
	u[0] = v[1] = 1;
	u[1] = v[0] = 0;
	n = 0;
	while (r[n+1] != 0) {
		n++;
		divis = DIV(r[n-1], r[n]);
		q[n] = divis.quot;
		r[n+1] = divis.rem;
		u[n+1] = u[n-1] - (u[n] * q[n]);
		v[n+1] = v[n-1] - (v[n] * q[n]);
	} /* n est maintenant le rang du dernier reste non nul */
	calculs_faits = 1;
}

bezout_t bezout(int a, int b) {
	bezout_t result;

	if (!calculs_faits)
		euclide_complet(a, b);
	result.u = u[n];
	result.v = v[n];
	result.d = r[n];
	return result;
}

int pgcd(int a, int b) {
	if (!calculs_faits)
		euclide_complet(a, b);
	return r[n];
}

/********************************/
/*       fonction mixte         */
/********************************/

void resoudre(int a, int b, int c, bezout_t rel) {
	div_t divis;
	int x_part, y_part;
	int facteur_a, facteur_b, facteur_c;

	divis = div(c, rel.d); facteur_c = divis.quot;
	if (divis.rem != 0) {
		printf("Ce dernier ne divise pas %d, donc l'equation\n", c);
		printf("\t%d * x + %d * y = %d\n", a, b, c);
		printf("n'a pas de solutions avec x et y entiers.\n");
	}
	else {
		printf("En multipliant la relation de Bezout obtenue par %d,\n", facteur_c);
		printf("on voit qu'une solution particuliere est :\n");

		x_part = rel.u * facteur_c;
		y_part = rel.v * facteur_c;

		printf("\tx = %d \ty = %d\n", x_part, y_part);

		facteur_a = b / rel.d;
		facteur_b = a / rel.d;

		printf("Comme PGCD(%d, %d) = %d = %d * %d = %d * %d\n", a, b, a * b / rel.d, a, facteur_a, b, facteur_b);
		printf("la solution generale est :\n");
		printf("\tx = %d + k * %d \ty = %d + k * %d \t avec k dans Z\n", x_part, facteur_a, y_part, facteur_b);
	}
}


/*****************************/
/*   fonctions d'affichage   */
/*****************************/

void aff_euclide(int a, int b) {
	int i;

	printf("\nAppliquons l'algorithme d'Euclide etendu a %d et %d :\n\n", a, b);

	printf("i\t");
	for (i = 0; i <= n; i++)
		printf("%10d", i);
	printf("\n");

	printf("q(i)\t");
	for (i = 0; i <= n; i++)
		printf("%10d", q[i]);
	printf("\n");

	printf("r(i)\t");
	for (i = 0; i <= n; i++)
		printf("%10d", r[i]);
	printf("\n");

	printf("u(i)\t");
	for (i = 0; i <= n; i++)
		printf("%10d", u[i]);
	printf("\n");

	printf("v(i)\t");
	for (i = 0; i <= n; i++)
		printf("%10d", v[i]);
	printf("\n");
}

void aff_bezout(int a, int b, bezout_t rel) {
	printf("\nUne relation de Bezout entre %d et %d est : \n", a, b);
	printf("%d * %d + %d * %d = %d\n", a, rel.u, b, rel.v, rel.d);
}

void aff_pgcd(int a, int b, int d) {
	printf("\nLe PGCD de %d et %d est %d.\n", a, b, d);
}

int main(void) {
	int a, b, c;
	bezout_t rel;
	char choix;

	printf("Que voulez-vous faire ?\n");
	printf("p : calculer le PGCD de a et b\n");
	printf("b : calculer une relation de Bezout entre a et b\n");
	printf("e : voir les details d'Euclide etendu pour a et b\n");
	printf("r : resoudre l'equation ax + by = c en solutions entieres\n");
	printf("Votre choix : "); scanf("%c", &choix);

	switch (choix) {
		case 'p' :
			printf("a = "); scanf("%d", &a);
			printf("b = "); scanf("%d", &b);
			aff_pgcd(a, b, pgcd(a, b));
			break;
		case 'b' :
			printf("a = "); scanf("%d", &a);
			printf("b = "); scanf("%d", &b);
			aff_bezout(a, b, bezout(a, b));
			break;
		case 'e' :
			printf("a = "); scanf("%d", &a);
			printf("b = "); scanf("%d", &b);
			euclide_complet(a, b);
			aff_euclide(a, b);
			break;
		case 'r' :
			printf("a = "); scanf("%d", &a);
			printf("b = "); scanf("%d", &b);
			printf("c = "); scanf("%d", &c);
			euclide_complet(a, b);
			aff_euclide(a, b);
			rel = bezout(a, b);
			aff_bezout(a, b, rel);
			aff_pgcd(a, b, pgcd(a, b));
			resoudre(a, b, c, rel);
			break;
		default : printf("Je n'ai pas compris votre demande :(\n");
	}

	return 0;
}

