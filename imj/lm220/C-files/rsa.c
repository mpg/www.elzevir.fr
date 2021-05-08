/**********************************************************/
/*                       RSA                              */
/*             (Rivest, Shamir, Adleman)                  */
/**********************************************************/

/* DISCLAIMER :
 * Le code ci-dessous est une illustration a but pedagogique uniquement.
 * Il ne constitue PAS une implementation realiste ni utilisable de RSA :
 * de nombreux "details" (notament fonction de remplissage)
 * essentiels a la securite du systeme sont omis.
 */

#include <stdio.h>
#include <time.h>
#include <gmp.h>

/* On utilise ici la bibliotheque pour grands nombres GMP  :
 * les fonctions prefixees gmp en proviennent, le type mpz_t
 * designe un entier multi-precision.
 * Il faut penser a lier avec cette bibliotheque a la compilation.
 * par exemple, avec gcc, utiliser l'option -lgmp
 */

typedef struct {
	mpz_t n; /* le module */
	mpz_t e; /* l'exposant */
} pub_t;

typedef struct { /* on stocke la clef privee sous une forme optimisee pour le decryptage : */
	mpz_t p, q; /* les facteurs du module */
	mpz_t dmp1, dmq1; /* l'exposant de decryptage d, modulo p-1 et modulo q-1 */
	mpz_t iqmp; /* l'inverse de q modulo p */
} priv_t;

#define L 256U /* longueur minimale, et approximative, en bits du module RSA */
#define LO L/8 /* la meme longueur en octets, ie en caracteres */
/* on supposera que les caracteres sont codes sur 8 bits,
 * LO est donc la longueur d'un bloc de message en caracteres.
 * C'est potentiellement si vous etes dans un terminal en UTF-8.
 * Par prudence, n'entrez pas de message avec des caracteres accentues */

/* le type mpz_t etant base sur des allocations dynamiques de memoire,
 * ses instances doivent etre allouees avant utilisation, puis liberees apres.
 * Les 4 fonctions suivantes le font pour les deux types rsa crees. */

void pub_init(pub_t *pub) {
	mpz_init(pub->n);
	mpz_init(pub->e);
}

void priv_init(priv_t *priv) {
	mpz_init(priv->p); mpz_init(priv->q);
	mpz_init(priv->dmp1); mpz_init(priv->dmq1);
	mpz_init(priv->iqmp);
}

void pub_clear(pub_t *pub) {
	mpz_clear(pub->n);
	mpz_clear(pub->e);
}

void priv_clear(priv_t *priv) {
	mpz_clear(priv->p); mpz_clear(priv->q);
	mpz_clear(priv->dmp1); mpz_clear(priv->dmq1);
	mpz_clear(priv->iqmp);
}

void creer(pub_t *pub, priv_t *priv) {
	gmp_randstate_t etat; /* sert pour le generateur de nombres pseudo-aleatoires */
	mpz_t alea; /* sert a introduire un peu d'aleatoire */

	mpz_t candidat; /* un candidat pour pp, reste a voir s'il est premier */
	mpz_t mini; /* un minimum pour la taille des nombres premiers */
	mpz_t phi_n; /* l'indicatrice d'Euler de n : phi(n) */
	mpz_t d; /* l'exposant de dechiffrement */
	mpz_t tmp; /* une variable temporaire pour les calculs*/
	int ok; /* tout va-t'il bien ? */

	mpz_init(mini);
	mpz_ui_pow_ui(mini, 2U, L/2); /* mini = 2^(L/2) */
	gmp_randinit_default(etat); /* initialisation d'un generateur pseudo-aleatoire */
	gmp_randseed_ui(etat, (unsigned int) time(NULL) ); /* on recupere l'heure comme graine du generateur */

	mpz_init(alea); mpz_init(candidat);

	mpz_urandomb(alea, etat, 64U); /* alea = nombre aleatoire compris entre 0 et 2"(L/2) */
	mpz_add(candidat, mini, alea); /* candidat = mini + alea */
	mpz_nextprime(priv->p, candidat);

	mpz_urandomb(alea, etat, L/2); /* alea = nombre aleatoire compris entre 0 et 2"(L/2) */
	mpz_add(candidat, mini, alea); /* candidat = mini + alea */
	mpz_nextprime(priv->q, candidat);

	mpz_clear(mini); mpz_clear(candidat); mpz_clear(alea); /* on est poli, on fait le menage en sortant */

	mpz_mul(pub->n, priv->p, priv->q); /* n = p * q */

	mpz_init(phi_n);
	mpz_sub(phi_n, pub->n, priv->p);
	mpz_sub(phi_n, phi_n, priv->q);
	mpz_add_ui(phi_n, phi_n, 1U); /* avec les deux lignes precedentes, fait phi(n) = n - p - q + 1 */

	mpz_init(d);
	do { /* choix de e inversible modulo phi(n) et calcul de son inverse d dans la foulee */
		mpz_urandomm(pub->e, etat, phi_n); /* tire e au hasard entre 0 et phi(n) - 1 */
		ok = mpz_invert(d, pub->e, phi_n); /* d = inverse de e modulo phi(n) si c'est possible */
	} while (!ok);
	mpz_clear(phi_n); /* on a plus besoin de lui, on libere la memoire */

	/* on a finit les choix, on calcule le reste de la cle secrete optimisee */
	mpz_init(tmp);
	mpz_sub_ui(tmp, priv->p, 1U); /* tmp = p-1 */
	mpz_mod(priv->dmp1, d, tmp); /* dmp1 = d mod (p-1) */
	mpz_sub_ui(tmp, priv->q, 1U); /* tmp = q-1 */
	mpz_mod(priv->dmq1, d, tmp); /* dmq1 = d mod (q-1) */
	mpz_clear(d); mpz_clear(tmp); /* on a plus besoin d'eux, on les jette sans pitie */

	mpz_invert(priv->iqmp, priv->q, priv->p); /* iqmp = inverse de q modulo p */
}


