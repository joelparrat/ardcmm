
#include "intgrp.h"
#include "bblio.h"
#include "bbllg.h"
#include "bblht.h"

LS   *lstprt;						// contenu de controleur.brc mis en forme (port digital analogic timer)
LS   *lstbrc;						// contenu de controleur.brc (brochage)
int  gi_cnn;						// connecte
int  go_srl[2];						// open port serie (1) ou fifo (2)
char gr_file[2][32];				// open port serie (1) ou fifo (2)
char gr_snd[LNGSND+1];
char gr_rcp[LNGRCV+6];
char gr_rcv[LNGRCV+1];
char gr_prt[5] = {'\t', '\n', 6, 0x15, 0};
char gr_lng[21] = "abcdefghijklmnopqrst";
int  gi_cmd;						//
int  gi_anl;						// analogique affichage dynamique (nombre a afficher)
char gr_anl[90];					// adc actif + correspondance portbit/n°adc
char gr_dgt[90];					// dgt revennu actif suite suppression adc
char gc_pwm[6] = {3, 5, 6, 9, 10, 11};
char gc_dgt[11][3] = {"", "", "", "", "", "", "c0", "c1", "c2", "d4", "b0"};
char gr_ard[4][8][4] = 
	 {
		"", "", "", "", "", "", "", "", 
		"D8", "D9", "D10", "D11", "D12", "D13", "XT1", "XT2", 
		"A0", "A1", "A2", "A3", "SDA", "SCL", "RST", "",
		"RX", "TX", "D2", "D3", "D4", "D5", "D6", "D7"
	 };
char gr_tt[4][8][21] = 
	 {
		"LED.Bleue", "LED.Verte", "LED.Rouge", "Simulateur.Entree", "I2C.SDA", "I2C.SCL", "Pompe.Debit", "Photoresistance", 
		"D8", "Ventilo.Radiateur.2", "Peltier.Reservoir.1", "Peltier.Reservoir.2", "OneWire.Temperature", "Voyant.Carte", "Quartz.1", "Quartz.2", 
		"LED.Bleue", "LED.Verte", "LED.Rouge", "Simulateur.Entree", "I2C.SDA", "I2C.SCL", "Reset", "",
		"D0/RX", "D1/TX", "D2", "Luminosite.LED", "Pompe.Erreur", "Ventilo.Boitier", "Ventilo.Radiat.1", "D7"
	 };
int  gi_maj[NMBSRT]; 	// tableau maj
char gc_savett;			// sauvegarde valeur NO ETAT precedent
unsigned char srt[PRM+1][NMBSRT+1];
char gc_brcpwm[12] = {0, 0, 0, 0, 0, 1, 2, 0, 0, 3, 4, 5};
unsigned char gc_12[3];

int  fi_sndmss(void);
int  fi_appcbt(char *, GtkComboBoxText *, char *);
int  fi_indcbt(char *, GtkComboBoxText *);
int  fi_gstmcu(const char *);
int  fi_gstlsn(const char *);
int  fi_getcnf(char *, char **);
void fv_majcnf(char *, char *);
void fv_affdnm(void);
void fv_majprm(void);
gboolean cb_cnn(gpointer);
gboolean cb_rcv(gpointer);
unsigned char *fp_ct12(unsigned char);

// Constructeur variable

