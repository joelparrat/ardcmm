
#include "intgrp.h"

#define BD B115200

int  gi_ind=0;								// index pour table tty dans le cas du RS232
// non connecte = 0 ... access dev ok = 1 ... open dev ok = 2 ... dsr ok 3 ... cts ok 4 --> connecte = 5 :::: fin = 9
int  gi_phs;								// phase de connection
char gc_clr;								// couleur voyant connection (g, r, m, o, c, b, v)
LS   *lstprt;								// contenu de controleur.brc mis en forme (port digital analogic timer)
LS   *lstbrc;								// contenu de controleur.brc (brochage)
int  go_dev;								// open port serie
char gr_dev[32];							// open port serie
char gr_snd[LNGSND+1];						// buffer envoi
char gr_rcp[LNGRCV+6];						// buffer de reception
int  gi_rcp;								// indice gr_rcp
char gr_rcv[LNGRCV+1];						// message recu sans le protocole
int  gi_rcv;								// message recu a traiter
int  gi_anl;								// analogique affichage dynamique (nombre a afficher)
char gr_anl[90];							// adc actif + correspondance portbit/n°adc
char gr_dgt[90];							// dgt revennu actif suite suppression adc
char gr_brc[12][8][8];						// broche uniquement analogique
char gr_ard[12][8][8];						// Broche reserve arduino --> disable (broche + io)
char gr_baf[12][8][32];						// l'affichage des broches
char gr_bhl[12][8][32];						// l'aide des broches
char gr_hlp[12][8][32];						// l'aide des io
int  gi_maj[SRT]; 							// tableau maj
char gc_savett;								// sauvegarde valeur NO ETAT precedent
unsigned char gc_12[3];
char gc_dgt[11][3] = {"", "", "", "", "", "", "c0", "c1", "c2", "d4", "b0"};
char gc_brcspn[12] = {0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1};
//int  gi_clr;								// flag memorisation fin init changement couleur (rouge) si changement combo
// memorisation tableau parametre = representation de ar->srt[][]
// mmysrt[.][] = colonne parametre (ind sortie etat ... mmn mmx f )
// mmysrt[][.] = ligne (ind) 1...B
struct _mmysrt
{
	GdkRGBA oclr;
	GdkRGBA nclr;
	char ovlr[17];
	char nvlr[17];
} mmysrt[16][SRT+1];
//unsigned char ar->srt[SRT+1][PRM+1];
//unsigned char gc_brcpwm[12] = {0, 0, 0, 0, 0, 1, 2, 0, 0, 3, 4, 5};

int  fi_sndmss(void);
int	 fi_sndi(void);
int	 fi_sndam(void);
int  fi_appcbt(char *, GtkComboBoxText *, char *);
int  fi_indcbt(char *, GtkComboBoxText *);
int  fi_gstmcu(const char *);
int  fi_gstlsn(const char *);
int  fi_gstdev(const char *);
int  fi_gstprt(const char *);
int  fi_getcnf(char *, char **);
void fv_majcnf(char *, char *);
void fv_affdnm(void);
void fv_majprm(void);
gboolean cb_vnt(gpointer);
gboolean cb_fin(gpointer);
gboolean cb_rcv(gpointer);
void fv_rcv_1c_off();
void fv_rcv_1c_on();
void fv_rcv_1c_msr_off();
void fv_rcv_1c_msr_on();
void fv_rcv_1c_cmm_off();
void fv_rcv_1c_cmm_on();
void fv_rcv_2c();
unsigned char *fp_ct12(unsigned char);

// Constructeur variable

void fv_cns_vrb(void)
{
	Wnd *wnd = &WND;
	char lr_nomfch[80], lr_dev[16];
	char *p, *lp_nommcu, *lp_nomlsn, *lp_nomdev, *lp_nomprt;
	gchar *txt;
	const gchar *lbl;
	int y, i, j, n, li_shm;
	
	// computation going on...
	//while (gtk_events_pending())
	//	gtk_main_iteration();
	// ...computation continued

	lg.ntr = 5;
	lg.grv= LG_INF;
	lg_strcpy(lg.prj, "ardcmm");
	lg.tll = 100000;
	lg.nmb = 3;
	lg_cns();

	gi_rcp = gi_rcv = 0;
	go_dev = -1;
	gr_snd[0] = gr_rcv[0] = 0;
	memset(gr_rcp, 0, LNGRCV+5);
	gc_savett = 0;
	gc_12[2] = 0;
	
	lstprt = ls_cns();
	lstbrc = ls_cns();
	
	if ((li_shm = shmget(CLEF, sizeof(Ar), IPC_CREAT /*| IPC_EXCL*/ | 0660)) == -1)
	{
		printf("Erreur shmget() ... Arret.\n");
		exit(-1);
	}
	
	gp_mmr = shmat(li_shm, 0, 0);
	ar = (Ar *) gp_mmr;
	ar->env = 0;
	for (i=0; i<SRT; i++)
		ar->maj[i] = 0;
	
	// garni les combobox fixe
	
	sprintf(lr_nomfch, "%s/controleur.lst", DSSFCH);
	if (fi_getcnf("mcu", &lp_nommcu) != -1)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtmcu, lp_nommcu);
	if (lp_nommcu)
		free(lp_nommcu);
	sprintf(lr_nomfch, "%s/liaison.lst", DSSFCH);
	if (fi_getcnf("liaison", &lp_nomlsn) != -1)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtlsn, lp_nomlsn);
	if (fi_getcnf("dev", &lp_nomdev) != -1)
	{
		if (!strcmp(lp_nomlsn, "RS232"))
		{
			for (i=0,j=-1; i<8; i++)
			{
				sprintf(lr_dev, "/dev/ttyS%d", i);
				if (!access(lr_dev, F_OK))
				{
					gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cbtdev, &lr_dev[5]);
					j++;
					if (lp_nomdev)
						if (*(lp_nomdev+4) == ('0' + i))
							gtk_combo_box_set_active((GtkComboBox *) wnd->cbtdev, j);
				}
			}
			if (j == -1)
			{
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cbtdev, "Aucun");
				gtk_combo_box_set_active((GtkComboBox *) wnd->cbtdev, 0);
			}
		}
		if (!strcmp(lp_nomlsn, "USB"))
		{
			for (i=0,j=-1; i<8; i++)
			{
				sprintf(lr_dev, "/dev/ttyUSB%d", i);
				if (!access(lr_dev, F_OK))
				{
					gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cbtdev, &lr_dev[5]);
					j++;
					if (lp_nomdev)
						if (*(lp_nomdev+6) == ('0' + i))
							gtk_combo_box_set_active((GtkComboBox *) wnd->cbtdev, j);
				}
			}
			if (j == -1)
			{
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cbtdev, "Aucun");
				gtk_combo_box_set_active((GtkComboBox *) wnd->cbtdev, 0);
			}
		}
		if (!strcmp(lp_nomlsn, "Aucune"))
		{
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cbtdev, "Aucun");
			gtk_combo_box_set_active((GtkComboBox *) wnd->cbtdev, 0);
		}
	}
	if (lp_nomdev)
		free(lp_nomdev);
	if (lp_nomlsn)
		free(lp_nomlsn);
	sprintf(lr_nomfch, "%s/port.lst", DSSFCH);
	if (fi_getcnf("prt", &lp_nomprt) != -1)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtprt, lp_nomprt);
	if (lp_nomprt)
		free(lp_nomprt);
	
	// garni memoire partagee en fonction de l'eeprom
	
	fv_majprm();

	for (y=0; y<SRT; y++)
		gi_maj[y] = 0;

	gi_phs = 0;
	gc_clr = 'g';
	g_timeout_add_seconds(1, cb_vnt, NULL);
	
	ar->flg |= 4;
}

// Destructeur variable

void fv_dst_vrb(void)
{
	printf("Arret dans 5s ...\n");
	gi_phs = -1;
	ar->cnn = gi_phs;
	ar->cnf = 3;
	g_timeout_add_seconds(5, cb_fin, NULL);
}

// arret programme

gboolean cb_fin(gpointer dnn)
{
	if (go_dev != -1)
		close(go_dev);
	
	ls_dst(lstprt);
	ls_dst(lstbrc);
	
	ar->flg &= ~4;

	if (gp_mmr != NULL)
		shmdt(gp_mmr);
	
	gtk_main_quit();
	return(FALSE);
}

//
// Envoi commande
//
// retour: -1 erreur_send 1 erreur_receive 0 ok_send_receive

int fi_sndmss(void)
{
	int  err=0;

	// Traduction du message
	
	fv_trdmss(gr_snd);
	
	// Controle du message
	
	switch (gr_snd[0])
	{
		case '{':
			err = fi_sndi();
			break;
		case '}':
			err = fi_sndam();
			break;
		case '|':
			err = fi_sndam();
			break;
		default:
			err = -1;
			printf("Type de commande inconnue !!!\n");
			break;
	}

	if (err != 0)
		return(err);

	// ajout du protocol
	
	write(go_dev, &gr_prt[0], 1);																					// caractere de debut
	write(go_dev, &gr_lng[strlen(gr_snd)], 1);																		// longueur message (sans protocol)
	write(go_dev, "j!!", 3);																						// groupe + adresse	
	
	// envoi le message
	
	if (go_dev != -1)
		if (write(go_dev, gr_snd, strlen(gr_snd)) != strlen(gr_snd))													// envoi commande
			return(-1);
	
	// ajout du protocol
	
	write(go_dev, &gr_prt[1], 1);
	
	return(0);
}

// Interrogation

