
#include "bblls.h"

//LS LISTE = { NULL, NULL, NULL };

LS		*ls_cns(void);									// Creation d'une liste
void	ls_adc(LS *, char *, char *, ...);				// adc avec double tri croissant
void	ls_add(LS *, char *, char *, ...);				// add avec double tri decroissant
void	ls_adp(LS *, char *, char *, ...);				// adp avec double sans tri en debut de liste
int		ls_adf(LS *, char *, char *, ...);				// adf avec double sans tri en fin de liste
int		ls_asc(LS *, char *, char *, ...);				// asc sans double tri croissant
void	ls_asd(LS *, char *, char *, ...);				// asd sans double tri decroissant
void	ls_asp(LS *, char *, char *, ...);				// asp sans double sans tri en debut de liste
int		ls_asf(LS *, char *, char *, ...);				// asf sans double sans tri en fin de liste
void	ls_stx(LS *, char *, int);						// Supprime un element ou il y a texte en c
void	ls_sps(LS *, int);								// Supprime un element en position p
void	ls_vid(LS *);									// Vide toute la liste
int		ls_rtx(LS *, char *val, int col);				// Recherche la position pos de l'element contenant la valeur val en colonne col
int     ls_rts(LS *, char *txt, int col);				// Recherche le texte txt suivant en colonne col / current
char	*ls_rps(LS *, int pos, int col);				// Recherche la valeur val de la colonne col en position pos
int     ls_rtt(LS *liste, char *, int , char *, int );	// Recherche texte et texte
int		ls_mtx(LS *, char *, int, char *, int);			// Modifie la valeur en colonne n de l'element contenant texte en colonne c
int		ls_mps(LS *, int pos, char *val, int col);		// Modifie la valeur val en colonne col de l'element en position pos
void	ls_prn(LS *);									// Vide dans stdout le contenu de la liste
void	ls_dst(LS *);									// Detruit la liste
void	ls_affmmr(LS *, char *, char *, int);

//
// Creation d'une liste
//

LS *ls_cns(void)
{
	LS *ls;
	
	ls = malloc(sizeof(*ls));
	ls->first = NULL;
	ls->current = NULL;
	ls->last = NULL;
	
	return(ls);
}

