
#ifndef BBLHT_H
#define BBLHT_H 1

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "bblio.h"
#include "bbllg.h"
#include "bblls.h"

#ifndef false
#define false 0
#endif
#ifndef true
#define true !false
#endif
#ifndef LNGSTR
#define LNGSTR 256
#endif

extern int  ht_sntfnc(char *, char *);
extern int  ht_nomdoc(char *, char *);
extern int  ht_read(char *, char *, char *);
extern int  ht_rmptxt(char *pr_bff, char *pr_txt, char *pr_atr, int pi_nmb);

#endif

