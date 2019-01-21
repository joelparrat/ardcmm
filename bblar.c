
#include "bblar.h"

void *gp_mmr = 0;							// memoire partagee
Ar   *ar = 0;								// memoire partagee projet ARDuino

char gr_srt[SRT+1][4] = {"LBT", "VBT", "VR1", "VR2", "PR1", "PR2", "LDB", "LDV", "LDR", "ERR", "PMP", "SRC", "SRD", "SRE", "SRT"};
char gr_prm[PRM+1][4] = {"ITP", "IBR", "IMN", "IMX", "OTP", "OBR", "OMN", "OMX", "NEN", "NCP", "NSR", "RSR", "MSR", "CMM", "PRM"};

unsigned char gr_prt[9] = {'\t', '\n', 6, 0x15, '}', '`', '~', 'x', 0};
unsigned char gr_lng[21] = "abcdefghijklmnopqrst";

unsigned char gc_pwm[6] = {3, 5, 6, 9, 10, 11};
unsigned char gc_brcpwm[12] = {0, 0, 0, 1, 0, 2, 3, 0, 0, 4, 5, 6};

void fv_trdmss(char *);																									// traduit message

//
// traduction message
//

void fv_trdmss(char *pr_mss)
{
	char *p, vlr[8], txt[80];
	int  x, y, i, j, err;
	
	strcpy(txt, "==> ");
	p = txt;
	err = 0;
	
	if ((pr_mss[0] != '{') && (pr_mss[0] != '}') && (pr_mss[0] != '|'))
	{
		printf("==> Type de commande inconnue !!! (%c)\n", pr_mss[0]);
		return;
	}
	
	switch (pr_mss[1])
	{
		case 'z':
			strcat(p, "Mode ventilo ");
			break;
			
		case 'y':
			strcat(p, "Nombre de capteur ");
			break;
			
		case 'x':
			strcat(p, "srt[");
			y = pr_mss[2] - 'a';
			strcat(p, gr_srt[y]);
			strcat(p, "][");
			x = pr_mss[3] - 'a';
			strcat(p, gr_prm[x]);
			strcat(p, "]");
			if (pr_mss[0] == '{')
				strcat(p, "=?");
			else
			{
				if (pr_mss[5] == 0)																						// valeur courte 1c
				{
					if (pr_mss[4] == '`')
						i = 0x3FF;
					else if (pr_mss[4] == '~')
						i = 0;
					else
						i = pr_mss[4] - 'a';
				}
				else																									// valeur longue 2c
				{
					i = pr_mss[4] - ' ';
					i <<= 6;
					i += pr_mss[5] - ' ';
					i &= 0x3FF;
				}

				switch (x)
				{
					case ITP:
						if (i == ITP_DGT)
							sprintf(vlr, "=DGT");
						if (i == ITP_ANL)
							sprintf(vlr, "=ANL");
						if (i == ITP_OWR)
							sprintf(vlr, "=OWR");
						break;
						
					case IBR:
						if (ar->srt[y][ITP] == ITP_DGT)
							sprintf(vlr, "=D%d", (i<14)?i:i-14);
						if (ar->srt[y][ITP] == ITP_ANL)
							sprintf(vlr, "=A%d", i);
						if (ar->srt[y][ITP] == ITP_OWR)
							sprintf(vlr, "=T%d", i);
						break;

					case IMN:
					case IMX:
						if (ar->srt[y][ITP] == ITP_DGT)
						{
							if (i == 0)
								sprintf(vlr, "=OFF");
							else
								sprintf(vlr, "=ON");
						}
						else if (ar->srt[y][ITP] == ITP_OWR)
							sprintf(vlr, "=%d", i<<2);
						else
							sprintf(vlr, "=%d", i);
						break;
													
					case OTP:
						if (i == OTP_NO)
							sprintf(vlr, "=NO");
						if (i == OTP_OFF)
							sprintf(vlr, "=OFF");
						if (i == OTP_ON)
							sprintf(vlr, "=ON");
						if (i == OTP_DGT)
							sprintf(vlr, "=DGT");
						if (i == OTP_PWM)
							sprintf(vlr, "=PWM");
						break;
					
					case OBR:
						if (ar->srt[y][OTP] == OTP_NO)
							sprintf(vlr, "=NO");
						if (i < 14)
							sprintf(vlr, "=D%d", i);
						else
							sprintf(vlr, "=A%d", i-14);
						break;
					
					case OMN:
					case OMX:
						if (ar->srt[y][OTP] == OTP_DGT)
						{
							if (i == 0)
								sprintf(vlr, "=OFF");
							else
								sprintf(vlr, "=ON");
						}
						else
							sprintf(vlr, "=%d", i);
						break;
					
					default:
						sprintf(vlr, "=%d", i);
						break;
				}
				strcat(p, vlr);
			}
			break;
			
		case 'w':
			strcat(p, "Adresse capteur no ");
			sprintf(vlr, "%d", pr_mss[2]-'a');
			strcat(p, vlr);
			break;
			
		default:
			err = -4;
			strcat(p, "Nom commande inconnue !!!\n");
			break;
	}
	
	printf("%s\n", txt);
}