int fi_sndi(void)
{
	int err=0;
	
	switch (gr_snd[1])
	{
		case 'z':
			if (gr_snd[2] != 0)
			{
				err = -1;
				printf("Erreur longueur commande interrogation mode ventilo !!!\n");
				break;
			}
			break;
		case 'y':
			if (gr_snd[2] != 0)
			{
				err = -1;
				printf("Erreur longueur commande interrogation nombre de capteur !!!\n");
				break;
			}
			break;
		case 'x':
			if (gr_snd[2] == 0)
			{
				err = -1;
				printf("Erreur longueur commande interrogation table ar->srt[?][] !!!\n");
				break;
			}
			if (gr_snd[3] == 0)
			{
				err = -1;
				printf("Erreur longueur commande interrogation table ar->srt[][?] !!!\n");
				break;
			}
			if (gr_snd[4] != 0)
			{
				err = -1;
				printf("Erreur longueur commande interrogation table ar->srt[][]... !!!\n");
				break;
			}
			break;
		case 'w':
			if (gr_snd[2] == 0)
			{
				err = -1;
				printf("Erreur longueur commande adresse capteur manque adr[?] !!!\n");
				break;
			}
			break;
		default:
			err = -1;
			printf("Nom commande inconnue !!!\n");
			break;
	}
	
	return(err);
}

// Affectation / Memorisation
		
int fi_sndam(void)
{
	int err=0;
	
	switch (gr_snd[1])
	{
		case 'z':
			if (gr_snd[2] == 0)
			{
				err = -1;
				printf("Erreur longueur commande affectation/memorisation mode ventilo !!!\n");
				break;
			}
			if ((gr_snd[2] != '~') && (gr_snd[2] != '`'))
			{
				err = -1;
				printf("Erreur valeur d'affectation/memorisation mode ventilo !!!\n");
				break;
			}
			break;
		case 'x':
			if (gr_snd[2] == 0)
			{
				err = -1;
				printf("Erreur longueur commande affectation/memorisation table ar->srt[?][] !!!\n");
				break;
			}
			if (gr_snd[3] == 0)
			{
				err = -1;
				printf("Erreur longueur commande affectation/memorisation table ar->srt[][?] !!!\n");
				break;
			}
			if (gr_snd[4] == 0)
			{
				err = -1;
				printf("Erreur longueur commande affectation/memorisation table ar->srt[][] !!!\n");
				break;
			}
			if (gr_snd[5] == 0)																							// valeur courte
			{
				if ((gr_snd[4] != '`') && (gr_snd[4] != '~'))
				{
					if ((gr_snd[4] < 'a') || (gr_snd[4] > 't'))
					{
						err = -1;
						printf("Erreur valeur courte !!!\n");
					}
				}
			}
			else																										// valeur longue
			{
				if ((gr_snd[4] < ' ') || (gr_snd[4] > '_') || (gr_snd[5] < ' ') || (gr_snd[5] > '_'))
				{
					err = -1;
					printf("Erreur valeur longueur !!!\n");
				}
			}
			break;
		case 'w':
			if (gr_snd[2] == 0)
			{
				err = -1;
				printf("Erreur longueur commande adresse capteur manque adr[?] !!!\n");
				break;
			}
			break;
		default:
			err = -1;
			printf("Nom commande inconnue !!!\n");
			break;
	}
	
	return(err);
}

//
// rempli un combobox_text (pp_cbt) avec le contenu d'un fichier (pp_fch)
// selection d'un texte (pp_txt)
//
// retour: 0=ok 1=append_pasdeselection -1=erreur
//

int fi_appcbt(char *pp_fch, GtkComboBoxText *pp_cbt, char *pp_txt)
{
	FILE *lo;
	char *p, lr_lgn[80];
	int  id, ret;

	id = 0;
	ret = 1;
	if ((lo = fopen(pp_fch, "r")) == NULL)																				// fichier inconnu
		return(-1);
		
	while (fgets(lr_lgn, sizeof(lr_lgn), lo) != NULL)
	{
		if ((p = strchr(lr_lgn, '\n')) != NULL)
			*p = 0;
		if ((p = strchr(lr_lgn, '\r')) != NULL)
			*p = 0;
		
		if (lr_lgn[0] != 0)
		{
			gtk_combo_box_text_append_text(pp_cbt, lr_lgn);
			if (pp_txt)
			{
				if (!strcmp(pp_txt, lr_lgn))
				{
					gtk_combo_box_set_active((GtkComboBox *) pp_cbt, id);
					ret = 0;
				}
			}
			id++;
		}
	}
	//if (!pp_txt)
	//{
	//	gtk_combo_box_set_active((GtkComboBox *) pp_cbt, 0);
	//	ret = 0;
	//}
		
	fclose(lo);
	
	return(ret);
}

//
// lit un fichier (pp_fch)
// et donne l'indice du texte active du combobox
//
// retour: -1:pas_trouve 0-n=indice
//

int fi_indcbt(char *pp_fch, GtkComboBoxText *pp_cbt)
{
	FILE *lo;
	char *p, lr_lgn[80];
	int  id, ret;
	gchar *txt;

	if ((txt = gtk_combo_box_text_get_active_text(pp_cbt)) == NULL)
		return(-1);

	if ((lo = fopen(pp_fch, "r")) == NULL)																				// fichier inconnu
	{
		g_free(txt);
		return(-1);
	}
		
	id = 0;
	ret = 1;
	while (fgets(lr_lgn, sizeof(lr_lgn), lo) != NULL)
	{
		if ((p = strchr(lr_lgn, '\n')) != NULL)
			*p = 0;
		if ((p = strchr(lr_lgn, '\r')) != NULL)
			*p = 0;

		if (!strcmp(lr_lgn, txt))
		{
			ret = 0;
			break;
		}

		id++;
	}
		
	fclose(lo);
	g_free(txt);
	
	return(ret?-1:id);
}


int fi_gstmcu(const char *pp_mcu)
{
	CH   *crn;
	FILE *lo;
	char *p, *q, prt[2], mcu[16], txt[80];
	int  pos, a, b;

	// pour l'affichage de l'aide
	
	for (a=0; a<12; a++)
	{
		for (b=0; b<8; b++)
		{
			gr_brc[a][b][0] = 0;																						// pour analogique uniquement
			gr_ard[a][b][0] = 0;																						// reserve arduino
			gr_baf[a][b][0] = 0;																						// affichage broche
			gr_bhl[a][b][0] = 0;																						// aide broche
			gr_hlp[a][b][0] = 0;																						// aide bouton io
		}
	}
			
	sprintf(txt, "%s/%s.ard", DSSFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) != NULL)
	{
		while (fgets(txt, sizeof(txt), lo) != NULL)
		{
			if ((p = strchr(txt, '\n')) != NULL)
				*p = 0;
			if ((p = strchr(txt, '\r')) != NULL)
				*p = 0;

			if (strlen(txt) < 3)
				continue;
							
			if ((txt[0] != 'P') && (txt[0] != 'A'))																		// utilise le port A pour anl uniquement
				continue;
				
			if ((txt[1] < 'A') || (txt[1] > 'L'))
				continue;

			if ((txt[2] < '0') || (txt[2] > '7'))
				continue;
			
			if (txt[3] == 0)																							// broche non reservee
				continue;
				
			if (txt[3] != ' ')																							// mauvaise syntaxe
				continue;
				
			p = &txt[4];
			strncpy(gr_ard[txt[1]-'A'][txt[2]-'0'], p, 7);
			gr_ard[txt[1]-'A'][txt[2]-'0'][7] = 0;
		}
		fclose(lo);
	}
			
	sprintf(txt, "%s/%s.baf", DSSFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) != NULL)
	{
		while (fgets(txt, sizeof(txt), lo) != NULL)
		{
			if ((p = strchr(txt, '\n')) != NULL)
				*p = 0;
			if ((p = strchr(txt, '\r')) != NULL)
				*p = 0;

			if (strlen(txt) < 3)
				continue;
			
			if ((txt[0] != 'P') && (txt[0] != 'A'))
				continue;
		
			if ((txt[1] < 'A') || (txt[1] > 'L'))
				continue;

			if ((txt[2] < '0') || (txt[2] > '7'))
				continue;
			
			if (txt[3] == 0)																							// broche sans affichage
				continue;
				
			if (txt[3] != ' ')																							// mauvaise syntaxe
				continue;
		
			p = &txt[4];
			strncpy(gr_baf[txt[1]-'A'][txt[2]-'0'], p, 7);
			gr_baf[txt[1]-'A'][txt[2]-'0'][7] = 0;
		}
		fclose(lo);
	}
			
	sprintf(txt, "%s/%s.bhl", DSSFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) != NULL)
	{
		while (fgets(txt, sizeof(txt), lo) != NULL)
		{
			if ((p = strchr(txt, '\n')) != NULL)
				*p = 0;
			if ((p = strchr(txt, '\r')) != NULL)
				*p = 0;

			if (strlen(txt) < 3)
				continue;
			
			if ((txt[0] != 'P') && (txt[0] != 'A'))
				continue;
		
			if ((txt[1] < 'A') || (txt[1] > 'L'))
				continue;

			if ((txt[2] < '0') || (txt[2] > '7'))
				continue;
			
			if (txt[3] == 0)																							// broche sans affichage
				continue;
				
			if (txt[3] != ' ')																							// mauvaise syntaxe
				continue;
		
			p = &txt[4];
			strncpy(gr_bhl[txt[1]-'A'][txt[2]-'0'], p, 31);
			gr_bhl[txt[1]-'A'][txt[2]-'0'][31] = 0;
		}
		fclose(lo);
	}
			
	sprintf(txt, "%s/%s.hlp", DSSFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) != NULL)
	{
		while (fgets(txt, sizeof(txt), lo) != NULL)
		{
			if ((p = strchr(txt, '\n')) != NULL)
				*p = 0;
			if ((p = strchr(txt, '\r')) != NULL)
				*p = 0;

			if (strlen(txt) < 3)
				continue;
			
			if ((txt[0] != 'P') && (txt[0] != 'A'))
				continue;
		
			if ((txt[1] < 'A') || (txt[1] > 'L'))
				continue;

			if ((txt[2] < '0') || (txt[2] > '7'))
				continue;
			
			if (txt[3] == 0)																							// bouton sans affichage
				continue;
				
			if (txt[3] != ' ')																							// mauvaise syntaxe
				continue;
		
			p = &txt[4];
			strncpy(gr_hlp[txt[1]-'A'][txt[2]-'0'], p, 31);
			gr_hlp[txt[1]-'A'][txt[2]-'0'][31] = 0;
		}
		fclose(lo);
	}
	
	// pour l'affichage des ports digital analogic
	ls_vid(lstprt);
	ls_vid(lstbrc);
	gi_anl = 0;
	prt[1] = 0;
	gr_anl[0] = gr_dgt[0] = 0;
	
	sprintf(txt, "%s/%s.brc", DSSFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) == NULL)
		return(-1);
		
	while (fgets(txt, sizeof(txt), lo) != NULL)
	{
		if ((p = strchr(txt, '\n')) != NULL)
			*p = 0;
		
		if (strlen(txt) < 3)
			continue;
		
		if ((txt[3] != ' ') && (txt[3] != 0))
			continue;
			
		if ((txt[0] == 'A') && (txt[1] == 'A'))										// broche uniquement ANALOGIQUE ex: AA6 et AA7
		{
			strcpy(gr_brc[0][txt[2]-'0'], &txt[2]);
			continue;
		}

		ls_asc(lstbrc, "0", txt);
		
		prt[0] = tolower(txt[1]);
		ls_asc(lstprt, "0", "%s%s%s%s", prt, "........", "........", "........");
		if (ls_rtx(lstprt, prt, 0) == -1)
			continue;
		
		crn = lstprt->current;
		*((crn->mmr+(*(crn->mmr+8)))+(txt[2]-'0')) = txt[2];

		if ((p = strstr(txt, " ADC")) != NULL)
		{
			if (sscanf(p+4, "%d", &a) == 1)
			{
				if (ls_rtx(lstprt, prt, 0) != -1)
				{
					crn = lstprt->current;
					*((crn->mmr+(*(crn->mmr+16)))+(txt[2]-'0')) = (a<10)?'0'+a:'a'+a-10;
				}
			}
		}

		if ((p = strstr(txt, " OC")) != NULL)
		{
			if (sscanf(p+3, "%d", &a) == 1)
			{
				if (ls_rtx(lstprt, prt, 0) != -1)
				{
					crn = lstprt->current;
					*((crn->mmr+(*(crn->mmr+24)))+(txt[2]-'0')) = (a<10)?'0'+a:'a'+a-10;
				}
			}
		}
		if ((p = strstr(txt, " ICP")) != NULL)
		{
			if (sscanf(p+4, "%d", &a) == 1)
			{
				if (ls_rtx(lstprt, prt, 0) != -1)
				{
					crn = lstprt->current;
					*((crn->mmr+(*(crn->mmr+24)))+(txt[2]-'0')) = (a<10)?'0'+a:'a'+a-10;
				}
			}
		}
		if ((p = strstr(txt, " T")) != NULL)
		{
			if (sscanf(p+2, "%d", &a) == 1)
			{
				if (ls_rtx(lstprt, prt, 0) != -1)
				{
					crn = lstprt->current;
					*((crn->mmr+(*(crn->mmr+24)))+(txt[2]-'0')) = (a<10)?'0'+a:'a'+a-10;
				}
			}
		}
	}
	fclose(lo);

	//ls_prn(lstprt);
	//ls_prn(lstbrc);
	
	fv_affdnm();																										// affichage dynamique
	return(0);
}