void fv_cns_vrb(void)
{
	Wnd *wnd = &WND;
	char lr_nomfch[80];
	char *p, *lp_nommcu, *lp_nomlsn;
	gchar *txt;
	const gchar *lbl;
	int y, i, j, n;
	
	lg.ntr = 5;
	lg.grv= LG_INF;
	lg_strcpy(lg.prj, "vslprm");
	lg.tll = 1000000;
	lg.nmb = 3;
	lg_cns();

	gi_cnn = 0;
	gi_cmd = -1;
	go_srl[0] = go_srl[1] = -1;
	gr_snd[0] = gr_rcv[0] = 0;
	gc_savett = 0;
	gc_12[2] = 0;
	
	lstprt = ls_cns();
	lstbrc = ls_cns();
	
	// garni les combobox fixe
	
	sprintf(lr_nomfch, "%s/controleur.lst", NOMFCH);
	fi_getcnf("mcu", &lp_nommcu);
	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtmcu, lp_nommcu);
	if (lp_nommcu)
		free(lp_nommcu);
	sprintf(lr_nomfch, "%s/liaison.lst", NOMFCH);
	fi_getcnf("liaison", &lp_nomlsn);
	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cbtlsn, lp_nomlsn);
	if (lp_nomlsn)
		free(lp_nomlsn);
	sprintf(lr_nomfch, "%s/index.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][INDEX], NULL);
	sprintf(lr_nomfch, "%s/sortie.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SORTIE], NULL);
	sprintf(lr_nomfch, "%s/etat.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][ETAT], NULL);
	//sprintf(lr_nomfch, "%s/broche.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][BROCHE], NULL);
	//sprintf(lr_nomfch, "%s/smn.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);
	//sprintf(lr_nomfch, "%s/smx.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
	sprintf(lr_nomfch, "%s/entree.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][ENTREE], NULL);
	sprintf(lr_nomfch, "%s/capteur.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][CAPTEUR], NULL);
	sprintf(lr_nomfch, "%s/type.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][TYPE], NULL);
	//sprintf(lr_nomfch, "%s/numero.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][NUMERO], NULL);
	//sprintf(lr_nomfch, "%s/mmn.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMINI], NULL);
	//sprintf(lr_nomfch, "%s/mmx.lst", NOMFCH);
	//for (y=0; y<NMBSRT; y++)
	//	fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMAXI], NULL);
	sprintf(lr_nomfch, "%s/fonction.lst", NOMFCH);
	for (y=0; y<NMBSRT; y++)
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][FONCTION], NULL);
	
	// met les valeurs des parametres par defaut en fonction de eeprom
	
	fv_majprm();

	for (y=0; y<NMBSRT; y++)
		gi_maj[y] = 0;
		
	if ((txt = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtlsn)) != NULL)
	{
		if (!strcmp(txt, "RS232"))
		{
			gtk_button_set_label((GtkButton *) wnd->bttcnn, "Connection... ");

			strcpy(gr_file[0], "/dev/ttyS0");
			gr_file[1][0] = 0;
		}
		
		if (!strcmp(txt, "USB"))
		{
			gtk_button_set_label((GtkButton *) wnd->bttcnn, "Connection... ");

			strcpy(gr_file[0], "/dev/ttyUSB0");
			gr_file[1][0] = 0;
		}
		
		if (!strcmp(txt, "AVRX86"))
		{
			gtk_button_set_label((GtkButton *) wnd->bttcnn, "Maitre... ");

			strcpy(gr_file[0], "/tmp/avrsnd");
			strcpy(gr_file[1], "/tmp/avrrcv");
		}
		g_free(txt);
	}

	g_timeout_add_seconds(1, cb_cnn, NULL);
}

// Destructeur variable

void fv_dst_vrb(void)
{
	ls_dst(lstprt);
	ls_dst(lstbrc);
}

//
// Envoi commande
//
// retour: -1 erreur_send 1 erreur_receive 0 ok_send_receive

int fi_sndmss(void)
{
	printf("--> \t%cJ!%s\n", gr_lng[strlen(gr_snd)], gr_snd);
	
	// ajout du protocol
	
	write(go_srl[0], &gr_prt[0], 1);																					// caractere de debut
	write(go_srl[0], &gr_lng[strlen(gr_snd)], 1);																		// longueur message (sans protocol)
	write(go_srl[0], "J!", 2);																							// groupe + adresse	
	
	// envoi le message
	
	if (go_srl[0] != -1)
		if (write(go_srl[0], gr_snd, strlen(gr_snd)) != strlen(gr_snd))													// envoi commande
			return(-1);
	
	// ajout du protocol
	
	write(go_srl[0], &gr_prt[1], 1);
	
	return(0);
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
	char *p, prt[2], mcu[16], txt[80];
	int  pos, a;
	
	ls_vid(lstprt);
	ls_vid(lstbrc);
	gi_anl = 0;
	prt[1] = 0;
	gr_anl[0] = gr_dgt[0] = 0;
	
	sprintf(txt, "%s/%s.brc", NOMFCH, pp_mcu);
	if ((lo = fopen(txt, "r")) == NULL)
		return(-1);
		
	while (fgets(txt, sizeof(txt), lo) != NULL)
	{
		if ((p = strchr(txt, '\n')) != NULL)
			*p = 0;
		
		if (strlen(txt) < 3)
			continue;
		
		if ((txt[3] == ' ') || (txt[3] == 0))
		{
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
		else																											// broche ADCn
		{
			prt[0] = 'c';
			if (strncmp(txt, "ADC", 3) == 0)
			{
				if (sscanf(&txt[3], "%d", &a) == 1)
				{
					if (ls_rtx(lstprt, prt, 0) != -1)
					{
						crn = lstprt->current;
						*((crn->mmr+(*(crn->mmr+16)))+(txt[3]-'0')) = (a<10)?'0'+a:'a'+a-10;
					}
				}
			}
		}
	}
	fclose(lo);

	ls_prn(lstprt);
	//ls_prn(lstbrc);
	
	fv_affdnm();																										// affichage dynamique
	return(0);
}

int fi_gstlsn(const char *pp_lsn)
{
	return(0);
}

// lecture element du fichier configuration (.../cnf/vslprm)
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

// Mise a jour fichier configuration (.../cnf/vslprm)
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
	
	if (wnd->grddgt)
		gtk_widget_destroy(wnd->grddgt);
	if (wnd->grdanl)
		gtk_widget_destroy(wnd->grdanl);
	if (wnd->grdowr)
		gtk_widget_destroy(wnd->grdowr);
	if (wnd->grdpwm)
		gtk_widget_destroy(wnd->grdpwm);
	if (wnd->grdbrc)
		gtk_widget_destroy(wnd->grdbrc);

	wnd->grddgt = gtk_grid_new();
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grddgt, TRUE);
	gtk_container_add((GtkContainer *) wnd->algdgt, wnd->grddgt);
	gtk_widget_show(wnd->grddgt);

	wnd->grdanl = gtk_grid_new();
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdanl, TRUE);
	gtk_container_add((GtkContainer *) wnd->alganl, wnd->grdanl);
	gtk_widget_show(wnd->grdanl);

	wnd->grdowr = gtk_grid_new();
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdowr, TRUE);
	gtk_container_add((GtkContainer *) wnd->algowr, wnd->grdowr);
	gtk_widget_show(wnd->grdowr);

	wnd->grdpwm = gtk_grid_new();
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdpwm, TRUE);
	gtk_container_add((GtkContainer *) wnd->algpwm, wnd->grdpwm);
	gtk_widget_show(wnd->grdpwm);

	wnd->grdbrc = gtk_grid_new();
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdbrc, TRUE);
	gtk_container_add((GtkContainer *) wnd->algbrc, wnd->grdbrc);
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
					((p == 'b') && (b == 5)) || ((p == 'b') && (b == 0)))
					wnd->bdgt[p-'a'][b] = gtk_button_new_with_label("O ");
				else
					wnd->bdgt[p-'a'][b] = gtk_button_new_with_label("Z ");
				n[1] = '0' + b;
				n[2] = 'g';
				gtk_widget_set_name(wnd->bdgt[p-'a'][b], n);															// name (n) port bit couleur
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
				gtk_button_set_image((GtkButton *) wnd->bdgt[p-'a'][b], wnd->pled);
				gtk_button_set_image_position((GtkButton *) wnd->bdgt[p-'a'][b], GTK_POS_RIGHT);
				gtk_button_set_always_show_image((GtkButton *) wnd->bdgt[p-'a'][b], TRUE);
				gtk_grid_attach((GtkGrid *) wnd->grddgt, wnd->bdgt[p-'a'][b], 9-b, yd+1, 1, 1);
				gtk_widget_show(wnd->bdgt[p-'a'][b]);
				g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);

				//sprintf(l, "P%c%c", toupper(p), '0'+b);
				strcpy(l, gr_ard[p-'a'][b]);
				wnd->bbrc[p-'a'][b] = gtk_button_new_with_label(l);
				gtk_widget_set_name(wnd->bbrc[p-'a'][b], &l[1]);
				gtk_grid_attach((GtkGrid *) wnd->grdbrc, wnd->bbrc[p-'a'][b], 8-b, yd+1, 1, 1);
				gtk_widget_show(wnd->bbrc[p-'a'][b]);
				gtk_widget_set_tooltip_text(wnd->bbrc[p-'a'][b], gr_tt[p-'a'][b]);
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
				gtk_widget_set_tooltip_text(wnd->eanl[p-'a'][b], gr_tt[0][b]);
			
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
		sprintf(d, "OneWire %d", i);
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
		sprintf(d, "PWM %d", gc_pwm[i]);
		gtk_widget_set_tooltip_text(wnd->epwm[i], d);
	
		//g_signal_connect((GtkWindow *) wnd->bdgt[p-'a'][b], "clicked", (GCallback) on_bdgt_clicked, NULL);
	}
	sprintf(l, "PWM");
	wnd->lpwmc = gtk_label_new(l);
	gtk_grid_attach((GtkGrid *) wnd->grdpwm, wnd->lpwmc, 0, 1, 1, 1);
	gtk_widget_show(wnd->lpwmc);
}

