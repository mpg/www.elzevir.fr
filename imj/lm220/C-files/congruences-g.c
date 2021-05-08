/***********************************************/
/*      Systemes de congruences generaux       */
/***********************************************/

/* Resoud tous les systemes de congruences lineaires
 *   a*x = b mod n
 *   c*x = d mod m
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int existe; /* dit si une solution existe */
	int x; /* une solution si elle existe, valeur non determinee sinon */
	int modulo; /* modulo quoi la solution est unique si elle existe, valeur non determinee sinon */
} sol_t;

typedef struct {
	int u, v; /* les coefficients d'une relation de Bezout */
        int d;  /* le membre de droite de la relation : le PGCD */
} bezout_t;


/************ On reprend deux fonctions de congruences.c ************/

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
/**************** fin de la partie commune avec congruences.c **************/

/* resoud ax = b mod n dans le cas general */
sol_t lineaire_g(int a, int b, int n) {
	sol_t sol;
	bezout_t rel;
	div_t b_sur_d;

	rel = bezout(a, n);
	b_sur_d = div(b, rel.d);
	sol.existe = (b_sur_d.rem == 0);
	if (sol.existe) {
		sol.modulo = n / rel.d;
		sol.x = rel.u * b_sur_d.quot;
		sol.x = mod(sol.x, sol.modulo);
	}
	return sol;
}

/* mise en oeuvre du theoreme chinois complet */
sol_t chinois_g(int a, int n, int b, int m) {
	sol_t sol;
	bezout_t rel;
	int a1, b1; /* coefficients reduits modulo le pgcd */

	rel = bezout(n, m);
	a1 = mod(a, rel.d); b1 = mod(b, rel.d);
	sol.existe = (a1 == b1);
	if (sol.existe) {
		sol.modulo = m * n / rel.d; /* = ppcm(m, n) */
		sol.x = (b * rel.u * n + a * rel.v * m) / rel.d;
	}
	return sol;
}

/* resoud le systeme de congruence annonce au debut */
sol_t systeme_g(int a, int b, int n, int c, int d, int m) {
	sol_t sol1, sol2, sol;

	sol1 = lineaire_g(a, b, n);
	sol2 = lineaire_g(c, d, m);
	if (sol1.existe && sol2.existe)
		sol = chinois_g(sol1.x, sol1.modulo, sol2.x, sol2.modulo);
	else
		sol.existe = 0;
	return sol;
}

int main(void) {
	int a, b, c, d, m, n; /* les coefficients du systeme d'equations comme indique plus haut */
	sol_t sol; /* sa solution */

	printf("a = "); scanf("%d", &a);
	printf("b = "); scanf("%d", &b);
	printf("n = "); scanf("%d", &n);
	printf("c = "); scanf("%d", &c);
	printf("d = "); scanf("%d", &d);
	printf("m = "); scanf("%d", &m);
	sol = systeme_g(a, b, n, c, d, m);

	if (sol.existe)
		printf("Une solution est %d. Elle est unique modulo %d.\n", mod(sol.x, sol.modulo), sol.modulo);
	else
		printf("Ce systeme n'a pas de solution dans Z.\n");

	return 0;
}
