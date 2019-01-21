
#ifndef BBLEN_H
#define BBLEN_H 1

typedef struct _en
{
	int  oto;		// -auto
	int  ntr;		// niveau trace
	char *hom;		// HOME
	char *usr;		// USER
	char *cnf;		// configuration
	char *icn;		// icon
	char *fch;		// fichier
	char *log;		// log
	char *www;		// www
	char *ubngtk;	// dvl/gtk
	char *wndc;		// dvl/windows/c
	char *wndcpp;	// dvl/windows/cpp
	char *avrc;		// dvl/avr/c
	char *avrasm;	// dvl/avr/asm
	char *avrhex;	// dvl/avr/hex
	char *avrinc;	// dvl/avr/hex
	char *armc;		// dvl/arm/c
	char *armasm;	// dvl/arm/asm
	char *armhex;	// dvl/arm/hex
	char *bin;		// usr/bin
	char *app;		// usr/local/bin
} EN;

extern EN en;

extern void en_get(int, char **, char **);

#endif