//
// Ajoute un texte dans la liste (avec double, tri croissant)
//
// pour un texte simple ls_add(texte)
// pour un texte et un caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
/*
void ls_adc(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv, *prc, *crt;
	va_list ap;
	char *chn;
	int  i, nmb;
	char car;
	
   	nv = malloc(sizeof(*nv));
	nv->texte1 = NULL;
	nv->texte2 = NULL;
	nv->texte3 = NULL;
	nv->nombre = -1;
	nv->caractere = ' ';

	if (strchr(texte, '%') == NULL)
	{
		nv->texte1 = malloc(strlen(texte)+1);
		strcpy(nv->texte1, texte);
	}
	else
	{
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':              // string
					if (i == 0)
					{
						chn = va_arg(ap, char *);
						nv->texte1 = malloc(strlen(chn)+1);
						strcpy(nv->texte1, chn);
					}
					if (i == 1)
					{
						chn = va_arg(ap, char *);
						nv->texte2 = malloc(strlen(chn)+1);
						strcpy(nv->texte2, chn);
					}
					if (i == 2)
					{
						chn = va_arg(ap, char *);
						nv->texte3 = malloc(strlen(chn)+1);
						strcpy(nv->texte3, chn);
					}
					i++;
					break;
				case 'd':              // int
					nmb = va_arg(ap, int);
					nv->nombre = nmb;
					break;
				case 'c':              // char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					nv->caractere = car;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)	// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
	}
	else					// ajout en fin de liste
	{
		//liste->last->next = nv;
		//nv->prior = liste->last;
		//nv->next = NULL;
		//liste->current = nv;
		//liste->last = nv;

		prc = crt = liste->first;		// Insertion triee
		while (crt != NULL)
		{
			if (strcmp(crt->texte1, nv->texte1) >= 0)
			{
				//printf("%s > %s\n", crt->texte1, nv->texte1);
				break;
			}
			//printf("%s < %s\n", crt->texte1, nv->texte1);
		
			prc = crt;
			crt = crt->next;
		}
		if (crt == NULL)
		{
			//printf("dernier %s\n", nv->texte1);
			prc->next = nv;
			nv->prior = prc;
			nv->next = NULL;
			liste->last = nv;
			liste->current = nv;
		}
		else
		{
			if (crt == prc)
			{
				//printf("nouveau premier %s\n", nv->texte1);
				liste->first = nv;
				crt->prior = nv;
				nv->next = crt;
				nv->prior = NULL;
			}
			else
			{
				//printf("trie %s\n", nv->texte1);
				prc->next = nv;
				crt->prior = nv;
				nv->prior = prc;
				nv->next = crt;
			}
		}
	}
}
*/
//
// Ajoute un texte dans la liste (avec double, tri decroissant)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
/*
void ls_add(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv;
	va_list ap;
	char *chn;
	int  i, nmb;
	char car;
	
   	nv = malloc(sizeof(*nv));
	nv->texte1 = NULL;
	nv->texte2 = NULL;
	nv->texte3 = NULL;
	nv->nombre = -1;
	nv->caractere = ' ';

	if (strchr(texte, '%') == NULL)
	{
		nv->texte1 = malloc(strlen(texte)+1);
		strcpy(nv->texte1, texte);
	}
	else
	{
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':              // string
					if (i == 0)
					{
						chn = va_arg(ap, char *);
						nv->texte1 = malloc(strlen(chn)+1);
						strcpy(nv->texte1, chn);
					}
					if (i == 1)
					{
						chn = va_arg(ap, char *);
						nv->texte2 = malloc(strlen(chn)+1);
						strcpy(nv->texte2, chn);
					}
					if (i == 2)
					{
						chn = va_arg(ap, char *);
						nv->texte3 = malloc(strlen(chn)+1);
						strcpy(nv->texte3, chn);
					}
					i++;
					break;
				case 'd':              // int
					nmb = va_arg(ap, int);
					nv->nombre = nmb;
					break;
				case 'c':              // char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					nv->caractere = car;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)	// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
	}
	else					// ajout en fin de liste
	{
		liste->last->next = nv;
		nv->prior = liste->last;
		nv->next = NULL;
		liste->current = nv;
		liste->last = nv;
	}
}
*/
//
// Ajoute un texte dans la liste (avec double, sans tri en debut)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
/*
void ls_adp(LS *liste, char * clntri, char *texte, ...)
{
	CH *nv;
	va_list ap;
	char *chn;
	int  i, nmb;
	char car;
	
   	nv = malloc(sizeof(*nv));
	nv->texte1 = NULL;
	nv->texte2 = NULL;
	nv->texte3 = NULL;
	nv->nombre = -1;
	nv->caractere = ' ';

	if (strchr(texte, '%') == NULL)
	{
		nv->texte1 = malloc(strlen(texte)+1);
		strcpy(nv->texte1, texte);
	}
	else
	{
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':              // string
					if (i == 0)
					{
						chn = va_arg(ap, char *);
						nv->texte1 = malloc(strlen(chn)+1);
						strcpy(nv->texte1, chn);
					}
					if (i == 1)
					{
						chn = va_arg(ap, char *);
						nv->texte2 = malloc(strlen(chn)+1);
						strcpy(nv->texte2, chn);
					}
					if (i == 2)
					{
						chn = va_arg(ap, char *);
						nv->texte3 = malloc(strlen(chn)+1);
						strcpy(nv->texte3, chn);
					}
					i++;
					break;
				case 'd':              // int
					nmb = va_arg(ap, int);
					nv->nombre = nmb;
					break;
				case 'c':              // char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					nv->caractere = car;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)	// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
	}
	else					// ajout en fin de liste
	{
		liste->last->next = nv;
		nv->prior = liste->last;
		nv->next = NULL;
		liste->current = nv;
		liste->last = nv;
	}
}
*/
//
// Ajoute un texte dans la liste (avec double, sans tri en fin)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//

