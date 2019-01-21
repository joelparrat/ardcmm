
#ifndef BBLAR_H
#define BBLAR_H 1

#include <stdio.h>
#include <string.h>

#ifndef CLEF
#define CLEF 9999	// clef pour la memoire partagee
#endif

#ifndef DSSICN
#define DSSICN "/var/local/icn"			// nom du dossier icon
#endif

#ifndef DSSFCH
#define DSSFCH "/var/local/fch/ard"		// nom du dossier fichier (pour le projet ard)
#endif

#ifndef ACK
#define ACK			6
#endif
#ifndef NACK
#define NACK		0x15
#endif

#ifndef LNGSND
#define LNGSND 19	// longueur send
#endif
#ifndef LNGRCV
#define LNGRCV 19	// longueur receive
#endif

// description contenu de la table (srt[.][]) sortie analogique (pwm)

#ifndef LBT
#define LBT 0		// pwm luminosite boitier (a)
#endif
#ifndef VBT
#define VBT 1		// pwm ventilo boitier (b)
#endif
#ifndef VR1
#define VR1 2		// pwm ventilo radiateur.1 (c)
#endif
#ifndef VR2
#define VR2 3		// pwm ventilo radiateur.2 (d)
#endif
#ifndef PR1
#define PR1 4		// pwm peltier reservoir.1 (e)
#endif
#ifndef PR2
#define PR2 5		// pwm peltier reservoir.2 (f)
#endif
#ifndef LDB
#define LDB 6		// dgt led bleue (g)
#endif
#ifndef LDV
#define LDV 7		// dgt led verte (h)
#endif
#ifndef LDR
#define LDR 8		// dgt led rouge (i)
#endif
#ifndef ERR
#define ERR	9		// dgt erreur debit pompe (j)
#endif
#ifndef PMP
#define PMP	10		// dgt pilotage (m/a) pompe (k)
#endif
#ifndef SRC
#define SRC 11		// sortie 12 (l)
#endif
#ifndef SRD
#define SRD 12		// sortie 13 (m)
#endif
#ifndef SRE
#define SRE 13		// sortie 14 (n)
#endif
#ifndef SRT
#define SRT 14		// dernier numero pour table
#endif

// description contenu de la table (srt[][.]) mesure/parametre

#ifndef ITP
#define ITP 0		// (a) type d'entree NO=0 DGT=1 ANL=2 OWR=4 INT=8 I2C=16 ...
#endif
#ifndef IBR
#define IBR 1		// (b) numero broche de l'entree (0-255)
#endif
#ifndef IMN
#define IMN	2		// (c) seuil mini comparaison de l'entree (mesure mini)
#endif
#ifndef IMX
#define IMX 3		// (d) seuil maxi comparaison de l'entree (mesure maxi)
#endif
#ifndef OTP
#define OTP 4		// (e) type de sortie NO=0 OFF=1 ON=2 DGT=4 PWM=8 ...
#endif
#ifndef OBR
#define OBR 5		// (f) numero de broche de la sortie (0-255)
#endif
#ifndef OMN
#define OMN 6		// (g) seuil mini comparaison de sortie (commande mini)
#endif
#ifndef OMX
#define OMX 7		// (h) seuil maxi comparaison de sortie (commande maxi)
#endif
#ifndef NEN
#define NEN 8		// (i) nom des entrees
#endif
#ifndef NCP
#define NCP 9		// (j) nom des capteur
#endif
#ifndef NSR
#define NSR 10		// (k) nom des sorties
#endif
#ifndef RSR
#define RSR 11		// (l) reserve
#endif
#ifndef MSR
#define MSR 12		// (m) mesure (dans srt mais pas en eeprom)
#endif
#ifndef CMM
#define CMM 13		// (n) commande (dans srt mais pas en eeprom)
#endif
#ifndef PRM
#define PRM 14		// *** dernier numero define pour table ***
#endif

#define ITP_NO  0
#define ITP_DGT 1
#define ITP_ANL 2
#define ITP_OWR 4

#define OTP_NO  0
#define OTP_OFF 1
#define OTP_ON  2
#define OTP_DGT 4
#define OTP_PWM 8

/*
#ifndef Prm 
typedef struct _prm
{
	int srt;	// semaphore maj srt (ardcmm->i+j ardprm->0)
} Prm;
#endif

#ifndef Srv
typedef struct _srv
{
	int srt;	// semaphore maj srt (ardcmm->i+j ardsrv->0)
} Srv;
#endif
*/
#ifndef Ar 
typedef struct _ar
{
	//Prm prm;
	//Srv srv;
	int flg;	// prm->1 srv->2 cmm->4 sml->8
	
	int cnf;	// semaphore maj cnn|crt|lsn|tty|prt (ardcmm->3 ardprm->-1 ardsrv->-2)
	int cnn;	// connection
	int crt;	// carte
	int lsn;	// liaison
	int tty;	// ttyS0 ...
	int prt;	// port serveur web
	
	int maj[SRT];	// semaphore maj srt (ardcmm->1pourITP 2pourIBR 4pourIMN ... sur maj[srt] ET ardprm->-1 -2 ... ardsrv pas concerne affiche toujours toute la table)
	unsigned short srt[SRT+1][PRM+1];
	
	int env;	// semaphore maj snd (demande d'envoie ardprm->1 || demande d'envoie ardsrv->2 || demande d'envoie ardcmm->4) (envoie ardcmm |= 8) (reponse ack ardcmm->0)
	char snd[LNGSND];
	
	int rcp;	// semaphore maj rcv (ardcmm->3 ardprm->-1 ardsrv->-2)
	char rcv[LNGRCV];
} Ar;
#endif

extern void *gp_mmr;						// memoire partagee
extern Ar *ar;

extern char gr_srt[SRT+1][4];
extern char gr_prm[PRM+1][4];

extern unsigned char gr_prt[9];
extern unsigned char gr_lng[21];

extern unsigned char gc_pwm[6];
extern unsigned char gc_brcpwm[12];

extern void fv_trdmss(char *);				// traduit message

#endif