void fv_majprm(void)
{
	Wnd *wnd = &WND;
	FILE *lo;
	char *d, *f, lr_nomfch[32], lr_lgn[10];
	int  v, p, s, t;
	
	sprintf(lr_nomfch, "%s/eeprom", NOMFCH);
	
	if ((lo = fopen(lr_nomfch, "r")) == NULL)
	{
		perror(lr_nomfch);
		return;
	}
	
	//partie independante de l'eeprom
	for (s=0; s<NMBSRT; s++)
	{
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][INDEX], s+1);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SORTIE], s+1);
		if ((s >= 6) && (s <= 8))
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ENTREE], 2);
		else if (s > 8)
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ENTREE], s-2);
		else
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ENTREE], s+1);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][FONCTION], 1);
	}
	
	p = s = 0;
	while (fgets(lr_lgn, sizeof(lr_lgn), lo))
	{
		if (d = strchr(lr_lgn, '\n'))
			*d = 0;
		if (d = strchr(lr_lgn, '\r'))
			*d = 0;
		
		if (lr_lgn[0] == 0)
			continue;
			
		if (!(d = strchr(lr_lgn, '=')))
			continue;
		
		d++;
		sscanf(d, "%02x", &v);
		srt[p][s] = (unsigned char) v;
		//if (p==CPT) printf("srt[%d][%d]=%02x\n", p, s, v);
		
		s++;
		if (s >= NMBSRT)
		{
			s = 0;
			p++;
			if (p >= MSR)
			{
				//printf("Erreur debordement table srt[][]\n");
				break;
			}
		}
	}
	
	for (s=0; s<NMBSRT; s++)
	{
		if ((srt[TIO][s] & 1) == 0)
		{
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 3);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], srt[SPN][s]+1);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], (!srt[SMN][s])?1:2);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], (!srt[SMX][s])?1:2);
		}
		else
		{
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 4);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], gc_brcpwm[srt[SPN][s]]+1);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], srt[SMN][s]/10+1);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], srt[SMX][s]/10+1);
		}

		if ((srt[TIO][s] & 2) == 0)
		{
			if ((srt[TIO][s] & 4) == 0)
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 1);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][CAPTEUR], 2);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], srt[CPT][s]+1-'a');
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], (!srt[MMN][s])?1:2);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], (!srt[MMX][s])?1:2);
			}
			else
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 3);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][CAPTEUR], 1);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], srt[CPT][s]+1-'0');
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], srt[MMN][s]/5+1);
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], srt[MMX][s]/5+1);
			}
		}
		else
		{
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 2);
			if (s == 0)
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][CAPTEUR], 10);
			else
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][CAPTEUR], 5);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], srt[CPT][s]+1-'A');
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], srt[MMN][s]/10+1);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], srt[MMX][s]/10+1);
		}
	}

	fclose(lo);
}