void chiffrer(pub_t pub, mpz_t M, mpz_t C) {
	mpz_powm(C, M, pub.e, pub.n); /* C = M^e mod n */
}

void dechiffrer(priv_t priv, mpz_t C, mpz_t M) {
	mpz_t a, b; /* resultats intermediares avant theoreme chinois */
	mpz_init(a), mpz_init(b);
	mpz_t tmp1, tmp2; /* resultats intermediares */
	mpz_init(tmp1), mpz_init(tmp2);

	/* on utilise le theoreme chinois, pour calculer rapidement C^d mod n */
	mpz_powm(a, C, priv.dmp1, priv.p); /* a = C^(d mod (p-1)) mod p = C^d mod p */
	mpz_powm(b, C, priv.dmq1, priv.q); /* b = C^(d mod (q-1)) mod q = C^d mod q */
	mpz_sub(tmp1, a, b); /* tmp1 = a - b */
	mpz_mul(tmp1, tmp1, priv.iqmp); /* tmp1 = tmp1 * q^(-1) */
	mpz_mod(tmp2, tmp1, priv.p); /* on reduit : tmp2 = tmp1 mod p */
	mpz_mul(tmp1, priv.q, tmp2); /* tmp1 = q * tmp2 */
	mpz_add(M, b, tmp1); /* M = b + tmp1 */

	/* vive la proprete */
	mpz_clear(a); mpz_clear(b);
	mpz_clear(tmp1); mpz_clear(tmp2);
}

/* convertit une chaine de LO caracteres en un grand nombre. */
void chaine_to_nb(char chaine[], mpz_t nb) {
	mpz_t valeur; /* la valeur associee a un caractere, en fonction de sa position */
	mpz_init(valeur);
	unsigned int i; /* compteur pour la boucle */

	mpz_set_ui(nb, 0U);
	for (i = 0; i < LO; i++) {
		mpz_set_ui(valeur, (unsigned int)chaine[i]); /* conversion du i-eme caractere en grand entier */
		mpz_mul_2exp(valeur, valeur, i*8); /* valeur = valeur * 2^(8i) */
		mpz_add(nb, nb, valeur); /* nb = nb + valeur */
	}

	mpz_clear(valeur);
}

/* convertit un grand nombre en chaine de LO caracteres */
void nb_to_chaine(mpz_t nb, char chaine[]) {
	mpz_t valeur; /* valeur a convertir en caractere */
	mpz_init(valeur);
	mpz_t nb_local; /* une copie locale de nb pour ne pas l'abimer l'original */
	mpz_init(nb_local); mpz_set(nb_local, nb); /*nb_local = nb */
	unsigned int i; /* compteur de boucle */

	for (i = 0; i < LO; i++) {
		mpz_fdiv_r_2exp(valeur, nb_local, 8U); /* valeur = nb mod 2^8 */
		chaine[i] = (char) mpz_get_ui(valeur); /* conversion de valeur en caractere */
		/* on jette les 8 derniers bits de nb, qu'on vient de lire, pour preparer la prochaine iteration : */
		mpz_fdiv_q_2exp(nb_local, nb_local, 8U); /* nb_local = nb_local / 2^8 */
	}

	mpz_clear(valeur); mpz_clear(nb_local);
}

/* main a juste pour but de verifier qu'il n'y a pas d'erreur grossiere,
 * que le programme s'execute bien (sans erreur de segmentation ou autre),
 * et que sur des exemples il donne le resultat attendu */
int main(void) {
	char message[LO+1], D_message[LO+1]; /* le message en clair, son dechiffre, sous forme de caracteres */
	mpz_t M, C, D; /* le message, le chiffre, le dechiffre, sous forme de grands entiers */
	mpz_init(M); mpz_init(C); mpz_init(D);
	pub_t pub; /* clef publique */
	pub_init(&pub);
	priv_t priv; /* clef privee */
	priv_init(&priv);
	int i; /* compteur de boucle */

	creer(&pub, &priv);

	message[LO] = D_message[LO] = '\0'; /* terminaison des tableaux vus comme chaines */
	printf("Entrer un message, sous forme d'une chaine d'au plus %d caracteres.\n", LO);
	printf("pas de caracteres accentues, cedilles, etc.)\n");
	for (i = 0; i < LO; i++)
		if ((message[i] = getchar()) == '\n')
			break;
	while (i < LO)
		message[i++] = ' ';
	printf("Le message pris en compte est : \"%s\"\n", message);

	chaine_to_nb(message, M);
	chiffrer(pub, M, C);

	gmp_printf("Un fois convertit en grand entier, le message devient :\n %Zd \n", M);
	gmp_printf("Le message chiffre correspondant est :\n %Zd \n", C);

	dechiffrer(priv, C, D);
	nb_to_chaine(D, D_message);
	gmp_printf("En dechiffrant ce dernier, on trouve :\n %Zd \n", D);
	printf("Convertit en chaine de caracteres, cela signifie :\n %s \n", D_message);

	/* nettoyage de principe, meme si on quitte le programme juste apres */
	mpz_clear(M); mpz_clear(C); mpz_clear(D);
	pub_clear(&pub);
	priv_clear(&priv);
	return 0;
}