int ls_adf(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv, *prc, *crt;
	va_list ap;
	char *d, *chn;
	int  i, n, t, tt, nmb, x, o;
	int  c1, c2, c3;
	char car, nombre[16];
	char *m;
	
   	nv = malloc(sizeof(*nv));

	if (strchr(texte, '%') == NULL)
	{
		nv->mmr = malloc((sizeof(nv->mmr)*2)+strlen(texte)+1);
		*(nv->mmr) = 16;
		*(nv->mmr+8) = NULL;
		strcpy(nv->mmr+(*(nv->mmr)), texte);
	}
	else
	{
		for (n = 0, d = texte; *d != 0; d++)	// compte le nombre de variables (n)
			if ((*d == '%') && (*(d+1) != '%'))
				n++;

		tt = sizeof(nv->mmr) * (n+1);			// allocation memoire (tt) pour les pointeurs sur les variables
		nv->mmr = malloc(tt);
		for (i=0; i<=n; i++)
			*(nv->mmr+(i*8)) = NULL;					// le dernier pointer est NULL
		//ls_affmmr(liste, "init", nv->mmr, n);
		
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':	// string
					chn = va_arg(ap, char *);
					t = strlen(chn) + 1;				// ajoute la longueur de la chaine dans l'allocation memoire (tt)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					//printf("realloc <%s>\n", chn);
					//ls_affmmr(liste, "realloc", nv->mmr, n);
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), chn);
					o = t;
					i++;
					//ls_affmmr(liste, "string", nv->mmr, n);
					break;
					
				case 'd':	// int
					nmb = va_arg(ap, int);
					sprintf(nombre, "%d", nmb);
					t = strlen(nombre) + 1;			// ajoute la longueur du nombre convertie en chaine (t)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					//printf("realloc <%s>\n", nombre);
					//ls_affmmr(liste, "realloc", nv->mmr, n);
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = t;
					i++;
					//ls_affmmr(liste, "int", nv->mmr, n);
					break;
					
				case 'c':	// char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					sprintf(nombre, "%c", car);
					tt += 2;								// ajoute la longueur d'un CAR convertie en chaine dans t
					m = realloc(nv->mmr, tt);
					nv->mmr = m;
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					//printf("realloc <%s>\n", nombre);
					//ls_affmmr(liste, "realloc", nv->mmr, n);
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = 2;
					i++;
					//ls_affmmr(liste, "char", nv->mmr, n);
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)				// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
		//ls_affmmr(liste, "premier", nv->mmr, n);
	}
	else							// ajout en fin de liste
	{
		liste->last->next = nv;
		nv->prior = liste->last;
		nv->next = NULL;
		liste->current = nv;
		liste->last = nv;
		//ls_affmmr(liste, "fin", nv->mmr, n);
	}

	return(0);
}

//
// Ajoute un texte dans la liste (sans double, tri croissant)
//
// pour un texte simple ls_add(ls, 0, txt)
// pour un texte et un caractere ls_add(ls, 0, "%s %c", txt, car)
// si c'est un nombre ls_add(ls, 0, "%d", nmb)
// idem avec caractere ls_add(ls, 0, "%d %c", nmb, car)
//
// tri: colonne a trie 0 premiere 1 deuxieme ...
//