// CallBack TimeOut 1S

gboolean cb_cnn(gpointer dnn)
{
	Wnd *wnd = &WND;
	GdkRGBA color;
	const gchar *name;
	gchar *txt;
	char  n[2];
	
	//gi_cnn = 0;
	name = gtk_widget_get_name(wnd->bttcnn);
	if (*name == 'g')
	{
		strcpy(n, "r");
		wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");
	}
	else
	{
		strcpy(n, "g");
		wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
	}
	gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
	gtk_widget_set_name(wnd->bttcnn, n);

	if ((txt = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtlsn)) != NULL)
	{
		if (!strcmp(txt, "RS232"))
		{
			if ((go_srl[0] = open(gr_file[0], O_RDWR | O_NONBLOCK)) != -1)												// O_NONBLOCK or O_NDELAY
			{
				gtk_button_set_label((GtkButton *) wnd->bttcnn, "/dev/ttyS0 ");
				gdk_rgba_parse(&color, "#000aaa000000");
				gtk_widget_override_color(wnd->bttcnn, GTK_STATE_FLAG_NORMAL, &color);
				gi_cnn = 1;
			}
		}
		
		if (!strcmp(txt, "USB"))
		{
			if ((go_srl[0] = open(gr_file[0], O_RDWR | O_NONBLOCK)) != -1)												// O_NONBLOCK or O_NDELAY
			{
				gtk_button_set_label((GtkButton *) wnd->bttcnn, "/dev/ttyUSB0 ");
				gdk_rgba_parse(&color, "#000aaa000000");
				gtk_widget_override_color(wnd->bttcnn, GTK_STATE_FLAG_NORMAL, &color);
				gi_cnn = 1;
			}
		}
		
		if (!strcmp(txt, "AVRX86"))
		{
			if (go_srl[0] == -1)
				go_srl[0] = open(gr_file[0], O_RDONLY | O_NONBLOCK);													// O_NONBLOCK or O_NDELAY
			if (go_srl[0] != -1)
			{
				if ((go_srl[1] = open(gr_file[1], O_WRONLY | O_NONBLOCK)) != -1)										// O_NONBLOCK or O_NDELAY
				{
					gtk_button_set_label((GtkButton *) wnd->bttcnn, "/tmp/avrsnd ");
					gdk_rgba_parse(&color, "#000aaa000000");
					gtk_widget_override_color(wnd->bttcnn, GTK_STATE_FLAG_NORMAL, &color);
					gi_cnn = 1;
				}
			}
				
		}

		g_free(txt);
	}

	if (gi_cnn)
	{
		gi_cmd = -1;
		strcpy(n, "v");
		wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
		gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
		gtk_widget_set_name(wnd->bttcnn, n);
		gdk_rgba_parse(&color, "#000000000000");
		gtk_widget_override_color(wnd->bttcnn, GTK_STATE_FLAG_NORMAL, &color);

		g_timeout_add(1, cb_rcv, NULL);
	}
	
	return(!gi_cnn);
}

// CallBack Reception 1mS

