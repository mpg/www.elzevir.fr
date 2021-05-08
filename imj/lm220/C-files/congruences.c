/***********************************************/
/*      Systemes de congruences simples        */
/***********************************************/

/* Resoud des systemes de congruences lineaires simples,
 * en utilisant le theoreme chinois et l'inversion modulo n.
 * Etant donne un systeme de la forme :
 *   a*x = b mod n
 *   c*x = d mod m
 * ou :
 * - a est inversible modulo n,
 * - c est inversible modulo m,
 * - m et n sont premiers entre eux,
 * trouve l'unique solution comprise entre 0 et m*n.
 * Si une des conditions ci-dessus n'est pas remplie,
 * renvoie un message d'erreur.
 */

#include <stdio.h>
#include <stdlib.h>

/* codes d'erreurs */
#define RAS 0
#define INV 1
#define CHIN 2
int pb = RAS; /* variable globale pour detection des problemes */

typedef struct {
	int u, v; /* les coefficients d'une relation de Bezout */
        int d;  /* le membre de droite de la relation : le PGCD */
} bezout_t;

/* calcule une relation de Bezout entre a et b */
bezout_t bezout(int a, int b) {
	bezout_t result;
	div_t divis;
	int u, v, u1, v1, u2, v2; /* coefficients u et v aux rangs n, n-1 et n-2 */
         int q1; /* quotient au rang precedent */

	u2 = 1; u1 = 0;
	v2 = 0; v1 = 1;
	while (b != 0) {
		divis = div(a, b);
		q1 = divis.quot;
		u = u2 - q1 * u1;
		v = v2 - q1 * v1;
		/* on a fini les calculs, on decale pour preparer le prochain cycle */
		u2 = u1; v2 = v1;
		u1 = u; v1 = v;
		a = b; b = divis.rem;
	}
	result.u = u2; /* resultats decales, on est alle un cran trop loin */
	result.v = v2; /* avant de se rendre compte qu'on avait deja fini... */
	result.d = a;
	if (result.d < 0) { /* On veut absolument une relation avec 1, pas -1 ! */
		result.u = - result.u; result.v = - result.v;
		result.d = - result.d;
	}
	return result;
}

/* renvoie le representant compris entre 0 et n-1 de a modulo n */
int mod(int a, int n) {
	a = a % n; /* fonction modulo standard */
	if (a < 0) /* le probleme est qu'on est pas sur de son comportement quant au signe */
		a += n;
	return a;
}

/* calcul l'inverse de a modulo n s'il existe, signale le probleme sinon */
int inverse_mod(int a, int n) {
	bezout_t bez;

	bez = bezout(a, n);
	if (bez.d != 1)
		pb = INV;
	return bez.u;
}

/* met en oeuvre le theoreme chinois simple si son hypothese est verifie, le signale sinon */
int chinois(int a, int n, int b, int m) {
	bezout_t bez;
	int x;

	bez = bezout(n, m);
	if (bez.d != 1)
		pb = CHIN;
	x = b * bez.u * n + a * bez.v * m;
	return x;
}

int systeme(int a, int b, int n, int c, int d, int m) {
	int x;

	b = b * inverse_mod(a, n);
	d = d * inverse_mod(c, m);
	if (pb == RAS)
		x = chinois(b, n, d, m);
	return mod(x, m * n);
}

int main(void) {
	int a, b, c, d, m, n, x;

	printf("a = "); scanf("%d", &a);
	printf("b = "); scanf("%d", &b);
	printf("n = "); scanf("%d", &n);
	printf("c = "); scanf("%d", &c);
	printf("d = "); scanf("%d", &d);
	printf("m = "); scanf("%d", &m);
	x = systeme(a, b, n, c, d, m);
	switch (pb) {
		case RAS :
			printf("Une solution est %d ; elle est unique modulo %d\n", x, m * n);
			break;
		case INV :
			printf("Probleme pour inverser a ou b : resoudre d'abord chaque equation avec lineaire-plus\n");
			break;
		case CHIN :
			printf("Je ne sais pas traiter ce genre de systeme : voir chinois-plus\n");
			break;
		default: printf("Il s'est passe quelque chose de vraiment bizarre :(\n");
	}

	return 0;
}