int ls_asc(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv, *prc, *crt;
	va_list ap;
	char *d, *chn;
	int  i, n, t, tt, nmb, x, o;
	int  c1, c2, c3;
	char car, nombre[16];
	char *m;
	
   	nv = malloc(sizeof(*nv));

	if (strchr(texte, '%') == NULL)
	{
		nv->mmr = malloc((sizeof(nv->mmr)*2)+strlen(texte)+1);
		*(nv->mmr) = 16;
		*(nv->mmr+8) = NULL;
		strcpy(nv->mmr+(*(nv->mmr)), texte);
	}
	else
	{
		for (n = 0, d = texte; *d != 0; d++)	// compte le nombre de variables (n)
			if ((*d == '%') && (*(d+1) != '%'))
				n++;

		tt = sizeof(nv->mmr) * (n+1);			// allocation memoire (tt) pour les pointeurs sur les variables
		nv->mmr = malloc(tt);
		for (i=0; i<=n; i++)
			*(nv->mmr+(i*8)) = NULL;					// le dernier pointer est NULL
		
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':	// string
					chn = va_arg(ap, char *);
					t = strlen(chn) + 1;				// ajoute la longueur de la chaine dans l'allocation memoire (tt)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), chn);
					o = t;
					i++;
					break;
					
				case 'd':	// int
					nmb = va_arg(ap, int);
					sprintf(nombre, "%d", nmb);
					t = strlen(nombre) + 1;			// ajoute la longueur du nombre convertie en chaine (t)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = t;
					i++;
					break;
					
				case 'c':	// char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					sprintf(nombre, "%c", car);
					tt += 2;								// ajoute la longueur d'un CAR convertie en chaine dans t
					m = realloc(nv->mmr, tt);
					nv->mmr = m;
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = 2;
					i++;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	crt = liste->first;
	switch (sscanf(clntri, "%d,%d,%d", &c1, &c2, &c3))					// recherche si existe deja (sans double)
	{
		case 1:
			while (crt != NULL)
			{
				if (strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0)
					break;
				crt = crt->next;
			}
			break;
		case 2:
			while (crt != NULL)
			{
				if ((strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c2*8))), nv->mmr+(*(nv->mmr+(c2*8)))) == 0))
					break;
				crt = crt->next;
			}
			break;
		case 3:
			while (crt != NULL)
			{
				if ((strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c2*8))), nv->mmr+(*(nv->mmr+(c2*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c3*8))), nv->mmr+(*(nv->mmr+(c3*8)))) == 0))
					break;
				crt = crt->next;
			}
			break;
	}

	if (crt == NULL)
	{
		if (liste->last == NULL)	// creation premier
		{
			nv->prior = NULL;
			nv->next = NULL;
			liste->first = nv;
			liste->current = nv;
			liste->last = nv;
		}
		else					// ajout en fin de liste
		{
			//liste->last->next = nv;
			//nv->prior = liste->last;
			//nv->next = NULL;
			//liste->current = nv;
			//liste->last = nv;

			prc = crt = liste->first;		// Insertion triee
			switch (sscanf(clntri, "%d,%d,%d", &c1, &c2, &c3))					// recherche si tri croissant
			{
				case 1:
					while (crt != NULL)
					{
						if (strcmp(crt->mmr+(*crt->mmr+c1), nv->mmr+(*nv->mmr+c1)) > 0)
							break;
						prc = crt;
						crt = crt->next;
					}
					break;
				case 2:
					while (crt != NULL)
					{
						if ((strcmp(crt->mmr+(*crt->mmr+c1), nv->mmr+(*nv->mmr+c1)) >= 0) && 
							(strcmp(crt->mmr+(*crt->mmr+c2), nv->mmr+(*nv->mmr+c2)) > 0))
							break;
						crt = crt->next;
					}
					break;
				case 3:
					while (crt != NULL)
					{
						if ((strcmp(crt->mmr+(*crt->mmr+c1), nv->mmr+(*nv->mmr+c1)) >= 0) && 
							(strcmp(crt->mmr+(*crt->mmr+c2), nv->mmr+(*nv->mmr+c2)) >= 0) && 
							(strcmp(crt->mmr+(*crt->mmr+c3), nv->mmr+(*nv->mmr+c3)) > 0))
							break;
						crt = crt->next;
					}
					break;
			}
			if (crt == NULL)
			{
				//printf("dernier %s\n", nv->texte1);
				prc->next = nv;
				nv->prior = prc;
				nv->next = NULL;
				liste->last = nv;
				liste->current = nv;
			}
			else
			{
				if (crt == prc)
				{
					//printf("nouveau premier %s\n", nv->texte1);
					liste->first = nv;
					crt->prior = nv;
					nv->next = crt;
					nv->prior = NULL;
				}
				else
				{
					//printf("trie %s\n", nv->texte1);
					prc->next = nv;
					crt->prior = nv;
					nv->prior = prc;
					nv->next = crt;
				}
			}
		}
		
		return(0);
	}
	
	return(1);
}