gboolean cb_rcv(gpointer dnn)
{
	Wnd *wnd = &WND;
	GdkRGBA color;
	char n[4], vlr[5], txt[80];
	int  i, j, l, f, c, ret, li_nmb;
	const char *pbc;
	const gchar *acc;																									// mode access du bouton
	const gchar *o_pbc;																									// nom du bouton
	char *p, lr_rcv[LNGRCV+6];
	
	i = j = f = c = 0;
	gr_rcv[0] = 0;
	
	while (1)
	{
		if (read(go_srl[0], &lr_rcv[0], 1) != 1)
		{
			c++;
			if (c > 10)
				break;
			usleep(1);
			
			/* computation going on... */
			while (gtk_events_pending())
				gtk_main_iteration();
			/* ...computation continued */
		}
		else
		{
			c = 0;
			printf("%c", lr_rcv[0]);
		}

		if (lr_rcv[0] == '\t')																							// debut message
			break;

		//printf("Ce n'est pas une tabulation %02x %c\n", lr_rcv[0], lr_rcv[0]);
	}
	if (lr_rcv[0] != '\t')
	{
		//printf("Reception non continue\n");
		return(TRUE);
	}
	
	i++;
	c = 0;
	while (1)
	{
		if (read(go_srl[0], &lr_rcv[i], 1) != 1)
		{
			c++;
			if (c > 1000)
				break;
			usleep(50);
			
			/* computation going on... */
			while (gtk_events_pending())
				gtk_main_iteration();
			/* ...computation continued */
			
			continue;
		}
		c = 0;
		printf("%c", lr_rcv[i]);

		if (lr_rcv[i] == '\n')																							// fin message
			break;
		
		i++;
		if (i >= LNGRCV)
			break;
	}
	if (lr_rcv[i] != '\n')
	{
		printf("Reception non continue ... i=%d c=%d rcv=", i, c);
		for (c=0; c<i; c++)
			printf("%c", lr_rcv[c]);
		printf("\n");
		return(TRUE);
	}
		
	lr_rcv[i+1] = 0;
	if (strlen(lr_rcv)-5 != lr_rcv[1]-'a')
	{
		printf("Erreur longueur reception %d/%d\n", strlen(lr_rcv)-5, lr_rcv[1]-'a');
		return(TRUE);
	}
		
	strncpy(gr_rcv, &lr_rcv[4], strlen(lr_rcv)-5);
	gr_rcv[strlen(lr_rcv)-5] = 0;
		
	printf("<-- %d %s\n", strlen(gr_rcv), gr_rcv);
	lg_strcpy(lg.unt, "cllbck");
	lg_strcpy(lg.fnc, "cb_rcv");
	lg_strcpy(lg.mss, "Recu:");
	lg_strcpy(lg.err, gr_rcv);
	lg_ecr();

	if (gr_rcv[0] == 0x6)																								// reponse courte ACK
	{
		printf("<-- ACK\n");
		return(TRUE);
	}
	if (gr_rcv[0] == 0x15)																								// reponse courte NACK
	{
		printf("<-- NACK\n");
		return(TRUE);
	}
	if (gr_rcv[0] != '}')																								// reponse a une lecture
	{
		printf("<-- %d %s Erreur reception 1er caractere inconnu\n", strlen(gr_rcv), gr_rcv);
		return(TRUE);
	}
	
	switch (gr_rcv[1])
	{
		case 'z':																										// mode silencieux (ventilo+eclairage)
			sprintf(txt, "Mode ventilo = ");
			if (gr_rcv[2] == '~')
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->etbl[0], 0);
				strcat(txt, "Securise.");
			}
			else
			{
				gtk_combo_box_set_active((GtkComboBox *) wnd->etbl[0], 1);
				strcat(txt, "Silencieux.");
			}
			printf("z: %s\n", txt);
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
			gtk_entry_set_text((GtkEntry *) wnd->etbl[1], txt);
			printf("y: %s\n", txt);
			break;
		case 'x':
			strcpy(txt, "srt[");
			if ((gr_rcv[2] >= 'a') && (gr_rcv[2] <= 't'))
			{
				i = gr_rcv[2] - 'a';
				switch (i)
				{
					case 0:																								// la sortie est sur la broche (0-99)
						strcat(txt, "SPN");
						break;
					case 1:																								// la sortie est de type numerique ou analogique (dgt=0/anl=1)
						strcat(txt, "TIO");
						break;
					case 2:											// la sortie est pilotee par un capteur de temperature (0-9), un capteur analogique (A-Z) ou un capteur numerique (a-z)
						strcat(txt, "CPT");
						break;
					case 3:																								// seuil de mesure mini
						strcat(txt, "MMN");
						break;
					case 4:																								// seuil de mesure maxi
						strcat(txt, "MMX");
						break;
					case 5:																								// seuil sortie mini
						strcat(txt, "SMN");
						break;
					case 6:																								// seuil sortie maxi
						strcat(txt, "SMX");
						break;
					case 7:																								// valeur mesure par le capteur
						strcat(txt, "MSR");
						break;
					case 8:																								// valeur envoye sur la sortie (commande)
						strcat(txt, "CMM");
						break;
					default:
						printf("Reponse incorrecte 3eme c. n'est pas entre 'a' et 'i' !!!\n");
						break;
				}
			}
			else
				printf("Reponse incorrecte 3eme c. n'est pas entre 'a' et 't' !!!\n");
				
			strcat(txt, "][");
			if ((gr_rcv[3] >= 'a') && (gr_rcv[3] <= 't'))
			{
				i = gr_rcv[3] - 'a';
				switch (i)
				{
					case 0:																								// sortie luminosite boitier
						strcat(txt, "LBT");
						break;
					case 1:																								// sortie ventilo boitier
						strcat(txt, "VBT");
						break;
					case 2:																								// sortie ventilo radiateur 1
						strcat(txt, "VR1");
						break;
					case 3:																								// sortie ventilo radiateur 2
						strcat(txt, "VR2");
						break;
					case 4:																								// sortie peltier reservoir 1
						strcat(txt, "PR1");
						break;
					case 5:																								// sortie peltier reservoir 2
						strcat(txt, "PR2");
						break;
					case 6:																								// Sortie LED Bleue
						strcat(txt, "LDB");
						break;
					case 7:																								// Sortie LED Verte
						strcat(txt, "LDV");
						break;
					case 8:																								// Sortie LED Rouge
						strcat(txt, "LDR");
						break;
					case 9:																								// Sortie Erreur pression pompe
						strcat(txt, "ERR");
						break;
					case 10:																							// Sortie PD8
						strcat(txt, "OD8");
						break;
					default:
						printf("Reponse incorrecte 4eme c. n'est pas entre 'a' et 'k' !!!\n");
						break;
				}
				strcat(txt, "]=");
			}
			else
				printf("Reponse incorrecte 4eme c. n'est pas entre 'a' et 't' !!!\n");
			if (strlen(gr_rcv) == 5)
			{
				if ((gr_rcv[4] != '`') && (gr_rcv[4] != '~'))
					printf("Reponse incorrecte 5eme c. n'est ni '`' ni '~' !!!\n");
				else
				{
					if (gr_rcv[4] == '~')
					{
						strcat(txt, "OFF");
						if ((gr_rcv[3] == 'g') || (gr_rcv[3] == 'h') || (gr_rcv[3] == 'i'))
						{
							n[2] = 'g';
							wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
						}
						else
						{
							n[2] = 'r';
							wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");
						}
					}
					else
					{
						strcat(txt, "ON");
						if ((gr_rcv[3] == 'g') || (gr_rcv[3] == 'h') || (gr_rcv[3] == 'i'))
						{
							if (gr_rcv[3] == 'g')
							{
								n[2] = 'b';
								wnd->pled = gtk_image_new_from_file("/var/local/icn/pbleu.png");
							}
							if (gr_rcv[3] == 'h')
							{
								n[2] = 'v';
								wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
							}
							if (gr_rcv[3] == 'i')
							{
								n[2] = 'r';
								wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");
							}
						}
						else
						{
							wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
						}
					}
					n[0] = 'c';
					n[1] = gr_rcv[3]-'g'+'0';
					n[3] = 0;
					gtk_widget_set_name(wnd->bdgt[2][gr_rcv[3]-'g'], n);												// name (n) port bit couleur
					gtk_button_set_image((GtkButton *) wnd->bdgt[2][gr_rcv[3]-'g'], wnd->pled);

					/* computation going on... */

					while (gtk_events_pending())
						gtk_main_iteration();

					/* ...computation continued */
				}
			}
			else
			{
				if ((gr_rcv[4] < ' ') || (gr_rcv[4] > '_'))
					printf("Reponse incorrecte 5eme c. n'est pas entre ' ' et '_' !!!\n");
				else
				{
					if ((gr_rcv[5] < ' ') || (gr_rcv[5] > '_'))
						printf("Reponse incorrecte 6eme c. n'est pas entre ' ' et '_' !!!\n");
					else
					{
						i = gr_rcv[4] - ' ';
						i <<= 6;
						i += gr_rcv[5] - ' ';
						i &= 0xFF;
						sprintf(vlr, "%d", i);
						strcat(txt, vlr);

						for (i=0; i<6; i++)
							if (gc_pwm[i] == srt[SPN][gr_rcv[3]-'a'])
								break;
						if (i != 6)
							gtk_entry_set_text((GtkEntry *) wnd->epwm[i], vlr);

						/* computation going on... */

						while (gtk_events_pending())
							gtk_main_iteration();

						/* ...computation continued */
					}
				}
			}
			printf("x: %s\n", txt);
			break;
		case 'w':
			sscanf(&gr_rcv[3], "%x", &j);
			sprintf(vlr, "%d", j);
			gtk_entry_set_text((GtkEntry *) wnd->epwm[gr_rcv[1]-'0'], vlr);
			printf("w: %s\n", gr_rcv);
			break;
		case 'v':
			n[3] = 0;
			sscanf(&gr_rcv[3], "%x", &i);
			sscanf(&gr_rcv[1], "%x", &j);
			n[0] = gc_dgt[j][0];
			n[1] = gc_dgt[j][1];
			if (i == 0)
			{
				n[2] = 'r';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");
			}
			else
			{
				n[2] = 'v';
				wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
			}
			gtk_widget_set_name(wnd->bdgt[n[0]-'a'][n[1]-'0'], n);														// name (n) port bit couleur
			gtk_button_set_image((GtkButton *) wnd->bdgt[n[0]-'a'][n[1]-'0'], wnd->pled);
			printf("v: %s\n", gr_rcv);
			break;
		case 'u':
			printf("u: %s\n", gr_rcv);
			break;
		default:
			printf("<-- %d %s Erreur 2eme caractere inconnu\n", strlen(gr_rcv), gr_rcv);
			break;
	}
	
	return(TRUE);
}

