
#include "bblht.h"

#define DOC "/usr/share/gtk-doc/html/gtk3/Gtk"
#define NIV 3

int  ht_sntfnc(char *, char *);
int  ht_nomdoc(char *, char *);
int  ht_read(char *, char *, char *);
int  ht_rmptxt(char *pr_bff, char *pr_txt, char *pr_atr, int pi_nmb);

// Syntaxe fonction
//
// Entree: pr_nomfnc: nom de la fonction pour qui on recherche la syntaxe (ex: gtk_window_new)
// Sortie: pr_sntfnc: syntaxe de la fonction (ex: GtkWidget *gtk_window_new(gint TOP_LEVEL))

int ht_sntfnc(char *pr_nomfnc, char *pr_sntfnc)
{
	char *lr_nomdoc, *lr_bfftxt;

	//printf("Fonction: %s\n", pr_nomfnc);
	
	*pr_sntfnc = 0;
	if ((lr_nomdoc = malloc(LNGSTR)) == NULL)
	{
		printf("Erreur memoire !!!\n");
		return(-1);
	}
		
	if (ht_nomdoc(pr_nomfnc, lr_nomdoc) != 0)
	{
		if (lr_nomdoc)
			free(lr_nomdoc);
		return(1);
	}
	if (ht_read(lr_nomdoc, pr_nomfnc, pr_sntfnc) != 0)
	{
		if (lr_nomdoc)
			free(lr_nomdoc);
		return(2);
	}
	
	if (lr_nomdoc)
		free(lr_nomdoc);
	
	return(0);
}

// Recherche le nom de la doc correspondante a la fonction
//
// Entree: pr_nomfnc: Nom de la fonction
// Sortie: pr_nomdoc: Nom de la doc contenant la fonction recherche

int ht_nomdoc(char *pr_nomfnc, char *pr_nomdoc)
{
	int  li_i, li_f;
	char *lp_d, *lp_f;
	char tmp[NIV+1][64];
	
	lp_d = pr_nomfnc;
	*pr_nomdoc = 0;
	
	if (strncmp(lp_d, "gtk_", 4) != 0)
	{
		printf("%s n'est pas une fonction GTK !!!\n", pr_nomfnc);
		return(-1);
	}
	
	lp_d += 4;
	lp_f = lp_d;
	li_i = 0;
	
	while (1)
	{
		while ((*lp_f != '_') && (*lp_f != 0))
			lp_f++;
		
		strncpy(tmp[li_i], lp_d, lp_f-lp_d);
		tmp[li_i][lp_f-lp_d] = 0;
		tmp[li_i][0] = toupper(tmp[li_i][0]);
		li_i++;
		
		if (*lp_f == 0)			// fin nom fonction
			break;
			
		if (li_i >= NIV)		// niveau max
			break;
			
		lp_d = lp_f + 1;
		lp_f = lp_d;
	}
	
	while (1)
	{
		strcpy(pr_nomdoc, DOC);
		for (li_f=0; li_f<li_i; li_f++)
			strcat(pr_nomdoc, tmp[li_f]);
		strcat(pr_nomdoc, ".html");
		//printf("??? %s\n", pr_nomdoc);
		if (access(pr_nomdoc, F_OK) == 0)
		{
			//printf("Nom doc = %s\n", pr_nomdoc);
			break;
		}
		li_i--;
		if (li_i == 0)
		{
			printf("Nom doc non trouve !!!\n");
			*pr_nomdoc = 0;
			break;
		}
	}
	
	return(!li_i);
}

// Lecture