//
// Ajoute un texte dans la liste (sans double, tri decroissant)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
/*
void ls_asd(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv;
	va_list ap;
	char *chn;
	int  i, nmb;
	char car;
	
   	nv = malloc(sizeof(*nv));
	nv->texte1 = NULL;
	nv->texte2 = NULL;
	nv->texte3 = NULL;
	nv->nombre = -1;
	nv->caractere = ' ';

	if (strchr(texte, '%') == NULL)
	{
		nv->texte1 = malloc(strlen(texte)+1);
		strcpy(nv->texte1, texte);
	}
	else
	{
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':              // string
					if (i == 0)
					{
						chn = va_arg(ap, char *);
						nv->texte1 = malloc(strlen(chn)+1);
						strcpy(nv->texte1, chn);
					}
					if (i == 1)
					{
						chn = va_arg(ap, char *);
						nv->texte2 = malloc(strlen(chn)+1);
						strcpy(nv->texte2, chn);
					}
					if (i == 2)
					{
						chn = va_arg(ap, char *);
						nv->texte3 = malloc(strlen(chn)+1);
						strcpy(nv->texte3, chn);
					}
					i++;
					break;
				case 'd':              // int
					nmb = va_arg(ap, int);
					nv->nombre = nmb;
					break;
				case 'c':              // char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					nv->caractere = car;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)	// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
	}
	else					// ajout en fin de liste
	{
		liste->last->next = nv;
		nv->prior = liste->last;
		nv->next = NULL;
		liste->current = nv;
		liste->last = nv;
	}
}
*/
//
// Ajoute un texte dans la liste (sans double, sans tri en debut)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
/*
void ls_asp(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv;
	va_list ap;
	char *chn;
	int  i, nmb;
	char car;
	
   	nv = malloc(sizeof(*nv));
	nv->texte1 = NULL;
	nv->texte2 = NULL;
	nv->texte3 = NULL;
	nv->nombre = -1;
	nv->caractere = ' ';

	if (strchr(texte, '%') == NULL)
	{
		nv->texte1 = malloc(strlen(texte)+1);
		strcpy(nv->texte1, texte);
	}
	else
	{
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':              // string
					if (i == 0)
					{
						chn = va_arg(ap, char *);
						nv->texte1 = malloc(strlen(chn)+1);
						strcpy(nv->texte1, chn);
					}
					if (i == 1)
					{
						chn = va_arg(ap, char *);
						nv->texte2 = malloc(strlen(chn)+1);
						strcpy(nv->texte2, chn);
					}
					if (i == 2)
					{
						chn = va_arg(ap, char *);
						nv->texte3 = malloc(strlen(chn)+1);
						strcpy(nv->texte3, chn);
					}
					i++;
					break;
				case 'd':              // int
					nmb = va_arg(ap, int);
					nv->nombre = nmb;
					break;
				case 'c':              // char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					nv->caractere = car;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	if (liste->last == NULL)	// creation premier
	{
		nv->prior = NULL;
		nv->next = NULL;
		liste->first = nv;
		liste->current = nv;
		liste->last = nv;
	}
	else					// ajout en fin de liste
	{
		liste->last->next = nv;
		nv->prior = liste->last;
		nv->next = NULL;
		liste->current = nv;
		liste->last = nv;
	}
}
*/

//
// Ajoute un texte dans la liste (sans double, sans tri en fin)
//
// pour un texte simple ls_add(texte)
// pour un texte et une caractere ls_add("%s %c", texte, caractere)
// si c'est un nombre ls_add("%d", nombre)
// idem avec caractere ls_add("%d %c", nombre, caractere)
//
// clntri: pour teste d'existance sur 1-3 colonnes (3 colonnes max)
// ex: 0,2,5 le test se fait sur la colonne 0, 2 et 5 (0=premiere, 1=deuxieme...)
// ex: 0 une seule colonne
//
// retour: 0=ajoute 1=existe deja rien fait
//

