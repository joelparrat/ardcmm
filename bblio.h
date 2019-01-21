
#ifndef BBLIO_H
#define BBLIO_H 1

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "bbllg.h"
#include "bblls.h"

#ifndef false
#define false 0
#endif
#ifndef true
#define true !false
#endif
#ifndef LNGLGN
#define LNGLGN 256
#endif

extern char *io_bfffch(char *, int *);
extern LS   *io_lstfch(char *, char);
extern int  io_ecrbff(char *, char *, int);
extern void io_frebff(char *);
extern int  io_cpsd(char *, char *);
extern int  io_mvsd(char *, char *);

#endif