int ht_read(char *pr_nomfch, char *pr_nomfnc, char *pr_sntfnc)
{
	char *lr_bffhtm, *lr_syntaxe, *lr_nomfnc;
	int  li_nmblus, li_tll;
	char *lp_d, *lp_f;
	char *lr_txt, *lp_txt;
	int  li_prn, li_ret;
	unsigned char spc;
	 
	if ((lr_bffhtm = io_bfffch(pr_nomfch, &li_nmblus)) == NULL)
	{
		printf("Erreur io_bfffch %s\n", pr_nomfch);
		return(-1);
	}
	
	li_tll = LNGSTR;
	if ((lr_txt = malloc(li_tll)) == NULL)
	{
		printf("Erreur memoire\n");
		io_frebff(lr_bffhtm);
		return(-1);
	}
	if ((lr_syntaxe = malloc(512)) == NULL)
	{
		printf("Erreur memoire\n");
		if (lr_txt)
			free(lr_txt);
		io_frebff(lr_bffhtm);
		return(-1);
	}
	if ((lr_nomfnc = malloc(strlen(pr_nomfnc)+2)) == NULL)
	{
		printf("Erreur memoire\n");
		if (lr_syntaxe)
			free(lr_syntaxe);
		if (lr_txt)
			free(lr_txt);
		io_frebff(lr_bffhtm);
		return(-1);
	}
	strcpy(lr_nomfnc, pr_nomfnc);
	strcat(lr_nomfnc, "(");
	
	lp_d = lr_bffhtm;
	lp_f = lp_d;
	li_prn = 3;
	spc = ' ';
	*lr_syntaxe = 0;
	li_ret = 1;
	
	while (1)
	{
		if (*lp_f == 0)
			break;
			
		switch (*lp_f)
		{
			case '<':
				while ((*lp_f != '>') && (*lp_f != 0))
					lp_f++;
				break;
			case '\n':
			case '\r':
				while ((*lp_f == '\n') || (*lp_f == '\r'))
					lp_f++;
				lp_f--;
				break;
			default:
				while ((*lp_f != '<') && (*lp_f != 0))
					lp_f++;
				if ((lp_f-lp_d) > li_tll)
				{
					li_tll = lp_f-lp_d;
					if ((lp_txt = realloc(lr_txt, li_tll)) != NULL)
						lr_txt = lp_txt;
				}
				strncpy(lr_txt, lp_d, lp_f-lp_d);
				lr_txt[lp_f-lp_d] = 0;
				lp_f--;
				if ((!li_prn) && (spc == 0xA0))
				{
					//printf("%s\n", lr_txt);
					strcat(lr_syntaxe, lr_txt);
					strcat(lr_syntaxe, " ");
				}
				if ((lp_txt = strstr(lr_txt, "()")) != NULL)
				{
					lp_txt--;
					spc = (unsigned char) *lp_txt;
				}
				if ((lp_txt = strstr(lr_txt, ");")) != NULL)
				{
					spc = ' ';
					li_nmblus = ht_rmptxt(lr_syntaxe, "", "", 0);	// ne pas enlever (voir fonction ht_rmptxt)
					li_nmblus = ht_rmptxt(lr_syntaxe, "\n", " ", 0);
					li_nmblus = ht_rmptxt(lr_syntaxe, "* ", "*", 0);
					li_nmblus = ht_rmptxt(lr_syntaxe, " )", ")", 0);
					li_nmblus = ht_rmptxt(lr_syntaxe, " , ", ", ", 0);
					li_nmblus = ht_rmptxt(lr_syntaxe, " ( ", "(", 0);
					li_nmblus = ht_rmptxt(lr_syntaxe, "  ", " ", 0);
					if (*lr_syntaxe)
					{
//printf("ht:<%s>\n", lr_syntaxe); 
						if (strstr(lr_syntaxe, lr_nomfnc) != NULL)
						{
							printf("ht:trouve\n");
							li_ret = 0;
							strcpy(pr_sntfnc, lr_syntaxe);
							printf("ht:Syntaxe=%s\n", pr_sntfnc);
						}
					}
					*lr_syntaxe = 0;
				}
				if (strcmp(lr_txt, "Functions") == 0)
					li_prn--;
				if (strcmp(lr_txt, "Types and Values") == 0)
					li_prn--;
				break;
		}
		
		if (li_ret == 0)
			break;
			
		lp_f++;
		lp_d = lp_f;
	}
	
//printf("ht:deb1\n");
	//if (lr_nomfnc)
	//	free(lr_nomfnc);
//printf("ht:deb2\n");
	//if (lr_syntaxe)
	//	free(lr_syntaxe);
//printf("ht:deb3\n");
	//if (lr_txt)
	//	free(lr_txt);
//printf("ht:deb4\n");
	io_frebff(lr_bffhtm);
	
//printf("ht:deb5\n");
	return(li_ret);
}

