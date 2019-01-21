
#ifndef BBLLG_H
#define BBLLG_H 1

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#ifndef LG_SYS
#define LG_SYS 0
#endif
#ifndef LG_ERR
#define LG_ERR 1
#endif
#ifndef LG_ATT
#define LG_ATT 2
#endif
#ifndef LG_INF
#define LG_INF 3
#endif

#ifndef LG
typedef struct _lg
{
	FILE *log;
	int  ntr;
	int  grv;
	char nom[32];
	char prj[32];
	char unt[32];
	char fnc[32];
	char mss[128];
	char err[128];
	int  tll;
	char nmb;
} LG;
#endif

extern LG lg;

extern int  lg_cns(void);
extern void lg_ecr(void);
extern void lg_dst(void);
extern void lg_strcpy(char *dst, char *src);
extern void lg_strcat(char *dst, char *src);
extern void lg_strncpy(char *dst, char *src, int lng);
#endif

