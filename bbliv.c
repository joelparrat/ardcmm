
// nom: bbliv.c
// long: Bibliotheque Info Version

#include <stdio.h>
#include <string.h>

int iv_info(char *, int, char *, char *, char *);

//
// Lecture Information Version
//
// pp_bff: entree, bufferisation du programme ou on lit l'info
// pi_lng: entree, longueur du buffer programme ou on lit l'info
// pp_prj: sortie, nom du projet trouve dans le buffer
// pp_vrs: sortie, version trouvee dans le buffer
// pp_dat: sortie, date de creation trouvee dans le buffer
// retour: 0=trouve 1=pas trouve
//

int iv_info(char *pp_bff, int pi_lng, char *pp_prj, char *pp_vrs, char *pp_dat)
{
	int i, j;
	
	strcpy(pp_prj, "?");
	strcpy(pp_vrs, "?");
	strcpy(pp_dat, "?");
	
	if (pi_lng < 25)
		return(1);
		
	for (i=0, j=32; i<pi_lng; i++)
	{
		if ((*(pp_bff+i+0) == 'j') &&
			(*(pp_bff+i+1) == 'o') &&
			(*(pp_bff+i+2) == 'e') &&
			(*(pp_bff+i+3) == 'l'))
		{
			i += 5;
			for (j=0; j<32; j++)
			{
				if ((*(pp_bff+i+j+0) == 'p') &&
					(*(pp_bff+i+j+1) == 'a') &&
					(*(pp_bff+i+j+2) == 'r') &&
					(*(pp_bff+i+j+3) == 'r') &&
					(*(pp_bff+i+j+4) == 'a') &&
					(*(pp_bff+i+j+5) == 't'))
				{
					switch (j)
					{
						case 30:	// vrai
							strncpy(pp_prj, pp_bff+i, 10);
							pp_prj[10] = 0;
							strncpy(pp_vrs, pp_bff+i+11, 10);
							pp_vrs[10] = 0;
							strncpy(pp_dat, pp_bff+i+22, 7);
							pp_dat[7] = 0;
							break;
						case 28:	// presque
							strncpy(pp_prj, pp_bff+i, 10);
							pp_prj[10] = 0;
							strncpy(pp_vrs, pp_bff+i+11, 8);
							pp_vrs[8] = 0;
							strncpy(pp_dat, pp_bff+i+20, 7);
							pp_dat[7] = 0;
							break;
						case 20:	// intermediaire
							strncpy(pp_prj, pp_bff+i, 10);
							pp_prj[10] = 0;
							strncpy(pp_vrs, pp_bff+i+11, 8);
							pp_vrs[8] = 0;
							strcpy(pp_dat, "?");
							break;
						case 13:	// ancienne
							strncpy(pp_prj, pp_bff+i, 8);
							pp_prj[8] = 0;
							sprintf(pp_vrs, "%d.%d", *(pp_bff+i+9), *(pp_bff+i+10));
							sprintf(pp_dat, "%d/%d", *(pp_bff+i+11), *(pp_bff+i+12));
							break;
						default:
							strcpy(pp_prj, "???");
							strcpy(pp_vrs, "?.?");
							strcpy(pp_dat, "?/?");
							break;
					}
					break;
				}
			}
			if (j != 32)
				break;
		}
	}
		
	return((j==32)?1:0);
}