int fi_gstlsn(const char *pp_lsn)
{
	return(0);
}

int fi_gstdev(const char *pp_dev)
{
	sprintf(gr_dev, "/dev/%s", pp_dev);
	
	if (gi_phs > 1)
	{
		close(go_dev);
		go_dev = -1;
	}
	gi_phs = 0;
	
	return(0);
}

int fi_gstprt(const char *pp_prt)
{
	return(0);
}

// lecture element du fichier configuration (.../cnf/ardcmm)
//
// pp_elm: l'element dans cnf  (ex:liaison)
// pp_cnt: son contenu         (ex:RS232)
//
// retour: 0=ok 1=pas_trouve -1=erreur
// attention: pp_cnt doit etre libere apres utilisation si retour ok {if (pp_cnt) free(pp_cnt)}

int fi_getcnf(char *pp_elm, char **pp_cnt)
{
	char *d, *f, *lp_bff, *lp_elm;
	int  li_lus;

	if ((lp_bff = io_bfffch(NOMCNF, &li_lus)) == NULL)																	// fichier config n'existe pas
	{
		*pp_cnt = NULL;
		return(1);
	}

	if ((lp_elm = malloc(strlen(pp_elm) + 2)) == NULL)
	{
		*pp_cnt = NULL;
		io_frebff(lp_bff);																								// libere la memoire
		return(-1);
	}
	sprintf(lp_elm, "%s=", pp_elm);	

	if ((d = strstr(lp_bff, lp_elm)) == NULL)																			// element non trouve
	{
		*pp_cnt = NULL;
		free(lp_elm);
		io_frebff(lp_bff);																								// libere la memoire
		return(1);
	}

	d += strlen(lp_elm);
	if ((f = strchr(d, '\n')) != NULL)
	{
		*pp_cnt = malloc((f-d)+1);
		strncpy(*pp_cnt, d, f-d);
		*((*pp_cnt)+(f-d)) = 0;
	}
	else
	{
		*pp_cnt = malloc(strlen(d)+1);
		strcpy(*pp_cnt, d);
	}

	free(lp_elm);
	io_frebff(lp_bff);																									// libere la memoire
	
	return(0);
}

// Mise a jour fichier configuration (.../cnf/ardcmm)
//
// pr_txt: le texte dans cnf  (ex:liaison)
// pr_cnt: son contenu        (ex:RS232)

void fv_majcnf(char *pr_txt, char *pr_cnt)
{
	FILE *fo;
	char *d, *f, *bff, lr_cnt[16], lr_txt[16];
	int  lus;

	sprintf(lr_txt, "%s=", pr_txt);	
	if ((bff = io_bfffch(NOMCNF, &lus)) == NULL)																		// fichier config n'existe pas
	{
		if ((fo = fopen(NOMCNF, "w")) != NULL)																			// creation
		{
			fputs(lr_txt, fo);
			fputs(pr_cnt, fo);
			fputs("\n", fo);
			fclose(fo);
		}
	}
	else
	{
		if ((d = strstr(bff, lr_txt)) != NULL)																			// mcu trouve
		{
			d += strlen(lr_txt);
			if ((f = strchr(d, '\n')) != NULL)
			{
				strncpy(lr_cnt, d, f-d);
				lr_cnt[f-d] = 0;
				if (strcmp(lr_cnt, pr_cnt) != 0)
				{
					ht_rmptxt(bff, lr_cnt, pr_cnt, 1);																	// remplace ancien mcu par nouveau
					io_ecrbff(NOMCNF, bff, lus-(f-d)+strlen(pr_cnt));													// ecriture
				}
			}
			else
			{
				strcpy(lr_cnt, d);
				if (strcmp(lr_cnt, pr_cnt) != 0)
				{
					ht_rmptxt(bff, lr_cnt, pr_cnt, 1);																	// remplace ancien mcu par nouveau
					io_ecrbff(NOMCNF, bff, lus-strlen(lr_cnt)+strlen(pr_cnt));											// ecriture
				}
			}
		}
		else
		{
			if ((fo = fopen(NOMCNF, "a")) != NULL)																		// pas encore de mcu -> ajoute
			{
				fputs(lr_txt, fo);
				fputs(pr_cnt, fo);
				fputs("\n", fo);
				fclose(fo);
			}
		}
		io_frebff(bff);																									// libere la memoire
	}
}

