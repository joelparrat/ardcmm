
#include "bblio.h"

char *io_bfffch(char *, int *);
LS   *io_lstfch(char *, char);
int  io_ecrbff(char *, char *, int);
void io_frebff(char *);
int  io_cpsd(char *, char *);
int  io_mvsd(char *, char *);

// Bufferise un fichier

char *io_bfffch(char *pr_nomfch, int *pi_crclus)
{
	struct stat ls_stt;
	char *lr_bfffch;
	int  lo_fch;
	
	lg_strcpy(lg.unt, "bblio");
	lg_strcpy(lg.fnc, "io_bfffch");
	
	*pi_crclus = 0;
	if (access(pr_nomfch, F_OK) == -1)
	{
		//fprintf(stderr, "bblio io_bfffch access(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	if (stat(pr_nomfch, &ls_stt) == -1)
	{
		//fprintf(stderr, "bblio io_bfffch stat(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	if ((lr_bfffch = malloc(ls_stt.st_size+1)) == NULL)
	{
		//fprintf(stderr, "bblio io_bfffch malloc(%ld): %s\n", ls_stt.st_size+1, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	if ((lo_fch = open(pr_nomfch, O_RDONLY)) == -1)
	{
		//fprintf(stderr, "bblio io_bfffch open(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		free(lr_bfffch);
		return(NULL);
	}
	if ((*pi_crclus = read(lo_fch, lr_bfffch, ls_stt.st_size)) != ls_stt.st_size)
	{
		//fprintf(stderr, "bblio io_bfffch read(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		close(lo_fch);
		free(lr_bfffch);
		return(NULL);
	}
	lr_bfffch[ls_stt.st_size] = 0;
	close(lo_fch);
	//lg_strcpy(lg.mss, pr_nomfch);
	//sprintf(lg.err, "lus=%d", *pi_crclus);
	//lg_ecr();
	
	return(lr_bfffch);
}

//
// garnit une liste avec un fichier
//
// pr_nomfch: nom du fichier a charger dans la liste
// pc: caractere = separateur dans le fichier
//

LS *io_lstfch(char *pr_nomfch, char pc)
{
	FILE *lo_fch;
	char *lr_lgn;
	char *d, *f;
	char txt[8][80];
	int  i;
	LS   *ls;
	
	lg_strcpy(lg.unt, "bblio");
	lg_strcpy(lg.fnc, "io_lstfch");
	
	ls = ls_cns();
	
	if (access(pr_nomfch, F_OK) == -1)
	{
		fprintf(stderr, "bblio io_lstfch access(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	if ((lr_lgn = malloc(LNGLGN)) == NULL)
	{
		fprintf(stderr, "bblio io_lstfch malloc(%d): %s\n", LNGLGN, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	if ((lo_fch = fopen(pr_nomfch, "r")) == NULL)
	{
		fprintf(stderr, "bblio io_lstfch fopen(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		lg_strcpy(lg.mss, pr_nomfch);
		lg_strcpy(lg.err, strerror(errno));
		lg_ecr();
		return(NULL);
	}
	while (fgets(lr_lgn, LNGLGN, lo_fch))
	{
		printf("bblio: ");
		if ((d = strchr(lr_lgn, '\n')) != NULL)
			*d = 0;
		if ((d = strchr(lr_lgn, '\r')) != NULL)
			*d = 0;
		if ((lr_lgn[0] == 0) || (lr_lgn[0] == '#'))	// vide || commentaire
		{
			printf("\n");
			continue;
		}
//ADC;ADC Rd, Rs;0001 11sd dddd ssss;Add with Carry Two Registers;2;w				
		for (i=0, d=lr_lgn; f=strchr(d, pc); i++, d=f+1)
		{
			strncpy(txt[i], d, f-d);
			txt[i][f-d] = 0;
			printf("%d=%s ", i, txt[i]);
		}
		strcpy(txt[i], d);
		
		printf("%d=%s\n", i, txt[i]);
		switch (i)
		{
			case 0:
				ls_asc(ls, "%s", txt[0]);
				break;
			case 1:
				ls_asc(ls, "%s %s", txt[0], txt[1]);
				break;
			case 2:
				ls_asc(ls, "%s %s %s", txt[0], txt[1], txt[2]);
				break;
			case 3:
				ls_asc(ls, "%s %s %s %s", txt[0], txt[1], txt[2], txt[3]);
				break;
			case 4:
				ls_asc(ls, "%s %s %s %s %s", txt[0], txt[1], txt[2], txt[3], txt[4]);
				break;
			case 5:
				ls_asc(ls, "%s %s %s %s %s %s", txt[0], txt[1], txt[2], txt[3], txt[4], txt[5]);
				break;
			case 6:
				ls_asc(ls, "%s %s %s %s %s %s %s", txt[0], txt[1], txt[2], txt[3], txt[4], txt[5], txt[6]);
				break;
			case 7:
				ls_asc(ls, "%s %s %s %s %s %s %s %s", txt[0], txt[1], txt[2], txt[3], txt[4], txt[5], txt[6], txt[7]);
				break;
		}
	}
	fclose(lo_fch);
	
	return(ls);
}

// Ecrit un buffer

int io_ecrbff(char *pr_nomfch, char *pr_bfffch, int pi_ecrnmb)
{
	int  lo_fch, li_ecrnmb;
	
	lg_strcpy(lg.unt, "bblio");
	lg_strcpy(lg.fnc, "io_ecrbff");
	
	if ((lo_fch = open(pr_nomfch, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
	{
		fprintf(stderr, "bblio io_ecrbff open(\"%s\"): %s\n", pr_nomfch, strerror(errno));
		return(0);
	}
	if ((li_ecrnmb = write(lo_fch, pr_bfffch, pi_ecrnmb)) == -1)
	{
		fprintf(stderr, "bblio io_ecrbff write(\"%s\"): %s\n", pr_nomfch, strerror(errno));
	}
	close(lo_fch);
	return(li_ecrnmb);
}

// Libere la memoire

void io_frebff(char *pr_bfffch)
{
	if (pr_bfffch != NULL)
		free(pr_bfffch);
	//lg_strcpy(lg.mss, "free()");
	//sprintf(lg.err, "ok");
	//lg_ecr();
}

// Copie un fichier source vers un fichier destination
int io_cpsd(char *pr_src, char *pr_dst)
{
	pid_t cpid;
	int stt;
	
	cpid = fork();
	if (cpid == 0)	// child
	{
		execl("/bin/cp", "cp", "--preserve", pr_src, pr_dst, (char *) NULL);
	}
	else	// parent
	{
    	do 
    	{
    		waitpid(cpid, &stt, WUNTRACED | WCONTINUED);
    	} while (!WIFEXITED(stt) && !WIFSIGNALED(stt));
    	
    	if (WIFEXITED(stt)) 
    	{
    		if (WEXITSTATUS(stt) != 0)	// erreur cp
				return(true);
			else
				return(false);
		}
	}	
	return(true);
}

// Move un fichier(dossier) source vers un fichier(dossier) destination
int io_mvsd(char *pr_src, char *pr_dst)
{
	pid_t cpid;
	int stt;
	
	cpid = fork();
	if (cpid == 0)	// child
	{
		execl("/bin/mv", "mv", "-n", pr_src, pr_dst, (char *) NULL);
	}
	else	// parent
	{
    	do 
    	{
    		waitpid(cpid, &stt, WUNTRACED | WCONTINUED);
    	} while (!WIFEXITED(stt) && !WIFSIGNALED(stt));
    	
    	if (WIFEXITED(stt)) 
    	{
    		if (WEXITSTATUS(stt) != 0)	// erreur mv
				return(true);
			else
				return(false);
		}
	}	
	return(true);
}