void on_cbtmcu_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_mcu;

	if ((lp_mcu = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtmcu)) == NULL)							// nom mcu non trouve
		return;
		
	fv_majcnf("mcu", lp_mcu);																							// mise a jour fichier config
	fi_gstmcu(lp_mcu);																									// traitement nouveau mcu (affichage)
	g_free(lp_mcu);
}

void on_cbtlsn_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	gchar *lp_lsn;

	if ((lp_lsn = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->cbtlsn)) == NULL)							// liaison non trouve
		return;
		
	fv_majcnf("liaison", lp_lsn);																						// mise a jour fichier config
	fi_gstlsn(lp_lsn);																									// traitement nouvelle liaison (connection)
	g_free(lp_lsn);
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

//
// Le contenu d'un comboboxtext a change
//

void on_cbtprm_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	GdkRGBA color;
	char vlr[5], lr_nomfch[80];
	int  j, k, x, ind;
	gchar *txt, *txt1;
	const gchar *name;
	
	name = gtk_widget_get_name((GtkWidget *) obj);
	k = *name - 'a';
	j = *(name+1) - 'a';
	gi_maj[j] |= 1 << k;

	if ((txt = gtk_combo_box_text_get_active_text((GtkComboBoxText *) obj)) == NULL)
		return;
	//printf("changed name=%s txt=%s j=%d k=%d\n", name, txt, j, k);

	if (k == ETAT)																										// etat
	{
		//printf("maj srt[%d][%d]\n", TIO, j);
		switch (*(txt+1))
		{
			case '|':																									// 0|1
			case 'N':																									// on
			case 'F':																									// off
				srt[TIO][j] &= ~1;
				break;
			case 'W':																									// pwm
				srt[TIO][j] |= 1;
				break;
		}

		if (!strcmp(txt, "PWM"))
		{
			gdk_rgba_parse(&color, "#666AAA000000");
			gtk_widget_override_color(wnd->cprm[j][SORTIE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][BROCHE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][SMINI], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][SMAXI], GTK_STATE_FLAG_NORMAL, &color);
			
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SORTIE], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][BROCHE]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D3/PWM");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D5/PWM");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D6/PWM");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D9/PWM");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D10/PWM");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][BROCHE], "D11/PWM");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][BROCHE], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMINI]);
			sprintf(lr_nomfch, "%s/smn.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][SMINI], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMINI], 6);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMAXI]);
			sprintf(lr_nomfch, "%s/smx.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][SMAXI], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMAXI], 21);
		}
		else 
		{
			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][BROCHE]);
			sprintf(lr_nomfch, "%s/broche.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][BROCHE], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][BROCHE], 5);

			if (!strcmp(txt, "0|1"))
			{
				gdk_rgba_parse(&color, "#000AAA666000");
				gtk_widget_override_color(wnd->cprm[j][SORTIE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][BROCHE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMINI], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMAXI], GTK_STATE_FLAG_NORMAL, &color);
			
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SORTIE], 7);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMINI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "NO");
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "OFF");
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "ON");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMINI], 1);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMAXI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "NO");
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "OFF");
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "ON");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMAXI], 2);
			}
			else if (!strcmp(txt, "OFF"))
			{
				gdk_rgba_parse(&color, "#000AAA666000");
				gtk_widget_override_color(wnd->cprm[j][SORTIE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][BROCHE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMINI], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMAXI], GTK_STATE_FLAG_NORMAL, &color);

				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SORTIE], 7);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMINI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "OFF");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMINI], 0);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMAXI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "OFF");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMAXI], 0);
			}
			else if (!strcmp(txt, "ON"))
			{
				gdk_rgba_parse(&color, "#000AAA666000");
				gtk_widget_override_color(wnd->cprm[j][SORTIE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][BROCHE], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMINI], GTK_STATE_FLAG_NORMAL, &color);
				gtk_widget_override_color(wnd->cprm[j][SMAXI], GTK_STATE_FLAG_NORMAL, &color);

				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SORTIE], 7);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMINI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "ON");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMINI], 0);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMAXI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "ON");
				//if (!gc_savett)
				//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMAXI], 0);
			}
			else if (!strcmp(txt, "NO"))
			{
				gdk_rgba_parse(&color, "#FFFAAA000000");

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMINI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMINI], "NO");
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMINI], 0);

				gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][SMAXI]);
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][SMAXI], "NO");
				gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][SMAXI], 0);
								
				for (x=1; x<MAJ; x++)
				{
					if (x == ETAT)
						continue;
					
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][x], 0);
					gtk_widget_override_color(wnd->cprm[j][x], GTK_STATE_FLAG_NORMAL, &color);
				}
			}
		}
		gtk_widget_override_color(wnd->cprm[j][ETAT], GTK_STATE_FLAG_NORMAL, &color);
	}

	if (k == BROCHE)																									// broche
	{
		//printf("maj srt[%d][%d]\n", SPN, j);
		switch (*txt)
		{
			case 'D':
				sscanf(&txt[1], "%d", &x);
				srt[SPN][j] = x;
				break;
			case 'A':
				sscanf(&txt[1], "%d", &x);
				x += 14;
				srt[SPN][j] = x;
				break;
		}
	}
	
	if (k == SMINI)																										// seuil sortie mini
	{
		if (*txt == 'O')
			if (*(txt+1) == 'F')
				srt[SMN][j] = 0;
			else
				srt[SMN][j] = 255;
		else
			srt[SMN][j] = (unsigned char) atoi(txt);
	}

	if (k == SMAXI)																										// seuil sortie maxi
	{
		if (*txt == 'O')
			if (*(txt+1) == 'F')
				srt[SMX][j] = 0;
			else
				srt[SMX][j] = 255;
		else
			srt[SMX][j] = (unsigned char) atoi(txt);
	}
	
	if (k == TYPE)																										// type
	{
		switch (*txt)
		{
			case 'D':																									// dgt
				srt[TIO][j] &= ~6;
				break;
			case 'A':																									// anl
				srt[TIO][j] |= 2;
				break;
			case 'O':																									// owr
				srt[TIO][j] |= 4;
				break;
		}

		if (!strcmp(txt, "ANL"))
		{
			gdk_rgba_parse(&color, "#666000AAA000");
			gtk_widget_override_color(wnd->cprm[j][ENTREE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][CAPTEUR], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][TYPE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][NUMERO], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMINI], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMAXI], GTK_STATE_FLAG_NORMAL, &color);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][ENTREE], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][NUMERO]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A0");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A1");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A2");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A3");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A4/SDA");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A5/SCL");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A6");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "A7");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][NUMERO], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMINI]);
			sprintf(lr_nomfch, "%s/mmn.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][MMINI], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMINI], 6);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMAXI]);
			sprintf(lr_nomfch, "%s/mmx.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][MMAXI], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMAXI], 21);
		}
		else if (!strcmp(txt, "OWR"))
		{
			gdk_rgba_parse(&color, "#666000AAA000");
			gtk_widget_override_color(wnd->cprm[j][ENTREE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][CAPTEUR], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][TYPE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][NUMERO], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMINI], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMAXI], GTK_STATE_FLAG_NORMAL, &color);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][ENTREE], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][NUMERO]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T0");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T1");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T2");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T3");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T4");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T5");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T6");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T7");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T8");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][NUMERO], "T9");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][NUMERO], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMINI]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "0");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "5");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "10");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "15");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "20");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "25");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "30");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "35");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "40");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "45");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "50");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "55");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "60");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "65");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "70");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "75");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "80");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "85");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "90");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "95");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "100");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "-5");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "-10");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "-15");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "-20");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMINI], 7);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMAXI]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "0");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "5");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "10");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "15");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "20");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "25");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "30");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "35");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "40");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "45");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "50");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "55");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "60");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "65");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "70");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "75");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "80");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "85");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "90");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "95");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "100");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "-5");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "-10");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "-15");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "-20");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMAXI], 11);
		}
		else if (!strcmp(txt, "DGT"))
		{
			gdk_rgba_parse(&color, "#000666AAA000");
			gtk_widget_override_color(wnd->cprm[j][ENTREE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][CAPTEUR], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][TYPE], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][NUMERO], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMINI], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MMAXI], GTK_STATE_FLAG_NORMAL, &color);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][ENTREE], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][NUMERO]);
			sprintf(lr_nomfch, "%s/numero.lst", NOMFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[j][NUMERO], NULL);
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][NUMERO], 8);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMINI]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "OFF");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMINI], "ON");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMINI], 1);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[j][MMAXI]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "NO");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "OFF");
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[j][MMAXI], "ON");
			//if (!gc_savett)
			//	gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[j][MMAXI], 2);
		}
	}
	
	if (k == NUMERO)																									// numero
	{
		switch (*txt)
		{
			case 'D':																									// Dn
				sscanf(&txt[1], "%d", &x);
				srt[CPT][j] = 'a' + x;
				break;
			case 'A':																									// An
				sscanf(&txt[1], "%d", &x);
				if ((srt[TIO][j] & 6) == 0)
				{
					x += 14;
					srt[CPT][j] = 'a' + x;
				}
				else
					srt[CPT][j] = 'A' + x;
				break;
			case 'T':																									// Tn
				sscanf(&txt[1], "%d", &x);
				srt[CPT][j] = '0' + x;
				break;
		}
	}

	if (k == MMINI)																										// seuil mesure mini
	{
		if (*txt == 'O')
			if (*(txt+1) == 'F')
				srt[MMN][j] = 0;
			else
				srt[MMN][j] = 255;
		else
			srt[MMN][j] = (unsigned char) atoi(txt);
	}

	if (k == MMAXI)																										// seuil mesure maxi
	{
		if (*txt == 'O')
			if (*(txt+1) == 'F')
				srt[MMX][j] = 0;
			else
				srt[MMX][j] = 255;
		else
			srt[MMX][j] = (unsigned char) atoi(txt);
	}

	if (k == FONCTION)																									// fonction
	{
		if (*txt == 'M')																								// maj
		{
			gdk_rgba_parse(&color, "#CCC000CCC000");
			gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[j][MAJ], "MAJ");
			sprintf(vlr, "}x%c%c", *(txt+1), 'a'+j);
			gtk_widget_set_name(wnd->cprm[j][MAJ], vlr);
		}
		else if (*txt == 'L')																							// lct
		{
			gdk_rgba_parse(&color, "#000AAA000000");
			gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[j][MAJ], "LCT");
			sprintf(vlr, "{x%c%c", *(txt+1), 'a'+j);
			gtk_widget_set_name(wnd->cprm[j][MAJ], vlr);
		}
		else if (*txt == 'R')																							// rom
		{
			gdk_rgba_parse(&color, "#CCC000000000");
			gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[j][MAJ], "ROM");
			sprintf(vlr, "|x%c%c", *(txt+1), 'a'+j);
			gtk_widget_set_name(wnd->cprm[j][MAJ], vlr);
		}
		else																											// no
		{
			gdk_rgba_parse(&color, "#FFFAAA000000");
			gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[j][MAJ], "NO");
			sprintf(vlr, "ux%c%c", *(txt+1), 'a'+j);
			gtk_widget_set_name(wnd->cprm[j][MAJ], vlr);
		}
	}

	//if (!strcmp(txt, "NO"))
	//	gc_savett = 0;
	//else
		gc_savett = 1;
}

void on_bttmaj_clicked(GtkButton *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	const gchar *name, *texte;
	int  i, j, ind;
	char *txt, vlr[8], lr_nomfch[80];
	
	name = gtk_widget_get_name((GtkWidget *) obj);
	//printf("bttmaj() name=%s srt[%d][%d]=%02x\n", name, (*(name+2))-'a', (*(name+3))-'a', srt[(*(name+2))-'a'][(*(name+3))-'a']);

	strcpy(gr_snd, name);
	
	switch(*(name+1))
	{
		case 'z':
			if (*name != '{')
			{
				texte = gtk_combo_box_text_get_active_text((GtkComboBoxText *) wnd->etbl[0]);
				if (!strcmp(texte, "Silencieux"))
					strcat(gr_snd, "`");
				else
					strcat(gr_snd, "~");
			}
			break;
		case 'x':
			if (*name != '{')
			{
				texte = fp_ct12(srt[*(name+2)-'a'][*(name+3)-'a']);
				strcat(gr_snd, texte);
			}
			break;
	}
	fi_sndmss();
	return;

	gi_maj[*(name+1)-'a'] = 0;
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