void fv_affdnm(void)
{
	CH *crn;
	Wnd *wnd = &WND;
	int  i, j, b, x, y, yd, ya, yt;
	char p, n[4], d[9], a[9], t[9], l[6], lr_tbl[16];																	// port (p) name (n) dgt (d) anl (a) pwm (t) lbl (l)
	char *ptr;
	
	if (wnd->grdowr)
		gtk_widget_destroy(wnd->grdowr);
	if (wnd->grdanl)
		gtk_widget_destroy(wnd->grdanl);
	if (wnd->grddgt)
		gtk_widget_destroy(wnd->grddgt);
	if (wnd->grdpwm)
		gtk_widget_destroy(wnd->grdpwm);
	if (wnd->grdbrc)
		gtk_widget_destroy(wnd->grdbrc);

	wnd->grdowr = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdowr, 5);
	gtk_widget_set_margin_end(wnd->grdowr, 5);
	//gtk_widget_set_margin_top(wnd->grdowr, 5);
	gtk_widget_set_margin_bottom(wnd->grdowr, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdowr, TRUE);
	gtk_container_add((GtkContainer *) wnd->frmowr, wnd->grdowr);
	gtk_widget_show(wnd->grdowr);

	wnd->grdanl = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdanl, 5);
	gtk_widget_set_margin_end(wnd->grdanl, 5);
	//gtk_widget_set_margin_top(wnd->grdanl, 5);
	gtk_widget_set_margin_bottom(wnd->grdanl, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdanl, TRUE);
	gtk_container_add((GtkContainer *) wnd->frmanl, wnd->grdanl);
	gtk_widget_show(wnd->grdanl);

	wnd->grddgt = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grddgt, 5);
	gtk_widget_set_margin_end(wnd->grddgt, 5);
	//gtk_widget_set_margin_top(wnd->grddgt, 5);
	gtk_widget_set_margin_bottom(wnd->grddgt, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grddgt, TRUE);
	gtk_container_add((GtkContainer *) wnd->frmdgt, wnd->grddgt);
	gtk_widget_show(wnd->grddgt);

	wnd->grdpwm = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdpwm, 5);
	gtk_widget_set_margin_end(wnd->grdpwm, 5);
	//gtk_widget_set_margin_top(wnd->grdpwm, 5);
	gtk_widget_set_margin_bottom(wnd->grdpwm, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdpwm, TRUE);
	gtk_container_add((GtkContainer *) wnd->frmpwm, wnd->grdpwm);
	gtk_widget_show(wnd->grdpwm);

	wnd->grdbrc = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdbrc, 5);
	gtk_widget_set_margin_end(wnd->grdbrc, 5);
	//gtk_widget_set_margin_top(wnd->grdbrc, 5);
	gtk_widget_set_margin_bottom(wnd->grdbrc, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdbrc, TRUE);
	gtk_container_add((GtkContainer *) wnd->frmbrc, wnd->grdbrc);
	gtk_widget_show(wnd->grdbrc);

	l[2] = 0;
	for (x=0; x<8; x++)
	{
		l[1] = '0' + x;
		l[0] = 'D';
		wnd->ldgtd[x] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grddgt, wnd->ldgtd[x], 9-x, 0, 1, 1);
		gtk_widget_show(wnd->ldgtd[x]);
		wnd->lbrcd[x] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grdbrc, wnd->lbrcd[x], 8-x, 0, 1, 1);
		gtk_widget_show(wnd->lbrcd[x]);
		l[0] = 'A';
		wnd->lanla[x] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->lanla[x], 9-x, 0, 1, 1);
		gtk_widget_show(wnd->lanla[x]);
		l[0] = 'T';
		wnd->lowra[x] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grdowr, wnd->lowra[x], 9-x, 0, 1, 1);
		gtk_widget_show(wnd->lowra[x]);
		l[0] = 'P';
		wnd->lpwma[x] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->lpwma[x], 9-x, 0, 1, 1);
		gtk_widget_show(wnd->lpwma[x]);
	}
	wnd->ldgtd[x] = gtk_label_new("All");
	gtk_grid_attach((GtkGrid *) wnd->grddgt, wnd->ldgtd[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->ldgtd[x]);
	wnd->lanla[x] = gtk_label_new("All");
	gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->lanla[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lanla[x]);
	wnd->lowra[x] = gtk_label_new("All");
	gtk_grid_attach((GtkGrid *) wnd->grdowr, wnd->lowra[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lowra[x]);
	wnd->lpwma[x] = gtk_label_new("All");
	gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->lpwma[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lpwma[x]);
	x++;
	wnd->lanla[x] = gtk_label_new("   ");
	gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->lanla[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lanla[x]);
	wnd->lowra[x] = gtk_label_new("   ");
	gtk_grid_attach((GtkGrid *) wnd->grdowr, wnd->lowra[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lowra[x]);
	wnd->lpwma[x] = gtk_label_new("   ");
	gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->lpwma[x], 9-x, 0, 1, 1);
	gtk_widget_show(wnd->lpwma[x]);
	
	yd = ya = 0;
	yt = -1;
	for (i=0; i<12; i++)
	{
		for (j=b=0; j<8; j++)
		{
			if (gr_brc[i][j][0] != 0)
			{
				wnd->eanl[i][j] = gtk_entry_new();																		// gtkentry pour ADC6 et ADC7 --> dans AA6 et AA7 = port A
				gtk_entry_set_width_chars((GtkEntry *) wnd->eanl[i][j], 5);
				gtk_entry_set_max_length((GtkEntry *) wnd->eanl[i][j], 5);
				n[0] = 'a' + i;
				n[1] = '0' + j;
				n[2] = gr_brc[i][j][0];
				gtk_widget_set_name(wnd->eanl[i][j], n);																// name (n) port bit numero
				gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->eanl[i][j], 9-j, ya+1, 1, 1);
				gtk_widget_show(wnd->eanl[i][j]);
				//sprintf(d, "ADC%d", (n[2]>='a')?n[2]-'a'+10:n[2]-'0');
				if (i == 0)
					gtk_widget_set_tooltip_text(wnd->eanl[0][j], gr_hlp[0][j]);
				if (gr_ard[i][j][0] == 0)
					gtk_widget_set_sensitive((GtkWidget *) wnd->eanl[i][j], true);
				else
					gtk_widget_set_sensitive((GtkWidget *) wnd->eanl[i][j], false);
				b = 1;
			}
		}

		if (b != 0)
		{		
			sprintf(l, "ADC ");
			wnd->lanlc[i] = gtk_label_new(l);
			gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->lanlc[i], 0, ya+1, 1, 1);
			gtk_widget_show(wnd->lanlc[i]);
			ya++;
		}
	}
	
	crn = lstprt->first;
	while (crn)
	{
		p = *(crn->mmr+(*crn->mmr));																					// port (p)
		strcpy(d, crn->mmr+(*(crn->mmr+8)));
		strcpy(a, crn->mmr+(*(crn->mmr+16)));
		strcpy(t, crn->mmr+(*(crn->mmr+24)));

		sprintf(l, "Port%c", toupper(p));
		wnd->ldgtp[p-'a'] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grddgt, wnd->ldgtp[p-'a'], 0, yd+1, 1, 1);
		gtk_widget_show(wnd->ldgtp[p-'a']);
		wnd->lbrcp[p-'a'] = gtk_label_new(l);
		gtk_grid_attach((GtkGrid *) wnd->grdbrc, wnd->lbrcp[p-'a'], 0, yd+1, 1, 1);
		gtk_widget_show(wnd->lbrcp[p-'a']);

		n[0] = p;
		for (b=0,n[3]=0; d[b]!=0; b++)
		{
			if (d[b] != '.')
			{
				if (((p == 'd') && (b == 4)) || ((p == 'c') && (b == 0)) || 
					((p == 'c') && (b == 1)) || ((p == 'c') && (b == 2)) ||
					((p == 'b') && (b == 5)) || ((p == 'b') && (b == 0)) ||
					((p == 'd') && (b == 3)) || ((p == 'd') && (b == 5)) || 
					((p == 'd') && (b == 6)) || ((p == 'b') && (b == 1)) ||
					((p == 'b') && (b == 2)) || ((p == 'b') && (b == 3)) ||
					((p == 'd') && (b == 1)) || ((p == 'c') && (b == 3)))
					wnd->bdgt[p-'a'][b] = gtk_button_new_with_label("O ");
				else
					wnd->bdgt[p-'a'][b] = gtk_button_new_with_label("Z ");
				if (gr_ard[p-'a'][b][0] == 0)
					gtk_widget_set_sensitive((GtkWidget *) wnd->bdgt[p-'a'][b], true);
				else
					gtk_widget_set_sensitive((GtkWidget *) wnd->bdgt[p-'a'][b], false);
				n[1] = '0' + b;
				n[2] = 'g';
				gtk_widget_set_name(wnd->bdgt[p-'a'][b], n);															// name (n) port bit couleur
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
				gtk_button_set_image((GtkButton *) wnd->bdgt[p-'a'][b], wnd->pled);
				gtk_button_set_image_position((GtkButton *) wnd->bdgt[p-'a'][b], GTK_POS_RIGHT);
				gtk_button_set_always_show_image((GtkButton *) wnd->bdgt[p-'a'][b], TRUE);
				gtk_grid_attach((GtkGrid *) wnd->grddgt, wnd->bdgt[p-'a'][b], 9-b, yd+1, 1, 1);
				gtk_widget_show(wnd->bdgt[p-'a'][b]);
				gtk_widget_set_tooltip_text(wnd->bdgt[p-'a'][b], gr_hlp[p-'a'][b]);
				g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);

				//sprintf(l, "P%c%c", toupper(p), '0'+b);
				strcpy(l, gr_baf[p-'a'][b]);
				if (ptr = strchr(l, ' '))
					*ptr = 0;
				wnd->bbrc[p-'a'][b] = gtk_button_new_with_label(l);
				if (gr_ard[p-'a'][b][0] == 0)
					gtk_widget_set_sensitive((GtkWidget *) wnd->bbrc[p-'a'][b], true);
				else
					gtk_widget_set_sensitive((GtkWidget *) wnd->bbrc[p-'a'][b], false);
				gtk_widget_set_name(wnd->bbrc[p-'a'][b], &l[1]);
				gtk_grid_attach((GtkGrid *) wnd->grdbrc, wnd->bbrc[p-'a'][b], 8-b, yd+1, 1, 1);
				gtk_widget_show(wnd->bbrc[p-'a'][b]);
				gtk_widget_set_tooltip_text(wnd->bbrc[p-'a'][b], gr_bhl[p-'a'][b]);
				g_signal_connect((GtkWindow *) wnd->bbrc[p-'a'][b], "clicked", (GCallback) on_bbrc_clicked, NULL);
			}
		}

		n[0] = p;
		for (b=0,i=0,n[3]=0; a[b]!=0; b++)
		{
			if (a[b] != '.')
			{
				i = 1;
				wnd->eanl[p-'a'][b] = gtk_entry_new();
				gtk_entry_set_width_chars((GtkEntry *) wnd->eanl[p-'a'][b], 5);
				gtk_entry_set_max_length((GtkEntry *) wnd->eanl[p-'a'][b], 5);
				n[1] = '0' + b;
				n[2] = a[b];
				gtk_widget_set_name(wnd->eanl[p-'a'][b], n);															// name (n) port bit numero
				gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->eanl[p-'a'][b], 9-b, ya+1, 1, 1);
				gtk_widget_show(wnd->eanl[p-'a'][b]);
				//sprintf(d, "ADC%d", (n[2]>='a')?n[2]-'a'+10:n[2]-'0');
				gtk_widget_set_tooltip_text(wnd->eanl[p-'a'][b], gr_bhl[p-'a'][b]);
				if (gr_ard[p-'a'][b][0] == 0)
					gtk_widget_set_sensitive((GtkWidget *) wnd->eanl[p-'a'][b], true);
				else
					gtk_widget_set_sensitive((GtkWidget *) wnd->eanl[p-'a'][b], false);
			
				//g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);
			}
		}
		if (i)
		{
			sprintf(l, "ADC%c", toupper(p));
			wnd->lanlc[p-'a'] = gtk_label_new(l);
			gtk_grid_attach((GtkGrid *) wnd->grdanl, wnd->lanlc[p-'a'], 0, ya+1, 1, 1);
			gtk_widget_show(wnd->lanlc[p-'a']);
			ya++;
		}

		n[0] = p;
		for (b=0,n[3]=0; t[b]!=0; b++)
		{
			if (t[b] != '.')
			{
				if ((t[b]-'0') > yt)
					yt = t[b] - '0';
			}
		}

		yd++;
		crn = crn->next;
	}

	for (i=0; i<8; i++)
	{
		wnd->eowr[i] = gtk_entry_new();
		gtk_entry_set_width_chars((GtkEntry *) wnd->eowr[i], 5);
		gtk_entry_set_max_length((GtkEntry *) wnd->eowr[i], 5);
		gtk_grid_attach((GtkGrid *) wnd->grdowr, wnd->eowr[i], 9-i, 1, 1, 1);
		gtk_widget_show(wnd->eowr[i]);
		sprintf(d, "D12 OneWire T%d", i);
		gtk_widget_set_tooltip_text(wnd->eowr[i], d);
	
		//g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);
	}
	sprintf(l, "OneWire");
	wnd->lowrc = gtk_label_new(l);
	gtk_grid_attach((GtkGrid *) wnd->grdowr, wnd->lowrc, 0, 1, 1, 1);
	gtk_widget_show(wnd->lowrc);

	for (i=0; i<6; i++)
	{
		wnd->epwm[i] = gtk_entry_new();
		gtk_entry_set_width_chars((GtkEntry *) wnd->epwm[i], 5);
		gtk_entry_set_max_length((GtkEntry *) wnd->epwm[i], 5);
		gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->epwm[i], 9-i, 1, 1, 1);
		gtk_widget_show(wnd->epwm[i]);
		sprintf(d, "PWM D%d", gc_pwm[i]);
		gtk_widget_set_tooltip_text(wnd->epwm[i], d);
	
		//g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);
	}
	sprintf(l, "PWM");
	wnd->lpwmc = gtk_label_new(l);
	gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->lpwmc, 0, 1, 1, 1);
	gtk_widget_show(wnd->lpwmc);
}