int ls_asf(LS *liste, char *clntri, char *texte, ...)
{
	CH *nv, *prc, *crt;
	va_list ap;
	char *d, *chn;
	int  i, n, t, tt, nmb, x, o;
	int  c1, c2, c3;
	char car, nombre[16];
	char *m;
	
   	nv = malloc(sizeof(*nv));

	if (strchr(texte, '%') == NULL)
	{
		nv->mmr = malloc((sizeof(nv->mmr)*2)+strlen(texte)+1);
		*(nv->mmr) = 16;
		*(nv->mmr+8) = NULL;
		strcpy(nv->mmr+(*(nv->mmr)), texte);
	}
	else
	{
		for (n = 0, d = texte; *d != 0; d++)	// compte le nombre de variables (n)
			if ((*d == '%') && (*(d+1) != '%'))
				n++;

		tt = sizeof(nv->mmr) * (n+1);			// allocation memoire (tt) pour les pointeurs sur les variables
		nv->mmr = malloc(tt);
		for (i=0; i<=n; i++)
			*(nv->mmr+(i*8)) = NULL;					// le dernier pointer est NULL
		
		va_start(ap, texte);
		i = 0;
		while (*texte)
		{
			if (*texte != '%')
			{
				texte++;
				continue;
			}
			texte++;
			switch (*texte)
			{
				case 's':	// string
					chn = va_arg(ap, char *);
					t = strlen(chn) + 1;				// ajoute la longueur de la chaine dans l'allocation memoire (tt)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), chn);
					o = t;
					i++;
					break;
					
				case 'd':	// int
					nmb = va_arg(ap, int);
					sprintf(nombre, "%d", nmb);
					t = strlen(nombre) + 1;			// ajoute la longueur du nombre convertie en chaine (t)
					tt += t;
					m = realloc(nv->mmr, tt);
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					nv->mmr = m;
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = t;
					i++;
					break;
					
				case 'c':	// char
					// need a cast here since va_arg only takes fully promoted types
					car = (char) va_arg(ap, int);
					sprintf(nombre, "%c", car);
					tt += 2;								// ajoute la longueur d'un CAR convertie en chaine dans t
					m = realloc(nv->mmr, tt);
					nv->mmr = m;
					if (m == NULL)
						printf("ERREUR m = NULL\n");
					if (i == 0)
						*(nv->mmr) = (n + 1)*8;		// on fait pointer le pointeur courant (i) sur la position courante (n+i+1)
					else
						*(nv->mmr+(i*8)) = (*(nv->mmr+((i-1)*8))) + o;
					strcpy(nv->mmr+(*(nv->mmr+(i*8))), nombre);
					o = 2;
					i++;
					break;
			}
			texte++;
		}
		va_end(ap);
	}
	
	crt = liste->first;
	switch (sscanf(clntri, "%d,%d,%d", &c1, &c2, &c3))					// recherche si existe deja (sans double)
	{
		case 1:
			while (crt != NULL)
			{
				if (strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0)
					break;
				crt = crt->next;
			}
			break;
		case 2:
			while (crt != NULL)
			{
				if ((strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c2*8))), nv->mmr+(*(nv->mmr+(c2*8)))) == 0))
					break;
				crt = crt->next;
			}
			break;
		case 3:
			while (crt != NULL)
			{
				if ((strcmp(crt->mmr+(*(crt->mmr+(c1*8))), nv->mmr+(*(nv->mmr+(c1*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c2*8))), nv->mmr+(*(nv->mmr+(c2*8)))) == 0) &&
					(strcmp(crt->mmr+(*(crt->mmr+(c3*8))), nv->mmr+(*(nv->mmr+(c3*8)))) == 0))
					break;
				crt = crt->next;
			}
			break;
	}
	if (crt == NULL)
	{
		if (liste->last == NULL)	// creation premier
		{
			nv->prior = NULL;
			nv->next = NULL;
			liste->first = nv;
			liste->current = nv;
			liste->last = nv;
		}
		else					// ajout en fin de liste
		{
			liste->last->next = nv;
			nv->prior = liste->last;
			nv->next = NULL;
			liste->current = nv;
			liste->last = nv;
		}

		return(0);
	}
	
	return(1);
}


//
// Supprime un element de la liste ou il y a texte en colonne c
//

