
#include <gtk/gtk.h>
#include "intgrp.h"
#include "bbliv.h"
#include "bblen.h"

IV iv = {"joel", "ardcmm", "0.0", "04/2017", "parrat"};
EN en;

int main(int argc, char **argv, char **envv)
{
	en_get(argc, argv, envv);

	gtk_init(&argc, &argv);
	aff_fenetre();
	gtk_main();

	return(0);
}
