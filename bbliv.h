
#ifndef BBLIV_H
#define BBLIV_H 1

#ifndef LNGPRJ
#define LNGPRJ 11
#endif
#ifndef LNGVRS
#define LNGVRS 11
#endif

#ifndef IV
typedef struct _iv
{
	char db[5];
	char nm[11];	// projet
	char vr[11];	// version
	char dt[8];
	char fn[7];
} IV;
#endif

extern IV iv;
extern int iv_info(char *, int, char *, char *, char *);

#endif

