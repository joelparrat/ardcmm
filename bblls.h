
#ifndef BBLLS_H
#define BBLLS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//
// Contenu de mmr
//
// liste de pointer terminee par null
// le contenu
//
// ex
//
// pppppppp000000001234
// pppppppp00000000A
// pppppppp00000000texte
// ppppppppssssssss00000000premierdeuxieme
//

#ifndef CH
typedef struct _ch
{
	struct _ch *prior;										// precedent
	struct _ch *next;										// suivant
	char *mmr;
} CH;
#endif

#ifndef LS
typedef struct _ls
{
	CH *first;												// premier
	CH *current;											// courant
	CH *last;												// dernier
} LS;
#endif

extern LS	*ls_cns(void);									// Creation d'une liste
extern void	ls_adc(LS *, char *, char *, ...);				// add avec double tri croissant
extern void	ls_add(LS *, char *, char *, ...);				// add avec double tri decroissant
extern void	ls_adp(LS *, char *, char *, ...);				// add avec double sans tri en debut de liste
extern int	ls_adf(LS *, char *, char *, ...);				// add avec double sans tri en fin de liste
extern int	ls_asc(LS *, char *, char *, ...);				// add sans double tri croissant
extern void	ls_asd(LS *, char *, char *, ...);				// add sans double tri decroissant
extern void	ls_asp(LS *, char *, char *, ...);				// add sans double sans tri en debut de liste
extern int	ls_asf(LS *, char *, char *, ...);				// add sans double sans tri en fin de liste
extern void	ls_stx(LS *, char *, int);						// Supprime un element ou il y a texte en c
extern void	ls_sps(LS *, int);								// Supprime un element en position p
extern void	ls_vid(LS *);									// Vide toute la liste
extern int	ls_rtx(LS *, char *, int);						// Recherche l'element contenant texte en c
extern int  ls_rts(LS *, char *txt, int col);				// Recherche le texte txt suivant en colonne col / current
extern char	*ls_rps(LS *, int pos, int col);				// Recherche la valeur val de la colonne col en position pos
extern int  ls_rtt(LS *liste, char *, int , char *, int );	// Recherche texte et texte
extern int	ls_mtx(LS *, char *, int, char *, int);			// Modifie la valeur en colonne n de l'element contenant texte en colonne c
extern int	ls_mps(LS *, int pos, char *val, int col);		// Modifie la valeur val en colonne col de l'element en position pos
extern void	ls_prn(LS *);									// Vide dans stdout le contenu de la liste
extern void	ls_dst(LS *);									// Detruit la liste
#endif