//
// Garni la memoire partagee en fonction de l'eeprom
//

void fv_majprm(void)
{
	Wnd *wnd = &WND;
	FILE *lo;
	char *d, *f, c, lr_nomfch[32], lr_lgn[128], vlr[8], cnt[]="JoelP.";
	int  v, p, s, r, a, w, t;
	char pp[]={0, 1, 2, 2, 3, 3, 4, 5, 6, 6, 7, 7, 8, 9, 10, 11};
	
	sprintf(lr_nomfch, "%s/controleur.lst", DSSFCH);
	ar->crt = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtmcu);
	sprintf(lr_nomfch, "%s/liaison.lst", DSSFCH);
	ar->lsn = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtlsn);
	ar->tty = gr_dev[strlen(gr_dev)-1]-'0';
	sprintf(lr_nomfch, "%s/port.lst", DSSFCH);
	ar->prt = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtprt);
	ar->cnn = gi_phs;
	ar->cnf = 3;
	
	sprintf(lr_nomfch, "%s/eeprom", DSSFCH);
	if ((lo = fopen(lr_nomfch, "r")) == NULL)
	{
		perror(lr_nomfch);
		return;
	}
	
	s = p = w = 0;
	while (fgets(lr_lgn, sizeof(lr_lgn), lo))
	{
		if (d = strchr(lr_lgn, '\n'))
			*d = 0;
		if (d = strchr(lr_lgn, '\r'))
			*d = 0;
		
		if (lr_lgn[0] == 0)
			continue;
		if (lr_lgn[0] == '*')
			continue;
			
		d = lr_lgn;
		if (!(f = strchr(d, '=')))
			continue;
		
		strncpy(vlr, d, f-d);
		vlr[f-d] = 0;
		if (sscanf(vlr, "%d", &a) != 1)
		{
			printf("Adresse invalide ... (%s / %s)\n", vlr, d);
			continue;
		}
		if ((a < 0) || (a > 1000))
		{
			printf("Adresse hors plage ... (%d)\n", a);
			continue;
		}
		
		d = f + 1;
		strncpy(vlr, d, 7);
		vlr[7] = 0;
		
		if (vlr[0] == '\'')
		{
			if (vlr[2] == '\'')
			{
				c = vlr[1];
				if (a <= 5)
				{
					if (cnt[a] == c)
						w |= 1 << a;
				}
				if (a < 32)
					continue;
				ar->srt[s][p] = c;
				ar->maj[s] |= 1 << p;
			}
		}
		else
		{
			if (sscanf(d, "%d", &v) == 1)
			{
				s = (a - 32) / 16;
				r = (a - 32) % 16;
				p = pp[r];
				if ((r == 2) || (r == 4) || (r == 8) || (r == 10))
					ar->srt[s][p] = v << 8;
				else if ((r == 3) || (r == 5) || (r == 9) || (r == 11))
					ar->srt[s][p] |= v;
				else
					ar->srt[s][p] = v;
				ar->maj[s] |= 1 << p;
				//printf("a=%d s=%d r=%d p=%d srt[%s][%s]=$%02x (%d)\n", a, s, r, p, gr_srt[s], gr_prm[p], ar->srt[s][p], ar->srt[s][p]);
			}
			else
				printf("adr=%d valeur incorrecte (%s)\n", a, d);
		}
	}

	if (w != 0x3F)
	{
		printf("Fichier eeprom incompatible avec cette version de programme ...\n");
	}
	
	fclose(lo);
}

//
// CallBack TimeOut 1S
//
// Gestion voyant etat connection
//

gboolean cb_vnt(gpointer dnn)
{
	Wnd *wnd = &WND;
	int  i;																												// gestion de la memoire partagee
	gchar *txt;
	char  n[2];
	struct termios tty, old;
	
	if (gi_phs < 0)
		return(FALSE);
	
//if (ar->maj & 1) printf("cb_vnt() maj=%d ind=$%x\n", ar->maj, ar->ind);
//printf("srt[%s][%s]=$%x\n", gr_prm[MMN], gr_srt[PR1], ar->srt[MMN][PR1]);
	if (gc_clr == 'g')
	{
		switch (gi_phs)
		{
			case 0:																										// non connecte
				gc_clr = 'r';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");

				if (!access(gr_dev, F_OK))
				{
					gi_phs++;
					ar->cnn = gi_phs;
					ar->cnf = 3;
					//gtk_button_set_label((GtkButton *) wnd->bttcnn, gr_dev);
				}
				break;
			
			case 1:																										// access dev
				gc_clr = 'm';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pmagenta.png");

				if ((go_dev = open(gr_dev, O_RDWR | O_NONBLOCK)) == -1)													// O_ASYNC
					gi_phs = 0;
				else
					gi_phs++;
				ar->cnn = gi_phs;
				ar->cnf = 3;
				break;

			case 2:																										// open
				gc_clr = 'o';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/porange.png");
				//printf("gr_dev=<%s>\n", gr_dev);

				// Config port serie	RAW,BD,8,N,1
				tcgetattr(go_dev, &tty);
				tcgetattr(go_dev, &old);
				cfsetospeed(&tty, BD);
				cfsetispeed(&tty, BD);
				tty.c_cflag &= ~CSIZE;
				tty.c_cflag |= CS8;
				tty.c_cflag &= ~PARENB;
				tty.c_cflag &= ~CSTOPB;
				tty.c_lflag = 0;
				tty.c_cc[VMIN] = 1;
				tty.c_cc[VTIME] = 0;
				tcsetattr(go_dev, TCSANOW, &tty);
				tcflush(go_dev, TCIOFLUSH);
				
				gi_phs++;
				ar->cnn = gi_phs;
				ar->cnf = 3;
				break;

			case 3:																										// vitesse (9600,8,N,1)
				gc_clr = 'y';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pjaune.png");
				
				if (ioctl(go_dev, TIOCMGET, &i) == -1)
				{
					gi_phs = 1;
					close(go_dev);
					go_dev = -1;

					if (errno == 5)
						gi_phs = 0;
					ar->cnn = gi_phs;
					ar->cnf = 3;
				}
				else
				{
					if (strncmp(gr_dev, "/dev/ttyUSB", 11) != 0)														// rs232
					{
						if (i & TIOCM_DSR)																				// dsr (port distant actif)
						{
							gi_phs++;
							ar->cnn = gi_phs;
							ar->cnf = 3;
						}
						//else
						//	printf("La carte NANO n'est pas prete ...\n");
					}
					else																								//usb
					{
						gi_phs++;
						ar->cnn = gi_phs;
						ar->cnf = 3;
					}
				}
				break;

			case 4:																										// machine distante prete (dsr)
				gc_clr = 'c';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pcyan.png");

				if (ioctl(go_dev, TIOCMGET, &i) == -1)
				{
					gi_phs = 1;
					close(go_dev);
					go_dev = -1;

					if (errno == 5)
						gi_phs = 0;
					ar->cnn = gi_phs;
					ar->cnf = 3;
				}
				else
				{
					if (strncmp(gr_dev, "/dev/ttyUSB", 11) != 0)														// rs232
					{
						if (!(i & TIOCM_DSR))																			// !dsr (machine distante inactive)
						{
							gi_phs = 3;
							ar->cnn = gi_phs;
							ar->cnf = 3;
							printf("La carte NANO n'est plus prete ...\n");
						}
						else
						{
							// bidon car genere par RTS de l'ordi (sur la carte convertisseur rs232/ttl le RTS et le CTS sont relie)
							if (i & TIOCM_CTS)																			// machine distante prete a recevoir
							{
								gi_phs++;
								ar->cnn = gi_phs;
								ar->cnf = 3;
							}
							else
								printf("La carte NANO est occupee\n");
						}
					}
					else																								// usb
					{
						gi_phs++;
						ar->cnn = gi_phs;
						ar->cnf = 3;
					}
				}
				break;

			case 5:																										// dsr et cts (distant en marche et a l'ecoute)
				gc_clr = 'b';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pbleu.png");

				gi_phs++;
				g_timeout_add(1, cb_rcv, NULL);																			// active la reception (machine locale)

				ar->cnn = gi_phs;
				ar->cnf = 3;
				break;

			case 6:																										// en attente de reception (connectee)
				gc_clr = 'v';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");

				if (strncmp(gr_dev, "/dev/ttyUSB", 11) != 0)															// rs232
				{
					if (ioctl(go_dev, TIOCMGET, &i) == -1)
					{
						if (errno == 5)
						{
							gi_phs = 0;
							close(go_dev);
							go_dev = -1;
							
							ar->cnn = gi_phs;
							ar->cnf = 3;
						}
					}
					else
					{
						if (!(i & TIOCM_DSR))																			// !dsr (port distant inactif)
						{
							gi_phs = 1;
							close(go_dev);
							go_dev = -1;
							printf("La carte NANO s'est deconnectee ...\n");
							
							ar->cnn = gi_phs;
							ar->cnf = 3;
						}
					}
				}
				else																									// usb
				{
					if (ioctl(go_dev, TIOCMGET, &i) == -1)
					{
						if (errno == 5)
						{
							gi_phs = 0;
							close(go_dev);
							go_dev = -1;
							
							ar->cnn = gi_phs;
							ar->cnf = 3;
						}
					}
				}
				break;

			case 7:																										// perdue la connection attente arret reception
				gc_clr = 'w';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pblanc.png");
				break;

			case 8:																										// perdue la connection reception arretee
				gc_clr = 'w';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pnoir.png");
				
				gi_phs = 0;
				ar->cnn = gi_phs;
				ar->cnf = 3;
				break;
		}
	}
	else
	{
		gc_clr = 'g';
		// pour avoir le voyant vert fixe
		if (gi_phs != 6)
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
		else
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
	}
	gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
	
	return(TRUE);
}

