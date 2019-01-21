
// nom=INTGRP
// version=0.0
// quoi=Interface graphique
// qui=GNRGRP

// Include

#include "intgrp.h"

// Define

// Declaration fonction

void aff_fenetre(void);
void create_wnd(void);
void show_wnd(void);
void hide_wnd(void);

void fv_cns_vrb(void);
void fv_dst_vrb(void);

// Declaration callback

void on_wnd_destroy(GtkWindow *, gpointer);
void on_cbtmcu_changed(GtkComboBox *, gpointer);
void on_cbtlsn_changed(GtkComboBox *, gpointer);
void on_cbtdev_changed(GtkComboBox *, gpointer);
void on_cbtprt_changed(GtkComboBox *, gpointer);
void on_bdgt_clicked(GtkButton *, gpointer);
void on_bbrc_clicked(GtkButton *, gpointer);
void on_cbtprm_changed(GtkComboBox *, gpointer);
void on_bttmaj_clicked(GtkButton *, gpointer);

// Variable globale

Wnd WND;

// Creation fenetre

void create_wnd(void)
{
	Wnd *wnd = &WND;
	char t[32];
	int x, y;
	
	wnd->wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow *) wnd->wnd, "Gestion communication avec NANO");
	gtk_window_set_icon_from_file((GtkWindow *) wnd->wnd, "/var/local/icn/ardcmm.png", NULL);
	gtk_window_set_default_size((GtkWindow *) wnd->wnd, 910, 440);
	//gtk_window_set_position((GtkWindow *) wnd->wnd, GTK_WIN_POS_CENTER_ALWAYS);

	wnd->box0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add((GtkContainer *) wnd->wnd, wnd->box0);

	wnd->box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start((GtkBox *) wnd->box0, wnd->box1, FALSE, TRUE, 5);

	wnd->frmio = gtk_frame_new(NULL);
	//gtk_widget_set_margin_start(wnd->frmio, 5);
	//gtk_widget_set_margin_end(wnd->frmio, 5);
	//gtk_widget_set_margin_top(wnd->frmio, 5);
	//gtk_widget_set_margin_bottom(wnd->frmio, 5);
	gtk_box_pack_start((GtkBox *) wnd->box1, wnd->frmio, FALSE, TRUE, 5);

	wnd->lblio = gtk_label_new(" E/S ");
	gtk_label_set_markup((GtkLabel *) wnd->lblio, "<span foreground=\"#222299\"><b> E/S </b></span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmio, wnd->lblio);

	wnd->box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add((GtkContainer *) wnd->frmio, wnd->box2);

	wnd->frmowr = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmowr, 5);
	gtk_widget_set_margin_end(wnd->frmowr, 5);
	//gtk_widget_set_margin_top(wnd->frmowr, 5);
	//gtk_widget_set_margin_bottom(wnd->frmowr, 5);
	gtk_box_pack_start((GtkBox *) wnd->box2, wnd->frmowr, FALSE, TRUE, 5);
	
	wnd->lblowr = gtk_label_new(" Temperatures ");
	gtk_label_set_markup((GtkLabel *) wnd->lblowr, "<span foreground=\"#222299\"> Temperatures </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmowr, wnd->lblowr);

	wnd->grdowr = NULL;

	wnd->frmanl = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmanl, 5);
	gtk_widget_set_margin_end(wnd->frmanl, 5);
	//gtk_widget_set_margin_top(wnd->frmanl, 5);
	//gtk_widget_set_margin_bottom(wnd->frmanl, 5);
	gtk_box_pack_start((GtkBox *) wnd->box2, wnd->frmanl, FALSE, TRUE, 5);
	
	wnd->lblanl = gtk_label_new(" Entrees Analogiques ");
	gtk_label_set_markup((GtkLabel *) wnd->lblanl, "<span foreground=\"#222299\"> Entrees Analogiques </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmanl, wnd->lblanl);

	wnd->grdanl = NULL;

	wnd->frmdgt = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmdgt, 5);
	gtk_widget_set_margin_end(wnd->frmdgt, 5);
	//gtk_widget_set_margin_top(wnd->frmdgt, 5);
	//gtk_widget_set_margin_bottom(wnd->frmdgt, 5);
	gtk_box_pack_start((GtkBox *) wnd->box2, wnd->frmdgt, FALSE, TRUE, 5);
	
	wnd->lbldgt = gtk_label_new(" Entrees/Sorties Numeriques ");
	gtk_label_set_markup((GtkLabel *) wnd->lbldgt, "<span foreground=\"#222299\"> Entrees/Sorties Numeriques </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmdgt, wnd->lbldgt);

	wnd->grddgt = NULL;

	wnd->frmpwm = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmpwm, 5);
	gtk_widget_set_margin_end(wnd->frmpwm, 5);
	//gtk_widget_set_margin_top(wnd->frmpwm, 5);
	//gtk_widget_set_margin_bottom(wnd->frmpwm, 5);
	gtk_box_pack_start((GtkBox *) wnd->box2, wnd->frmpwm, FALSE, TRUE, 5);

	wnd->lblpwm = gtk_label_new(" Sorties Analogiques (PWM) ");
	gtk_label_set_markup((GtkLabel *) wnd->lblpwm, "<span foreground=\"#222299\"> Sorties Analogiques (PWM) </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmpwm, wnd->lblpwm);

	wnd->grdpwm = NULL;

	wnd->frmsst = gtk_frame_new(NULL);
	//gtk_widget_set_margin_start(wnd->frmsst, 5);
	//gtk_widget_set_margin_end(wnd->frmsst, 5);
	//gtk_widget_set_margin_top(wnd->frmsst, 5);
	//gtk_widget_set_margin_bottom(wnd->frmsst, 5);
	gtk_box_pack_start((GtkBox *) wnd->box1, wnd->frmsst, FALSE, TRUE, 5);

	wnd->lblsst = gtk_label_new(" Systeme ");
	gtk_label_set_markup((GtkLabel *) wnd->lblsst, "<span foreground=\"#222299\"><b> Systeme </b></span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmsst, wnd->lblsst);

	wnd->box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add((GtkContainer *) wnd->frmsst, wnd->box3);

	wnd->frmcrt = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmcrt, 5);
	gtk_widget_set_margin_end(wnd->frmcrt, 5);
	//gtk_widget_set_margin_top(wnd->frmcrt, 5);
	//gtk_widget_set_margin_bottom(wnd->frmcrt, 5);
	gtk_box_pack_start((GtkBox *) wnd->box3, wnd->frmcrt, FALSE, TRUE, 5);

	wnd->lblcrt = gtk_label_new(" Carte ");
	gtk_label_set_markup((GtkLabel *) wnd->lblcrt, "<span foreground=\"#222299\"> Carte </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmcrt, wnd->lblcrt);

	wnd->box4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_margin_start(wnd->box4, 5);
	gtk_widget_set_margin_end(wnd->box4, 5);
	//gtk_widget_set_margin_top(wnd->box4, 5);
	gtk_widget_set_margin_bottom(wnd->box4, 5);
	gtk_container_add((GtkContainer *) wnd->frmcrt, wnd->box4);

	wnd->cbtmcu = gtk_combo_box_text_new();
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->cbtmcu, TRUE, TRUE, 5);

	wnd->cbtlsn = gtk_combo_box_text_new();
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->cbtlsn, TRUE, TRUE, 5);

	wnd->cbtdev = gtk_combo_box_text_new();
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->cbtdev, TRUE, TRUE, 5);

	wnd->cbtprt = gtk_combo_box_text_new();
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->cbtprt, TRUE, TRUE, 5);

	wnd->bttcnn = gtk_button_new();
	wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
	gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
	gtk_button_set_image_position((GtkButton *) wnd->bttcnn, GTK_POS_RIGHT);
	gtk_button_set_always_show_image((GtkButton *) wnd->bttcnn, TRUE);
	gtk_widget_set_name(wnd->bttcnn, "g");
	gtk_box_pack_end((GtkBox *) wnd->box4, wnd->bttcnn, TRUE, TRUE, 5);

	wnd->frmbrc = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmbrc, 5);
	gtk_widget_set_margin_end(wnd->frmbrc, 5);
	//gtk_widget_set_margin_top(wnd->frmbrc, 5);
	//gtk_widget_set_margin_bottom(wnd->frmbrc, 5);
	gtk_box_pack_start((GtkBox *) wnd->box3, wnd->frmbrc, FALSE, TRUE, 5);
	
	wnd->lblbrc = gtk_label_new(" Broches AVR --> Arduino");
	gtk_label_set_markup((GtkLabel *) wnd->lblbrc, "<span foreground=\"#222299\"> Broches AVR --> Arduino </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmbrc, wnd->lblbrc);

	wnd->grdbrc = NULL;

	wnd->frmtbl = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmtbl, 5);
	gtk_widget_set_margin_end(wnd->frmtbl, 5);
	//gtk_widget_set_margin_top(wnd->frmtbl, 5);
	//gtk_widget_set_margin_bottom(wnd->frmtbl, 5);
	gtk_box_pack_start((GtkBox *) wnd->box3, wnd->frmtbl, FALSE, TRUE, 5);
	
	wnd->lbltbl = gtk_label_new(" Tables ");
	gtk_label_set_markup((GtkLabel *) wnd->lbltbl, "<span foreground=\"#222299\"> Tables </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmtbl, wnd->lbltbl);

	wnd->grdtbl = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdtbl, 5);
	gtk_widget_set_margin_end(wnd->grdtbl, 5);
	//gtk_widget_set_margin_top(wnd->grdtbl, 5);
	gtk_widget_set_margin_bottom(wnd->grdtbl, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdtbl, FALSE);
	gtk_grid_set_column_spacing((GtkGrid *) wnd->grdtbl, 7);
	gtk_container_add((GtkContainer *) wnd->frmtbl, wnd->grdtbl);

	wnd->ltbl[0] = gtk_label_new("Mode ventilo");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->ltbl[0], 0, 0, 1, 1);

	wnd->etbl[0] = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->etbl[0], "Securise");
	gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->etbl[0], "Silencieux");
	gtk_combo_box_set_active((GtkComboBox *) wnd->etbl[0], 1);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[0], 1, 0, 2, 1);
	
	wnd->btbl[0] = gtk_button_new_with_label("LCT");
	sprintf(t, "{z");
	gtk_widget_set_name(wnd->btbl[0], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[0], 5, 0, 1, 1);
	
	wnd->btbl[1] = gtk_button_new_with_label("MAJ");
	sprintf(t, "}z");
	gtk_widget_set_name(wnd->btbl[1], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[1], 6, 0, 1, 1);
	
	wnd->btbl[2] = gtk_button_new_with_label("ROM");
	sprintf(t, "|z");
	gtk_widget_set_name(wnd->btbl[2], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[2], 7, 0, 1, 1);

	wnd->ltbl[1] = gtk_label_new("Nombre Capteur");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->ltbl[1], 0, 1, 1, 1);
	
	wnd->etbl[1] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[1], 24);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[1], 24);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[1], "00 / 00 / 00 / 00");
	gtk_widget_set_tooltip_text(wnd->etbl[1], "Numeric Analogic Onewire_Reel Onewire_Table");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[1], 1, 1, 2, 1);
	
	wnd->btbl[3] = gtk_button_new_with_label("LCT");
	sprintf(t, "{y");
	gtk_widget_set_name(wnd->btbl[3], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[3], 5, 1, 1, 1);

	wnd->ltbl[2] = gtk_label_new("AdresseCapteur");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->ltbl[2], 0, 2, 1, 1);
	
	wnd->etbl[2] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[2], 24);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[2], 24);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[2], "00.00.00.00.00.00.00.00");
	gtk_widget_set_tooltip_text(wnd->etbl[2], "Adresse du capteur");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[2], 1, 2, 2, 1);
	
	wnd->etbl[3] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[3], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[3], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[3], "00");
	gtk_widget_set_tooltip_text(wnd->etbl[3], "Numero du capteur (0-10)");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[3], 3, 2, 1, 1);
	
	wnd->etbl[4] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[4], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[4], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[4], "");
	gtk_widget_set_tooltip_text(wnd->etbl[4], "Rien");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[4], 4, 2, 1, 1);
	
	wnd->btbl[4] = gtk_button_new_with_label("LCT");
	sprintf(t, "{wa");
	gtk_widget_set_name(wnd->btbl[4], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[4], 5, 2, 1, 1);
	
	wnd->btbl[5] = gtk_button_new_with_label("MAJ");
	sprintf(t, "}wa");
	gtk_widget_set_name(wnd->btbl[5], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[5], 6, 2, 1, 1);
	
	wnd->btbl[6] = gtk_button_new_with_label("ROM");
	sprintf(t, "|wa");
	gtk_widget_set_name(wnd->btbl[6], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[6], 7, 2, 1, 1);

	wnd->ltbl[3] = gtk_label_new("Mesure");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->ltbl[3], 0, 3, 1, 1);
	
	wnd->etbl[5] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[5], 24);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[5], 24);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[5], "000");
	gtk_widget_set_tooltip_text(wnd->etbl[5], "Mesure");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[5], 1, 3, 2, 1);
	
	wnd->etbl[6] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[6], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[6], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[6], "00");
	gtk_widget_set_tooltip_text(wnd->etbl[6], "Numero du capteur");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[6], 3, 3, 1, 1);
	
	wnd->etbl[7] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[7], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[7], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[7], "");
	gtk_widget_set_tooltip_text(wnd->etbl[7], "Rien");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[7], 4, 3, 1, 1);
	
	wnd->btbl[7] = gtk_button_new_with_label("LCT");
	sprintf(t, "{xha");
	gtk_widget_set_name(wnd->btbl[7], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[7], 5, 3, 1, 1);
	
	wnd->btbl[8] = gtk_button_new_with_label("MAJ");
	sprintf(t, "}xha");
	gtk_widget_set_name(wnd->btbl[8], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[8], 6, 3, 1, 1);

	wnd->ltbl[4] = gtk_label_new("Commande");
	//gtk_label_set_markup(wnd->ltbl[4], "<span foreground=\"#FF0000\">Commande</span>");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->ltbl[4], 0, 4, 1, 1);
	
	wnd->etbl[8] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[8], 24);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[8], 24);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[8], "000");
	gtk_widget_set_tooltip_text(wnd->etbl[8], "Commande");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[8], 1, 4, 2, 1);
	
	wnd->etbl[9] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[9], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[9], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[9], "00");
	gtk_widget_set_tooltip_text(wnd->etbl[9], "Numero du capteur");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[9], 3, 4, 1, 1);
	
	wnd->etbl[10] = gtk_entry_new();
	gtk_entry_set_width_chars((GtkEntry *) wnd->etbl[10], 3);
	gtk_entry_set_max_length((GtkEntry *) wnd->etbl[10], 3);
	gtk_entry_set_text((GtkEntry *) wnd->etbl[10], "");
	gtk_widget_set_tooltip_text(wnd->etbl[10], "Rien");
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->etbl[10], 4, 4, 1, 1);
	
	wnd->btbl[9] = gtk_button_new_with_label("LCT");
	sprintf(t, "{xia");
	gtk_widget_set_name(wnd->btbl[9], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[9], 5, 4, 1, 1);
	
	wnd->btbl[10] = gtk_button_new_with_label("MAJ");
	sprintf(t, "}xia");
	gtk_widget_set_name(wnd->btbl[10], t);
	gtk_grid_attach((GtkGrid *) wnd->grdtbl, wnd->btbl[10], 6, 4, 1, 1);
	g_signal_connect((GtkWindow *) wnd->wnd, "destroy", (GCallback) on_wnd_destroy, NULL);
	g_signal_connect((GtkComboBox *) wnd->cbtmcu, "changed", (GCallback) on_cbtmcu_changed, "MCU");
	g_signal_connect((GtkComboBox *) wnd->cbtlsn, "changed", (GCallback) on_cbtlsn_changed, "Liaison");
	g_signal_connect((GtkComboBox *) wnd->cbtdev, "changed", (GCallback) on_cbtdev_changed, "Dev");
	g_signal_connect((GtkComboBox *) wnd->cbtprt, "changed", (GCallback) on_cbtprt_changed, "Port");
	g_signal_connect((GtkWindow *) wnd->btbl[0],  "clicked", (GCallback) on_bttmaj_clicked, "LCT");
	g_signal_connect((GtkWindow *) wnd->btbl[1],  "clicked", (GCallback) on_bttmaj_clicked, "MAJ");
	g_signal_connect((GtkWindow *) wnd->btbl[2],  "clicked", (GCallback) on_bttmaj_clicked, "ROM");
	g_signal_connect((GtkWindow *) wnd->btbl[3],  "clicked", (GCallback) on_bttmaj_clicked, "LCT");
	g_signal_connect((GtkWindow *) wnd->btbl[4],  "clicked", (GCallback) on_bttmaj_clicked, "LCT");
	g_signal_connect((GtkWindow *) wnd->btbl[5],  "clicked", (GCallback) on_bttmaj_clicked, "MAJ");
	g_signal_connect((GtkWindow *) wnd->btbl[6],  "clicked", (GCallback) on_bttmaj_clicked, "ROM");
	g_signal_connect((GtkWindow *) wnd->btbl[7],  "clicked", (GCallback) on_bttmaj_clicked, "LCT");
	g_signal_connect((GtkWindow *) wnd->btbl[8],  "clicked", (GCallback) on_bttmaj_clicked, "MAJ");
	g_signal_connect((GtkWindow *) wnd->btbl[9],  "clicked", (GCallback) on_bttmaj_clicked, "LCT");
	g_signal_connect((GtkWindow *) wnd->btbl[10], "clicked", (GCallback) on_bttmaj_clicked, "MAJ");
}

// Affichage Fenetre

void show_wnd(void)
{
	Wnd *wnd = &WND;

	gtk_widget_show_all(wnd->wnd);
}

// Cache fenetre

void hide_wnd(void)
{
	Wnd *wnd = &WND;

	gtk_widget_hide(wnd->wnd);
}

// Quitte le programme

void on_wnd_destroy(GtkWindow *obj, gpointer dnn)
{
	fv_dst_vrb();
	//gtk_main_quit();
}

//
// Affichage fenetre unique
//
// Appele par main.c
//

void aff_fenetre(void)
{
	create_wnd();

	fv_cns_vrb();

	show_wnd();
}