//
// Remplacement texte
//
// pr_txt: le texte d'origine
// pr_avn: le morceau a remplacer
// pr_apr: le nouveau morceau
// pi_nmb: nombre de remplacement (0=tous)
//
// retour: nombre effectif de remplacement

// Dans un buffer (pr_bff) remplace un texte (pr_txt) par un autre (pr_atr) un certain nombre (pi_nmb) de fois
//
// pi_nmb=0 --> tous
// Sortie: return: nombre de remplacement effectue

int ht_rmptxt(char *pr_bff, char *pr_txt, char *pr_atr, int pi_nmb)
{
	int i, j, k, l1, l2, l3, c;
	
	l1 = strlen(pr_bff);
	l2 = strlen(pr_txt);
	l3 = strlen(pr_atr);
	c = 0;
	
	if (l2 > l3)	// Raccourci la chaine
	{
		for (i=0; i<=(l1-l2); i++)
		{
			for (j=0; j<l2; j++)
				if (*(pr_bff+i+j) != *(pr_txt+j))
					break;
			if (j == l2)
			{
				c++;
				//strncpy(pr_bff+i, pr_atr, l3);
				for (k=0; k<l3; k++)
					*(pr_bff+i+k) = *(pr_atr+k);
				//strcpy(pr_bff+i+l3, pr_bff+i+l2);
				for (k=0; *(pr_bff+i+l2+k)!=0; k++)
					*(pr_bff+i+l3+k) = *(pr_bff+i+l2+k);
				*(pr_bff+i+l3+k) = 0;
				if (pi_nmb)
					if (pi_nmb == c)
						break;
				i--;
			}
		}
	}
	else
	{
		if (l2 < l3) 	// Allonge la chaine (attention a sa taille)
		{
				for (i=0; i<=(l1-l2); i++)
				{
				for (j=0; j<l2; j++)
					if (*(pr_bff+i+j) != *(pr_txt+j))
						break;
				if (j == l2)
				{
					c++;
					//strcpy(pr_bff+i+l3, pr_bff+i+l2);
					for (k=l1; k>=(i+l2); k--)
						*(pr_bff+k+(l3-l2)) = *(pr_bff+k);
					//strncpy(pr_bff+i, pr_atr, l3);
					for (k=0; k<l3; k++)
						*(pr_bff+i+k) = *(pr_atr+k);
					if (pi_nmb)
						if (pi_nmb == c)
							break;
					i--;
				}
			}
		}
		else
		{
			if (l2 == l3)		// meme longueur
			{
				if ((l2 == 0) && (l3 == 0))	// cas particulier (remplace vide par vide) pour supprimer le caractere special 0xA0
				{
					for (i=0; i<l1; i++)
					{
						if (*(pr_bff+i) == 0xFFFFFFA0)
						{
							*(pr_bff+i-1) = ' ';
							*(pr_bff+i) = ' ';
						}
					}
				}
				else
				{
					for (i=0; i<=(l1-l2); i++)	// cas normal de meme longueur
					{
						for (j=0; j<l2; j++)
							if (*(pr_bff+i+j) != *(pr_txt+j))
								break;
						if (j == l2)
						{
							c++;
							//strncpy(pr_bff+i, pr_atr, l3);
							for (k=0; k<l3; k++)
								*(pr_bff+i+k) = *(pr_atr+k);
							if (pi_nmb)
								if (pi_nmb == c)
									break;
							i--;
						}
					}
				}
			}
		}
	}
	
	return(c);
}