// CallBack Reception 1mS

gboolean cb_rcv(gpointer dnn)
{
	Wnd *wnd = &WND;
	//GdkRGBA color;
	char n[4], vlr[32], txt[80], mss[80], vrg[4];
	int  i, j, l, f, ret, li_nmb;
	const char *pbc;
	const gchar *acc;																									// mode access du bouton
	const gchar *o_pbc;																									// nom du bouton
	char *p;

	if (gi_phs < 6)
		return(FALSE);
	
	if ((ar->env & 8) == 0)
	{
		if (ar->env & 7)
		{
			ar->env |= 8;
			strcpy(gr_snd, ar->snd);
			fi_sndmss();
		}
	}
		
	if (gi_rcp == 0)
	{
		while (read(go_dev, &gr_rcp[0], 1) == 1)
		{
			/*
			if (gr_rcp[0] != '\n')
				printf("%c", gr_rcp[0]);
			else
				printf("\r");
			*/
			if (gr_rcp[0] == gr_prt[0])																					// debut message
			{
				gi_rcp = 1;
				break;
			}
		}
		return(TRUE);
	}
	else
	{
		while (read(go_dev, &gr_rcp[gi_rcp], 1) == 1)
		{
			/*
			if (gr_rcp[gi_rcp] != '\n')
				printf("%c", gr_rcp[gi_rcp]);
			else
				printf("\r");
			*/
			if (gr_rcp[gi_rcp] == '\r')
				continue;
			if (gr_rcp[gi_rcp] == gr_prt[1])																			// fin message
			{
				gr_rcp[gi_rcp+1] = 0;
				if (strlen(gr_rcp)-6 != gr_rcp[1]-'a')
				{
					printf("\rErreur longueur reception %ld/%d %s\n", strlen(gr_rcp)-6, gr_rcp[1]-'a', gr_rcp);
					//for (i=0; gr_rcp[i]!=0; i++)
					//{
					//	printf("<%02x> ", gr_rcp[i]);
					//}
					//printf("\n");
					gi_rcp = 0;
					break;
				}
		
				strncpy(gr_rcv, &gr_rcp[5], strlen(gr_rcp)-6);															// gr_rcv = commande (sans protocole)
				gr_rcv[strlen(gr_rcp)-6] = 0;
				gi_rcp = 0;
				gi_rcv = 1;
				//printf("<-- %s   \n", gr_rcv);//***//
				break;
			}
			
			gi_rcp++;
			if (gi_rcp >= LNGRCV+6)
			{
				printf("\rBuffer plein   \n");
				gi_rcp = 0;
				break;
			}
		}
		if (gi_rcv == 0)
			return(TRUE);
	}
		
	i = j = f = 0;
	gi_rcv = 0;
	
		
/**/	//printf("<-- (%ld) %s\n", strlen(gr_rcv), gr_rcv);
/*
	lg_strcpy(lg.unt, "cllbck");
	lg_strcpy(lg.fnc, "cb_rcv");
	lg_strcpy(lg.mss, "Recu:");
	lg_strcpy(lg.err, gr_rcv);
	lg_ecr();
*/
	if (gr_rcv[0] == gr_prt[2])																							// reponse courte ACK
	{
		if (ar->env & 8)
		{
			//printf("<~~ ack srt[%s][%s]=%d\n", gr_srt[gr_snd[2]-'a'], gr_prm[gr_snd[3]-'a'], ((((gr_snd[4]-' ')<<6)+(gr_snd[5]-' '))&0x3FF));//srt[gr_snd[2]-'a'][gr_snd[3]-'a']);
			if (gr_snd[5] == 0)																							// send court
				ar->srt[gr_snd[2]-'a'][gr_snd[3]-'a'] = (gr_snd[4]=='~')?0:0x3FF;
			else
				ar->srt[gr_snd[2]-'a'][gr_snd[3]-'a'] = (((gr_snd[4]-' ')<<6)+(gr_snd[5]-' '))&0x3FF;
			ar->env = 0;
			ar->maj[gr_snd[2]-'a'] |= 1 << (gr_snd[3]-'a');
		}
		printf("<== ACK\n");
		//ar->env = 0;
		return(TRUE);
	}
	if (gr_rcv[0] == gr_prt[3])																							// reponse courte NACK
	{
		if (ar->env & 8)
		{
			//printf("<== nack srt[%s][%s]=%d\n", gr_srt[gr_snd[2]-'a'], gr_prm[gr_snd[3]-'a'], ((((gr_snd[4]-' ')<<6)+(gr_snd[5]-' '))&0x3FF));//srt[gr_snd[2]-'a'][gr_snd[3]-'a']);			
			ar->env = 0;
		}
		printf("<== NACK\n");
		//ar->env = 0;
		return(TRUE);
	}
	if (gr_rcv[0] != '}')																								// pas envoie de nano
	{
		printf("<== %ld %s Erreur reception 1er caractere inconnu\n", strlen(gr_rcv), gr_rcv);
		return(TRUE);
	}
	
	switch (gr_rcv[1])
	{
		case 'z':																										// mode silencieux (ventilo+eclairage)
			sprintf(txt, "Mode ventilo=");
			if (gr_rcv[2] == '~')
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->etbl[0], 0);												// maj etat ventilo
				strcat(txt, "Securise.");
			}
			else if (gr_rcv[2] == '`')
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->etbl[0], 1);												// maj etat ventilo
				strcat(txt, "Silencieux.");
			}
			else
				strcat(txt, "???");
			printf("<== %s\n", txt);
			ar->env = 0;
			break;
			
		case 'y':																										// nombre de capteur
			txt[0] = 0;
			sprintf(vlr, "%d / ", gr_rcv[2] - 'a');
			strcat(txt, vlr);
			sprintf(vlr, "%d / ", gr_rcv[3] - 'a');
			strcat(txt, vlr);
			sprintf(vlr, "%d / ", gr_rcv[4] - 'a');
			strcat(txt, vlr);
			sprintf(vlr, "%d", 	  gr_rcv[5] - 'a');
			strcat(txt, vlr);
			gtk_entry_set_text((GtkEntry *) wnd->etbl[1], txt);															// maj nombre capteur
			printf("<== %s\n", txt);
			ar->env = 0;
			break;
			
		case 'x':																										// valeur
			if ((gr_rcv[2] < 'a') || (gr_rcv[2] > 'n'))																	// parametre (srt[.][]) valeur courte sur 1c.
			{
				printf("3eme c. != a-n !!!\n");
				break;
			}
			if ((gr_rcv[3] < 'a') || (gr_rcv[3] > 'n'))																	// sortie (ar->srt[][.]) valeur courte sur 1c.
			{
				printf("4eme c. != a-n !!!\n");
				break;
			}
			i = strlen(gr_rcv);
			if ((i < 5) || (i > 6))
			{
				printf("erreur longueur message != 5-6 !!!\n");
				break;
			}
			if (i == 5)																									// valeur courte ou digitale (1c)
			{
				if ((gr_rcv[4] != '~') && (gr_rcv[4] != '`'))															// erreur ni ON ni OFF
				{
					printf("erreur valeur ni ON ni OFF !!!\n");
					break;
				}
				if (gr_rcv[4] == '~')
					fv_rcv_1c_off();
				else
					fv_rcv_1c_on();
			}
			else																										// valeur longue ou analogique (2c)
				fv_rcv_2c();
			break;
			
		case 'w':																										// adresse
			//sprintf(txt, "adr[%d]=", gr_rcv[2]-'a');
			txt[0] = mss[0] = 0;
			for (j=0; j<8; j++)
			{
				i = gr_rcv[j*2+3] - ' ';
				i <<= 6;
				i += gr_rcv[j*2+4] - ' ';
				i &= 0xFF;
				sprintf(vlr, "%02x.", i);
				strcat(txt, vlr);
				sprintf(vlr, "$%02x ", i);
				strcat(mss, vlr);
			}
			txt[strlen(txt)-1] = 0;
			gtk_entry_set_text((GtkEntry *) wnd->etbl[2], txt);															// maj adresse
			printf("<== adr[%d]=%s\n", gr_rcv[2]-'a', mss);
			ar->env = 0;
			break;
			
		case 'v':
			printf("<== v: %s\n", gr_rcv);
			ar->env = 0;
			break;
			
		case 'u':
			printf("<== u: %s\n", gr_rcv);
			ar->env = 0;
			break;
			
		default:
			printf("<== %ld %s Erreur 2eme caractere inconnu\n", strlen(gr_rcv), gr_rcv);
			ar->env = 0;
			break;
	}

	return(TRUE);
}

