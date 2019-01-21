
//
// Bibliotheque LOG
//
// nom des fichiers log: /var/local/log/NOM_PRJ* en fonction de lg.nmb
//
// lg.log: FILE * du fichier log
// lg.ntr: niveau de trace general souhaite
// lg.grv: gravite du texte
// 			0-3
//			0 intervention obligatoire
//			1 erreur
//			2 attention
//			3 information
// lg.prj: nom du projet
// lg.unt: nom de unite
// lg.fnc: nom de la fonction
// lg.mss: message a logger
// lg.err: erreur a logger
// lg.tll: taille max du fichier log
// lg.nmb: nombre de fichier log
//
// contenu des fichiers log:
// 1: date
// 2: heure
// 3: chaine d'* en fonction de la gravite nombre=lg.grv
// 4: separateur
// 5: projet (programme)
// 6: separateur
// 7: unite (source)
// 8: separateur
// 9: fonction
// a: separateur
// b: message
// c: erreur
//
// separateur = " - "
//

#include "bbllg.h"

LG lg;

int  lg_cns(void);
void lg_ecr(void);
void lg_dst(void);
void lg_strcpy(char *dst, char *src);
void lg_strcat(char *dst, char *src);
void lg_strncpy(char *dst, char *src, int lng);

int lg_cns()
{
	struct stat st;
	int i;
	
	if (lg.prj[0] == 0)
	{
		strcpy(lg.err, "lg.prj n'a pas ete initialise");
		return(-1);
	}
	if ((lg.nmb != 0) && (lg.tll == 0))
	{
		strcpy(lg.err, "lg.tll n'a pas ete initialise");
		return(-1);
	}
		
	lg_strcpy(lg.nom, "/var/local/log/");
	lg_strcat(lg.nom, lg.prj);
	if (lg.nmb == 0)
	{
		lg_strcat(lg.nom, ".0");
		if ((lg.log = fopen(lg.nom, "w")) != NULL)
			fclose(lg.log);
	}
	else
	{
		lg_strcat(lg.nom, ".0");
		for (i=1; i<=lg.nmb; i++)
		{
			lg.nom[strlen(lg.nom)-1] = '0' + i;
			if (stat(lg.nom, &st) != 0)
				break;
			if (st.st_size < lg.tll)
				break;
		}
		if (st.st_size >= lg.tll)
		{
			lg.nom[strlen(lg.nom)-1] = '1';
			if ((lg.log = fopen(lg.nom, "w")) != NULL)
				fclose(lg.log);
		}
	}
		
	return(0);
}

void lg_ecr()
{
	time_t tt;
	struct tm *tm;
	struct stat st;
	struct timeval tv;
	int i;
	char sdt[24], mls[4], etl[]="***\0+++\0---\0...";
		
	if (lg.grv > lg.ntr)
		return;
	
	// ouverture fichier log
	
	if ((lg.log = fopen(lg.nom, "a")) == NULL)
		return;
		
	tt = time(NULL);
	tm = localtime(&tt);
	gettimeofday(&tv, 0);

	strftime(sdt, sizeof(sdt), "%d/%m/20%y %H:%M:%S.", tm);
	sprintf(mls, "%03d", tv.tv_usec / 1000);
	strcat(sdt, mls);
	fprintf(lg.log, "%s %3s - %s - %s - %s - %s %s\n", 
		sdt, &etl[lg.grv*4], lg.prj, lg.unt, lg.fnc, lg.mss, lg.err);
	
	// fermeture fichier log
	
	if (lg.log != NULL)
		fclose(lg.log);
		
	if (lg.nmb != 0)
	{
		if (stat(lg.nom, &st) == 0)
		{
			if (st.st_size >= lg.tll)
			{
				i = lg.nom[strlen(lg.nom)-1] - '0';
				i++;
				if (i > lg.nmb)
					i = 1;
				lg.nom[strlen(lg.nom)-1] = '0' + i;
				if ((lg.log = fopen(lg.nom, "w")) != NULL)
					fclose(lg.log);
			}
		}
	}
}

void lg_dst()
{
}

void lg_strcpy(char *dst, char *src)
{
	int l = 31;

	if (src == NULL)
	{
		dst[0] = 0;
		return;
	}

	if ((lg.mss == dst) || (lg.err == dst))
		l = 127;
	strncpy(dst, src, l);
	dst[l] = 0;
}

void lg_strcat(char *dst, char *src)
{
	int t, l = 31;

	if (src == NULL)
		return;

	if ((lg.mss == dst) || (lg.err == dst))
		l = 127;
	t = strlen(dst);
	strncpy(dst+t, src, l-t);
	dst[t+strlen(src)] = 0;
}

void lg_strncpy(char *dst, char *src, int lng)
{
	int l = 31;

	if (src == NULL)
	{
		dst[0] = 0;
		return;
	}

	if ((lg.mss == dst) || (lg.err == dst))
		l = 127;
	if (lng < l)
		l = lng;
	strncpy(dst, src, l);
	dst[l] = 0;
}