void ls_stx(LS *liste, char *texte, int c)
{
	CH *crn;
	int numero;
	long *p;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		p = (long *) crn->mmr;
		
		if (strcmp(*(p+c), texte) == 0)
		{
			if (numero == 0)	// efface le premier
			{
				if (crn->next == NULL)	// premier et unique
				{
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
					liste->first = NULL;
					liste->current = NULL;
					liste->last = NULL;
				}
				else			// le suivant devient premier et courant ?
				{
					liste->first = crn->next;
					if (liste->current == crn)
						liste->current = crn->next;
					crn->next->prior = NULL;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
			}
			else
			{
				if (crn->next == NULL)	// dernier
				{
					if (liste->current == crn)
						liste->current = crn->prior;
					liste->last = crn->prior;
					crn->prior->next = NULL;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
				else			// au milieu
				{
					if (liste->current == crn)
						liste->current = crn->prior;
					crn->prior->next = crn->next;
					crn->next->prior = crn->prior;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
			}
			break;
		}
	}
}

//
// Supprime un element de la liste a la position p
//

void ls_sps(LS *liste, int position)
{
	CH *crn;
	int numero;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		if (position == numero)
		{
			if (numero == 0)	// efface le premier
			{
				if (crn->next == NULL)	// premier et unique
				{
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
					liste->first = NULL;
					liste->current = NULL;
					liste->last = NULL;
				}
				else			// le suivant devient premier et courant ?
				{
					liste->first = crn->next;
					if (liste->current == crn)
						liste->current = crn->next;
					crn->next->prior = NULL;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
			}
			else
			{
				if (crn->next == NULL)	// dernier
				{
					if (liste->current == crn)
						liste->current = crn->prior;
					liste->last = crn->prior;
					crn->prior->next = NULL;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
				else			// au milieu
				{
					if (liste->current == crn)
						liste->current = crn->prior;
					crn->prior->next = crn->next;
					crn->next->prior = crn->prior;
					if (crn->mmr)
						free(crn->mmr);
					free(crn);
				}
			}
			break;
		}
	}
}

//
// Vide toute la liste
//

void ls_vid(LS *liste)
{
	while (liste->first != NULL)
		ls_sps(liste, 0);
}

//
// Recherche l'element contenant texte en colonne c
//

int ls_rtx(LS *liste, char *texte, int c)
{
	CH *crn;
	int numero;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		if (strcmp(crn->mmr+(*(crn->mmr+(c*8))), texte) == 0)
		{
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?-1:numero);
}

//
// Recherche l'element suivant contenant texte en colonne c
//

int ls_rts(LS *liste, char *texte, int c)
{
	CH *crn;
	int numero;
	
	numero = 0;
	crn = liste->current;
	for (crn=crn->next; crn != NULL; crn = crn->next, numero++)
	{
		if (strcmp(crn->mmr+(*(crn->mmr+(c*8))), texte) == 0)
		{
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?-1:numero);
}

//
// Pour tous les textes xxx en colonne nnn cherche l'existance d'un texte ttt en colonne ccc
//
// recherche xxx && ttt en nnn & ccc
//

int ls_rtt(LS *liste, char *xxx, int nnn, char *ttt, int ccc)
{
	CH *crn;
	int numero;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		if ((strcmp(crn->mmr+(*(crn->mmr+(nnn*8))), xxx) == 0) && (strcmp(crn->mmr+(*(crn->mmr+(ccc*8))), ttt) == 0))
		{
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?-1:numero);
}

//
// Recherche la valeur val de la colonne col en position pos
//

char *ls_rps(LS *liste, int pos, int col)
{
	CH *crn;
	int numero;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		if (pos == numero)
		{
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?NULL:*(crn->mmr+(col*8)));
}

//
// Modifie la valeur en colonne n de l'element contenant texte en colonne c
//

int ls_mtx(LS *liste, char *texte, int c, char *valeur, int n)
{
	CH *crn;
	int numero;
	long *p;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		p = (long *) crn->mmr;
		
		if (strcmp(*(crn->mmr+c), texte) == 0)
		{
			strcpy(*(p+n), valeur);	// doit avoir la meme longueur sinon realloc()
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?-1:numero);
}

//
// Modifie la valeur val en colonne col de l'element en position pos
//

int ls_mps(LS *liste, int pos, char *val, int col)
{
	CH *crn;
	int numero;
	
	for (numero = 0, crn = liste->first; crn != NULL; crn = crn->next, numero++)
	{
		if (numero == pos)
		{
			strcpy(crn->mmr+(*(crn->mmr+(col*8))), val);	// doit avoir la meme longueur sinon realloc()
			liste->current = crn;
			break;
		}
	}
	return((crn==NULL)?-1:numero);
}

//
// Vide le contenu de la liste
//

void ls_prn(LS *liste)
{
	CH *chn;
	int i, li_no=0;

	if (liste->first != NULL)
	{
		chn = liste->first;
		do
		{
			printf("LS: %d ", li_no);
			for (i=0; *(chn->mmr+i); i+=8)
				printf("%s ", chn->mmr+(*(chn->mmr+i)));
			printf("\n");
			li_no++;
			chn = chn->next;
		} while (chn != NULL);
	}
	else
		printf("Liste VIDE !!!\n");
}

//
// Detruit la liste
//

void ls_dst(LS *liste)
{
	ls_vid(liste);
	free(liste);
}

void ls_affmmr(LS *liste, char *msg, char *mmr, int n)
{
	int i;
	
	for (i=0; i<(n*8); i+=8)
	{
		printf("%s %d %ld %ld <%s>\n", msg, i, mmr+i, *(mmr+i), (*(mmr+i)==0)?"":mmr+(*(mmr+i))); 
	}
	printf("%s %d %ld %ld\n", msg, i, mmr+i, *(mmr+i)); 
}