// reception table srt write court off
void fv_rcv_1c_off()
{
	if (gr_rcv[3] == 'm')																					// MSR
	{
		fv_rcv_1c_msr_off();
	}
	else if (gr_rcv[3] == 'n')																				// CMM
	{
		fv_rcv_1c_cmm_off();
	}
	else																									// contenu table srt
	{
		ar->srt[gr_rcv[2]-'a'][gr_rcv[3]-'a'] = 0;
		ar->maj[gr_snd[2]-'a'] |= 1 << (gr_snd[3]-'a');
		printf("<== srt[%s][%s]=OFF\n", gr_srt[gr_rcv[2]-'a'], gr_prm[gr_rcv[3]-'a']);
	}
}

// reception table srt write court on
void fv_rcv_1c_on()
{
	if (gr_rcv[3] == 'm')																					// MSR
	{
		fv_rcv_1c_msr_on();
	}
	else if (gr_rcv[3] == 'n')																				// CMM
	{
		fv_rcv_1c_cmm_on();
	}
	else																									// contenu table srt
	{
		ar->srt[gr_rcv[2]-'a'][gr_rcv[3]-'a'] = 0x3FF;
		ar->maj[gr_snd[2]-'a'] |= 1 << (gr_snd[3]-'a');
		printf("<== srt[%s][%s]=ON\n", gr_srt[gr_rcv[2]-'a'], gr_prm[gr_rcv[3]-'a']);
	}
}

// reception d'une mesure off (entree=valeur d'un capteur numerique)
// gr_rcv[3] numero de la sortie
void fv_rcv_1c_msr_off()
{
	Wnd *wnd = &WND;
	int p, b;
	char n[4];

	if (ar->srt[gr_rcv[2]-'a'][ITP] != ITP_DGT)																			// ce n'est pas une entree dgt --> erreur
	{
		printf("srt[%s][MSR]=OFF Affectation mesure a OFF sur entree non numerique !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}
	if ((ar->srt[gr_rcv[2]-'a'][IBR] < 0) || (ar->srt[gr_rcv[2]-'a'][IBR] > 19)) 									// le capteur n'est pas compris entre D0 et D19
	{
		printf("srt[%s][MSR]=OFF Affectation mesure a OFF sur capteur different D0-D19 !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}

	if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 7)) 									// le capteur est compris entre D0 et D7
	{
		p = 3;
		b = ar->srt[gr_rcv[2]-'a'][IBR];
	}
	else if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 8) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 13)) 								// le capteur est compris entre D8 et D13
	{
		p = 1;
		b = ar->srt[gr_rcv[2]-'a'][IBR] - 8;
	}
	else if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 14) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 19)) 								// le capteur est compris entre D14 et D19
	{
		p = 2;
		b = ar->srt[gr_rcv[2]-'a'][IBR] - 14;
	}

	sprintf(n, "%c%cr", p+'a', b+'a');																					// name (n) port bit couleur
	gtk_widget_set_name(wnd->bdgt[p][b], n);
	wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");													// off = rouge
	gtk_button_set_image((GtkButton *) wnd->bdgt[p][b], wnd->pled);

	ar->srt[gr_rcv[2]-'a'][MSR] = 0;
	//***//ar->maj = 2;																										// uniquement ardsrv de concerne par la maj
	//***//ar->ind = (MSR << 4) + (gr_rcv[2]-'a');
}

// reception d'une mesure on (entree=valeur d'un capteur numerique)
void fv_rcv_1c_msr_on()
{
	Wnd *wnd = &WND;
	int p, b;
	char n[4];

	if (ar->srt[gr_rcv[2]-'a'][ITP] != ITP_DGT)																			// ce n'est pas une entree dgt --> erreur
	{
		printf("srt[%s][MSR]=ON Affectation mesure a ON sur entree non numerique !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}
	if ((ar->srt[gr_rcv[2]-'a'][IBR] < 0) || (ar->srt[gr_rcv[2]-'a'][IBR] > 19)) 									// le capteur n'est pas compris entre D0 et D19
	{
		printf("srt[%s][MSR]=ON Affectation mesure a ON sur capteur different D0-D19 !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}

	if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 7)) 									// le capteur est compris entre D0 et D7
	{
		p = 3;
		b = ar->srt[gr_rcv[2]-'a'][IBR];
	}
	else if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 8) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 13)) 								// le capteur est compris entre D8 et D13
	{
		p = 1;
		b = ar->srt[gr_rcv[2]-'a'][IBR] - 8;
	}
	else if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 14) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 19)) 								// le capteur est compris entre D14 et D19
	{
		p = 2;
		b = ar->srt[gr_rcv[2]-'a'][IBR] - 14;
	}

	sprintf(n, "%c%cv", p+'a', b+'a');																					// name (n) port bit couleur
	gtk_widget_set_name(wnd->bdgt[p][b], n);
	wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");													// on = vert
	gtk_button_set_image((GtkButton *) wnd->bdgt[p][b], wnd->pled);

	ar->srt[gr_rcv[2]-'a'][MSR] = 0x3FF;
	//***//ar->maj = 2;																										// uniquement ardsrv de concerne par la maj
	//***//ar->ind = (MSR << 4) + (gr_rcv[2]-'a');
}

// reception d'une commande off (ordre=valeur d'une sortie)
void fv_rcv_1c_cmm_off()
{
	Wnd *wnd = &WND;
	int p, b;
	char n[4];

	if (ar->srt[gr_rcv[2]-'a'][OTP] != OTP_DGT)																			// ce n'est pas une sortie dgt --> erreur
	{
		printf("srt[%s][CMM]=OFF Affectation commande a OFF sur sortie non numerique !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}
	if ((ar->srt[gr_rcv[2]-'a'][OBR] < 0) || (ar->srt[gr_rcv[2]-'a'][OBR] > 19)) 										// la broche de sortie n'est pas compris entre D0 et D19
	{
		printf("srt[%s][CMM]=OFF Affectation commande a OFF sur sortie differente D0-D19 !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}

	if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 7)) 										// le capteur est compris entre D0 et D7
	{
		p = 3;
		b = ar->srt[gr_rcv[2]-'a'][OBR];
	}
	else if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 8) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 13)) 								// le capteur est compris entre D8 et D13
	{
		p = 1;
		b = ar->srt[gr_rcv[2]-'a'][OBR] - 8;
	}
	else if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 14) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 19)) 								// le capteur est compris entre D14 et D19
	{
		p = 2;
		b = ar->srt[gr_rcv[2]-'a'][OBR] - 14;
	}

	sprintf(n, "%c%cr", p+'a', b+'a');																					// name (n) port bit couleur
	gtk_widget_set_name(wnd->bdgt[p][b], n);
	wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");													// off = rouge
	gtk_button_set_image((GtkButton *) wnd->bdgt[p][b], wnd->pled);

	ar->srt[gr_rcv[2]-'a'][CMM] = 0;
	//***//ar->maj = 2;																										// uniquement ardsrv de concerne par la maj
	//***//ar->ind = (CMM << 4) + (gr_rcv[2]-'a');
}

// reception d'une commande on (ordre=valeur d'une sortie)
void fv_rcv_1c_cmm_on()
{
	Wnd *wnd = &WND;
	int p, b;
	char n[4];

	if (ar->srt[gr_rcv[2]-'a'][OTP] != OTP_DGT)																			// ce n'est pas une sortie dgt --> erreur
	{
		printf("srt[%s][CMM]=ON Affectation commande a ON sur sortie non numerique !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}
	if ((ar->srt[gr_rcv[2]-'a'][OBR] < 0) || (ar->srt[gr_rcv[2]-'a'][OBR] > 19)) 										// la broche de sortie n'est pas compris entre D0 et D19
	{
		printf("srt[%s][CMM]=ON Affectation commande a ON sur sortie differente D0-D19 !!!\n", gr_srt[gr_rcv[2]-'a']);
		return;
	}

	if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 7)) 										// le capteur est compris entre D0 et D7
	{
		p = 3;
		b = ar->srt[gr_rcv[2]-'a'][OBR];
	}
	else if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 8) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 13)) 								// le capteur est compris entre D8 et D13
	{
		p = 1;
		b = ar->srt[gr_rcv[2]-'a'][OBR] - 8;
	}
	else if ((ar->srt[gr_rcv[2]-'a'][OBR] >= 14) && (ar->srt[gr_rcv[2]-'a'][OBR] <= 19)) 								// le capteur est compris entre D14 et D19
	{
		p = 2;
		b = ar->srt[gr_rcv[2]-'a'][OBR] - 14;
	}

	sprintf(n, "%c%cv", p+'a', b+'a');																					// name (n) port bit couleur
	gtk_widget_set_name(wnd->bdgt[p][b], n);
	wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");													// on = vert
	gtk_button_set_image((GtkButton *) wnd->bdgt[p][b], wnd->pled);

	ar->srt[gr_rcv[2]-'a'][CMM] = 0;
	//***//ar->maj = 2;																										// uniquement ardsrv de concerne par la maj
	//***//ar->ind = (CMM << 4) + (gr_rcv[2]-'a');
}

