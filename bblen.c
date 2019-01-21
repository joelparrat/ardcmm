
// nom: bblen.c
// long: Bibliotheque variables d'Environement

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbliv.h"
#include "bblen.h"

void en_get(int, char **, char **);

//
// Lecture et affectation des variables d'environement dans la struct en
//

void en_get(int pi_argc, char **pp_argv, char **pp_envv)
{
	int i;
	
	memset(&en, 0, sizeof(en));
	if (pi_argc > 1)
		if (strcmp(*(pp_argv+1), "-auto") == 0)
			en.oto = 1;

	for (i=0; *(pp_envv+i); i++)
	{
		if (strncmp(*(pp_envv+i), "NIVTRC=", 7) == 0)
			en.ntr = atoi((*(pp_envv+i))+7);
		if (strncmp(*(pp_envv+i), "HOME=", 5) == 0)
			en.hom = (*(pp_envv+i))+5;
		if (strncmp(*(pp_envv+i), "USER=", 5) == 0)
			en.usr = (*(pp_envv+i))+5;
		if (strncmp(*(pp_envv+i), "DSSCNF=", 7) == 0)
			en.cnf = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "DSSICN=", 7) == 0)
			en.icn = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "DSSFCH=", 7) == 0)
			en.fch = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "DSSLOG=", 7) == 0)
			en.log = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "DSSWWW=", 7) == 0)
			en.www = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "UBNGTK=", 7) == 0)
			en.ubngtk = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "WNDC=", 5) == 0)
			en.wndc = (*(pp_envv+i))+5;
		if (strncmp(*(pp_envv+i), "WNDCPP=", 7) == 0)
			en.wndcpp = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "AVRC=", 5) == 0)
			en.avrc = (*(pp_envv+i))+5;
		if (strncmp(*(pp_envv+i), "AVRASM=", 7) == 0)
			en.avrasm = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "AVRHEX=", 7) == 0)
			en.avrhex = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "AVRINC=", 7) == 0)
			en.avrinc = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "ARMC=", 5) == 0)
			en.armc = (*(pp_envv+i))+5;
		if (strncmp(*(pp_envv+i), "ARMASM=", 7) == 0)
			en.armasm = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "ARMHEX=", 7) == 0)
			en.armhex = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "EXEBIN=", 7) == 0)
			en.bin = (*(pp_envv+i))+7;
		if (strncmp(*(pp_envv+i), "EXEAPP=", 7) == 0)
			en.app = (*(pp_envv+i))+7;
	}

#ifdef BBLIV_H
	if (strcmp(*pp_argv, iv.nm) != 0)
		printf("IV %s != %s\n", *pp_argv, iv.nm);
	else
		printf("IV %s %s/%s %s%s %s\n", en.usr, en.app, iv.nm, (en.oto)?"-auto ":"", iv.vr, iv.dt);
#else
		printf("!IV %s %s/%s %s\n", en.usr, en.app, *pp_argv, (en.oto)?"-auto","");
#endif
}