// reception table srt write long
void fv_rcv_2c()
{
	Wnd *wnd = &WND;
	//GdkRGBA color;
	char n[4], vlr[32], txt[80], mss[80], vrg[4];
	int  i, j, l, f, ret, li_nmb;
	const char *pbc;
	const gchar *acc;																									// mode access du bouton
	const gchar *o_pbc;																									// nom du bouton
	char *p;

	if ((gr_rcv[4] < ' ') || (gr_rcv[4] > '_'))
	{
		printf("5eme c. != ' '-'_' !!!\n");
		return;
	}
	if ((gr_rcv[5] < ' ') || (gr_rcv[5] > '_'))
	{
		printf("6eme c. != ' '-'_' !!!\n");
		return;
	}

	i = gr_rcv[4] - ' ';
	i <<= 6;
	i += gr_rcv[5] - ' ';
	i &= 0x3FF;
	ar->srt[gr_rcv[2]-'a'][gr_rcv[3]-'a'] = i;
	//ar->maj = 3;
	//ar->ind = ((gr_rcv[2]-'a') << 4) + (gr_rcv[3]-'a');

	if ((gr_rcv[3] - 'a') == MSR)																						// mesure
	{
		if (ar->srt[gr_rcv[2]-'a'][ITP] == ITP_OWR)
		{
			if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 9))								// temperature
			{
	//printf("<~~~ t=%03x %x\n", i, i >> 2);
				if ((i & 0x200) == 0)																					// positif
				{
					switch (i & 3)
					{
						case 1:
							strcpy(vrg, ".25");
							break;
						case 2:
							strcpy(vrg, ".50");
							break;
						case 3:
							strcpy(vrg, ".75");
							break;
						default:
							strcpy(vrg, ".00");
							break;
					}
					i >>= 2;
					sprintf(vlr, "%d%s", i, vrg);
				}
				else																									// negatif
				{
					i = ~i;
					i++;
					switch (i & 3)
					{
						case 1:
							strcpy(vrg, ".25");
							break;
						case 2:
							strcpy(vrg, ".5");
							break;
						case 3:
							strcpy(vrg, ".75");
							break;
						default:
							strcpy(vrg, ".75");
							break;
					}
					i >>= 2;
					sprintf(vlr, "-%d%s", i, vrg);
				}
				gtk_entry_set_text((GtkEntry *) wnd->eowr[ar->srt[gr_rcv[2]-'a'][IBR]], vlr);
			}
		}
		if (ar->srt[gr_rcv[2]-'a'][ITP] == ITP_ANL)
		{
			if ((ar->srt[gr_rcv[2]-'a'][IBR] >= 0) && (ar->srt[gr_rcv[2]-'a'][IBR] <= 7))								// analogique
			{
				sprintf(vlr, "%d", i);
				if ((ar->srt[gr_rcv[2]-'a'][IBR] == 6) || (ar->srt[gr_rcv[2]-'a'][IBR] == 7))
					gtk_entry_set_text((GtkEntry *) wnd->eanl[0][ar->srt[gr_rcv[2]-'a'][IBR]], vlr);
				else
					gtk_entry_set_text((GtkEntry *) wnd->eanl[1][ar->srt[gr_rcv[2]-'a'][IBR]], vlr);					// marche uniquement avec nano pas avec mega
			}
		}
	}
	else if ((gr_rcv[3] - 'a') == CMM)																					// commande
	{
		for (j=0; j<6; j++)
		{
			if (gc_pwm[j] == ar->srt[gr_rcv[2]-'a'][OBR])
				break;
		}
		if (j != 6)
		{
			sprintf(vlr, "%d", i & 0xFF);
			gtk_entry_set_text((GtkEntry *) wnd->epwm[j], vlr);
		}
	}
	else
	{
		if ((gr_rcv[2] == gr_snd[2]) && (gr_rcv[3] == gr_snd[3]))
		{
			ar->env = 0;
			ar->maj[gr_snd[2]-'a'] |= 1 << (gr_snd[3]-'a');
			//printf("<== srt[%s][%s]=%d maj=%x ind=%x\n", gr_srt[gr_rcv[2]-'a'], gr_prm[gr_rcv[3]-'a'], i, ar->maj, ar->ind);
		}
		printf("<== srt[%s][%s]=%d\n", gr_srt[gr_rcv[2]-'a'], gr_prm[gr_rcv[3]-'a'], i);
	}
}

void on_cbtmcu_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_mcu;
	char lr_nomfch[32];

	if ((lp_mcu = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtmcu)) == NULL)							// nom mcu non trouve
		return;
		
	fv_majcnf("mcu", lp_mcu);																							// mise a jour fichier config
	fi_gstmcu(lp_mcu);																									// traitement nouveau mcu (affichage)
	g_free(lp_mcu);
	sprintf(lr_nomfch, "%s/controleur.lst", DSSFCH);
	ar->crt = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtmcu);
	ar->cnf = 3;
}

void on_cbtlsn_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_lsn;
	char lr_nomfch[32];

	if ((lp_lsn = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtlsn)) == NULL)							// liaison non trouve
		return;
		
	fv_majcnf("liaison", lp_lsn);																						// mise a jour fichier config
	fi_gstlsn(lp_lsn);																									// traitement nouvelle liaison (connection)
	g_free(lp_lsn);
	sprintf(lr_nomfch, "%s/liaison.lst", DSSFCH);
	ar->lsn = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtlsn);
	ar->cnf = 3;
}

void on_cbtdev_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_dev;

	if ((lp_dev = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtdev)) == NULL)							// dev non trouve
		return;
		
	fv_majcnf("dev", lp_dev);																							// mise a jour fichier config
	fi_gstdev(lp_dev);																									// traitement nouveau dev (connection)
	ar->tty = (*(lp_dev+strlen(lp_dev)-1))-'0';
	ar->cnf = 3;
	g_free(lp_dev);
}

void on_cbtprt_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_prt;
	char lr_nomfch[32];

	if ((lp_prt = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtprt)) == NULL)							// numero port non trouve
		return;
		
	fv_majcnf("prt", lp_prt);																							// mise a jour fichier config
	fi_gstprt(lp_prt);																									// traitement nouveau numero de port (affichage)
	g_free(lp_prt);
	sprintf(lr_nomfch, "%s/port.lst", DSSFCH);
	ar->prt = fi_indcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtprt);
	ar->cnf = 3;
}

void on_bdgt_clicked(GtkButton *obj, gpointer dnn)
{
	printf("bdgt[] clicked !!!\n");
	
/*
	if (gi_clc == 0)
		g_timeout_add(DBLCLC, cb_toc, obj);	//millisecondes
	gi_clc++;
	return;
*/
}

void on_bbrc_clicked(GtkButton *obj, gpointer dnn)
{
	printf("bbrc[] clicked !!!\n");
	
/*
	if (gi_clc == 0)
		g_timeout_add(DBLCLC, cb_toc, obj);	//millisecondes
	gi_clc++;
	return;
*/
}


void on_bttmaj_clicked(GtkButton *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	const gchar *name, *texte;
	int  i, j, k, ind;
	char *txt, vlr[8], prt[8], lr_nomfch[80];
	unsigned char c;
	
	name = gtk_widget_get_name((GtkWidget *) obj);
	//printf("bttmaj() dnn=%s name=<%s>\n", dnn, (char *) name);
	
	switch (*(name+1))
	{
		case 'z':
			strcpy(gr_snd, name);
			if (*name != '{')
			{
				texte = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->etbl[0]);
				if (!strcmp(texte, "Silencieux"))
					strcat(gr_snd, "`");
				else
					strcat(gr_snd, "~");
			}
			fi_sndmss();
			break;
			
		case 'y':
			strcpy(gr_snd, name);
			fi_sndmss();
			break;
			
		case 'x':
			j = *(name+3) - 'a';
			i = 1;
			while (gi_maj[j] & 0xFFF)
			{
				vlr[0] = 0;
				strcpy(gr_snd, name);
				if (gi_maj[j] & 8) strcat(vlr, "a");
				if (gi_maj[j] & 0x104) strcat(vlr, "b");
				if (gi_maj[j] & 0x200) strcat(vlr, "c");
				if (gi_maj[j] & 0x400) strcat(vlr, "d");
				if (gi_maj[j] & 0x800) strcat(vlr, "e");
				if (gi_maj[j] & 0x10) strcat(vlr, "f");
				if (gi_maj[j] & 0x20) strcat(vlr, "g");
				gr_snd[2] = vlr[0];
				//printf("bttmaj() dnn=%s name=%s j=%d ar->srt[%s][%d]=$%02x mmysrt[%d][%d]=%s\n", dnn, (char *) name, j, vlr, (*(name+3))-'a', ar->srt[vlr[0]-'a'][(*(name+3))-'a'], i, j, mmysrt[i][j].nvlr);
	
				if (*name != '{')
				{
					k = ar->srt[(*(name+3))-'a'][vlr[0]-'a'];
					k &= 0xFF; 
					prt[0] = ' ' + (k >> 6);
					prt[1] = ' ' + (k & 0x3F);
					prt[2] = 0;
					strcat(gr_snd, prt);
				}
				i++;
				
				fi_sndmss();
				//gi_maj[*(name+1)-'a'] = 0;
				if (gr_snd[2] == 'a') gi_maj[j] &= 0xFF7;
				if (gr_snd[2] == 'b') gi_maj[j] &= 0xEFB;
				if (gr_snd[2] == 'c') gi_maj[j] &= 0xDFF;
				if (gr_snd[2] == 'd') gi_maj[j] &= 0xBFF;
				if (gr_snd[2] == 'e') gi_maj[j] &= 0x7FF;
				if (gr_snd[2] == 'f') gi_maj[j] &= 0xFEF;
				if (gr_snd[2] == 'g') gi_maj[j] &= 0xFDF;
			}
			break;
			
		case 'w':
			texte = gtk_entry_get_text((GtkEntry *) wnd->etbl[3]);
			i = atoi(texte);
			if (i > 10)
				i = 10;
			texte = gtk_entry_get_text((GtkEntry *) wnd->etbl[4]);
			j = atoi(texte);
			strcpy(gr_snd, name);
			gr_snd[2] = gr_lng[i];
			gr_snd[3] = 0;

			//printf("bttmaj() name=%s adr[%d]\n", name, i*8+j);
			
			if (*name != '{')
			{
				texte = gtk_entry_get_text((GtkEntry *) wnd->etbl[2]);
				//printf("bttmaj() name=%s texte=%s\n", name, texte);
				for (i=0, txt=texte; i<8; i++, txt+=3)
				{
					sscanf(txt, "%02x", &j);
					vlr[0] = ' ' + (j >> 6);
					vlr[1] = ' ' + (j & 0x3F);
					vlr[2] = 0;
					strcat(gr_snd, vlr);
				}
			}
			
			fi_sndmss();
			break;
	}
	
	
	return;
}

unsigned char *fp_ct12(unsigned char pc)
{
	gc_12[1] = ' ' + (pc & 0x3F);
	gc_12[0] = ' ' + (pc >> 6);
	
	return(gc_12);
}
/*
unsigned char *fp_ct12(unsigned int pi)
{
	gc_12[1] = ' ' + (pi & 0x3F);
	gc_12[0] = ' ' + ((pi >> 6) & 0x3F);
	
	return(gc_12);
}
*/

