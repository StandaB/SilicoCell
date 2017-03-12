#include "stdafx.h"
#include "bunky.h"
#include <time.h>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

void bunky::inicializace(double meritko, bool tum)
{
	srand(time(0)); // inicializace generatoru nahodnych cisel


	for (size_t i = 0; i < kolonie; i++)
	{
		//x.push_back((50 * ((rand() % 1001) / 1000.0)) - 0.5);
		//y.push_back((50 * ((rand() % 1001) / 1000.0)) - 0.5);
		//z.push_back((50 * ((rand() % 1001) / 1000.0)) - 0.5);
		x.push_back(posun_x + (10 * ((rand() % 1001) / 1000.0)));
		y.push_back(posun_y + (10 * ((rand() % 1001) / 1000.0)));
		z.push_back((10 * ((rand() % 1001) / 1000.0)));
		r.push_back(r_bunek * (1 - ((((rand() % 1001) / 1000.0) - 0.5) / 20.0)));
		poz_r.push_back(r_bunek);
		//stav.push_back(round(rand() % 4 - 1));
		stav.push_back(1);
		//doba_zivota.push_back(round(10 * ((rand() % 1001) / 1000.0)));
		doba_zivota.push_back(1);
		poskozeni.push_back(((rand() % 1001) / 1000.0));
		prah_apop.push_back(0.0001 / meritko);
		prah_ziviny.push_back(0.5);
		prah_poskozeni.push_back(0.5);
		prah_deleni.push_back(0.1); // mitogeny
		prah_RF.push_back(0.5);
		metabolismus.push_back(0.001);
		dotyku.push_back(0);
		prekryti.push_back(0);
		tumor.push_back(0); // oznaceni zdravych bunek

		rust.push_back((r[i] * pow(2.0, (1.0 / 3.0)) - r[i]) / (780 / meritko)); // krok zmeny velikosti
		delka_cyklu.push_back(780 / meritko); // nastavena delka cyklu
		trvani_cyklu.push_back(0); // aktualni doba v cyklu

	}
	oprava = 0.01; // kolik poskozeni bunky se opravi za 1 iteraci <0,1>

// generovani ECM
	for (size_t i = 0; i < (200*200*200); i++)
	{
		//double c1 = (rand() % 1001) / 1000.0;
		//double c2 = (rand() % 1001) / 1000.0;
		//double c3 = (rand() % 1001) / 1000.0;
		//double soucet_c = c1 + c2 + c3;
		//ECM_x[i] = c1 / soucet_c; // normalizovany nahodny smer v oblastech (1x1x1 = 20x20x20 um)
		//ECM_y[i] = c2 / soucet_c;
		//ECM_z[i] = c3 / soucet_c;

		//// stejny smer
		//ECM_x[i] = 0.8;
		//ECM_y[i] = 0.1;
		//ECM_z[i] = 0.1;

		// bez ucinku
		ECM_x[i] = 1;
		ECM_y[i] = 1;
		ECM_z[i] = 1;


		meta[i] = 0; // koncentrace metabolitu v oblastech (1x1x1 meta = 20x20x20 um)
	}

	if (tum == 1)
	{
		// vytvoreni bunky tumoru
		tumor[1] = 1;
		prah_apop[1] = -1;
		poskozeni[1] = 0;
		delka_cyklu[1] = 500 / meritko; // S+G2+M faze
		rust[1] = (r[1] * pow(2.0, (1.0 / 3.0)) - r[1]) / delka_cyklu[1];
		prah_ziviny[1] = 0.1;
		prah_poskozeni[1] = 2; // rozsah <0,1>, 2 = poskozeni nema vliv
		prah_deleni[1] = -1; // nezavisi na mitogenech
		prah_RF[1] = -1; // nezavisi na RF
		metabolismus[1] = 0.01; // rychlejsi metabolismus
	}

}

void bunky::transform2(int poz_x, int poz_y, int poz_z)
{
	//// vypocet stredu pro otaceni
	//prumer_x = 0;
	//prumer_y = 0;
	//prumer_z = 0;
	//for (size_t i = 0; i < size(x); i++)
	//{
	//	prumer_x = prumer_x + x[i];
	//	prumer_y = prumer_y + y[i];
	//	prumer_z = prumer_z + z[i];
	//}
	//prumer_x = prumer_x / size(x);
	//prumer_y = prumer_y / size(x);
	//prumer_z = prumer_z / size(x);

	// otaceni
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -poz_z);

	glTranslatef(posun_x, posun_y, 0);
	glRotatef(poz_x, 0, 1, 0);
	glRotatef(poz_y, 1, 0, 0); // rotace objektu uprostred
	glTranslatef(-posun_x, -posun_y, 0);
}


void bunky::bunky_cyklus(double t_G1, double t_S, double t_G2, double t_M, double t_Apop, double t_cekani, double meritko, int vyber)
{
	kolik = size(x);

	for (size_t n = 0; n < kolik; n++)
	{
		// // bunecny cyklus // //
		stav_bb = stav[n];
		switch (stav_bb) {

		case -1 : // apoptoza
			// zmensovani velikosti, vymazani

			trvani_cyklu[n] += 1;
			r[n] = r[n] - (r[n] / (t_Apop / meritko)); // zmensovani bunky

			if (trvani_cyklu[n] > (t_Apop / meritko)) { // vymazani bunky
				x.erase(x.begin() + n);
				y.erase(y.begin() + n);
				z.erase(z.begin() + n);
				r.erase(r.begin() + n);
				poz_r.erase(poz_r.begin() + n);
				stav.erase(stav.begin() + n);
				rust.erase(rust.begin() + n);
				doba_zivota.erase(doba_zivota.begin() + n);
				delka_cyklu.erase(delka_cyklu.begin() + n);
				trvani_cyklu.erase(trvani_cyklu.begin() + n);
				poskozeni.erase(poskozeni.begin() + n);
				prah_apop.erase(prah_apop.begin() + n);
				prah_ziviny.erase(prah_ziviny.begin() + n);
				prah_poskozeni.erase(prah_poskozeni.begin() + n);
				prah_deleni.erase(prah_deleni.begin() + n);
				prah_RF.erase(prah_RF.begin() + n);
				metabolismus.erase(metabolismus.begin() + n);
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);
				tumor.erase(tumor.begin() + n);

				pocet_A = pocet_A + 1;
				kolik = size(x);
			}

			break;

		case 0 : // klid G0

			doba_zivota[n] += 1;

			kolik_zivin = vypocty.ziviny(x[n], y[n], z[n], vyber);
			kolik_RF = vypocty.RF(x[n], y[n], z[n], vyber);
			kolik_mitogenu = vypocty.mitogeny(x[n], y[n], z[n], vyber);
			kolik_toxinu = vypocty.toxiny(x[n], y[n], z[n], vyber);

// regulace velikosti (podle zivin a RF)
			//poz_r[n]; // pozadovana velikost bunky (15 pro kuzi)




// regulace poctu (utlacovane jsou ve stresu -> vyssi pst apoptozy)
			if (prekryti[n] > 10)
			{
				poskozeni[n] += kolik_toxinu;
			}


// ne-/vratna faze G0 (podle poskozeni DNA)
			mark1 = 0;
			if (dotyku[n] <= poc_dot) // podle[26] zavisi na poctu dotyku(hustote prostredi)
			{
				trvani_cyklu[n] += 1; // jak dlouho jsou priznive podminky(= synteza proteinu)
				if (trvani_cyklu[n] > (t_cekani / meritko)) // cekani na syntezu proteinu
				{
					mark1 = 1;
				}
			}
			else
			{
				trvani_cyklu[n] = trvani_cyklu[n] - ((trvani_cyklu[n] > 0) - (trvani_cyklu[n] < 0)); // pokles na 0 v nepriznivych podminkach
			}

// prah deleni
			mark2 = 0;
			if (kolik_mitogenu > prah_deleni[n]) // pritomnost mitogenu
			{
				mark2 = 1;
			}

// nevratna faze G0
			//if (poskozeni[n] <= (10 * prah_apop[n])) // vratna/nevratna G0
			//{
				if ((mark1 + mark2) == 2)
				{
					stav[n] = 1; // vstup do G1
					trvani_cyklu[n] = 0;
				}
			//}

//prah_deleni2 = 0.8;
			//if (kolik_mitogenu > prah_deleni2) // v nadprahovem pripade neni reseno malo prostoru a bunka rovnou vstupuje do G1
			//{
			//	stav[n] = 1;
			//	trvani_cyklu[n] = 0;
			//}

// prah RF
			if (kolik_RF < prah_RF[n]) // nedostatek RF -> bunka umira [Cell Size Regulation in Mammalian Cells.pdf]
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}

			break;

		case 1 : // faze G1

			doba_zivota[n] += 1; // doba zivota bunky
			trvani_cyklu[n] += 1; // doba ve fazi

			mark4 = 1;
			if (!tumor[n] && (dotyku[n] > poc_dot)) // maximalni pocet povolenych dotyku, pro tumor nema efekt
			{
				mark4 = 0;
				stav[n] = 0; // prechod do G0
				trvani_cyklu[n] = 0;
			}

			if (trvani_cyklu[n] > (t_G1 / meritko))
			{

				// koncentrace latek v okoli bunky
				kolik_zivin = vypocty.ziviny(x[n], y[n], z[n], vyber);
				kolik_RF = vypocty.RF(x[n], y[n], z[n], vyber);
				kolik_mitogenu = vypocty.mitogeny(x[n], y[n], z[n], vyber);
				kolik_toxinu = vypocty.toxiny(x[n], y[n], z[n], vyber);


				mark1 = 1;
				if (kolik_zivin < prah_ziviny[n])
				{
					mark1 = 0;
					stav[n] = 0; // prechod do G0
					trvani_cyklu[n] = 0;
				}

				mark2 = 0;
				if (kolik_mitogenu > prah_deleni[n])
				{
					mark2 = 1;
				}

				mark3 = 1;
				if (poskozeni[n] > prah_poskozeni[n])
				{
					mark3 = 0;
				}

				// restrikcni bod
				if ((mark1 + mark2 + mark3 + mark4) == 4)
				{
					stav[n] = 2; // vstup do bunecneho cyklu
					trvani_cyklu[n] = 0;
				}
			}
				
			break;

		case 2 : // cyklus

			doba_zivota[n] += 1; // doba ve fazi

			if (trvani_cyklu[n] < delka_cyklu[n])
			{
				r[n] = r[n] + rust[n]; // rust bunky

				trvani_cyklu[n] = trvani_cyklu[n] + 1;

				if ((((rand() % 1000001) / 1000000.0) / poskozeni[n]) < prah_apop[n]) // kontrolni bod
				{
					stav[n] = -1; // apoptoza
					trvani_cyklu[n] = 0;
				}
			}
			else // deleni
			{

				r[n] = r[n] / pow(2.0, (1.0 / 3.0));

// urceni parametru novych bunek ze signalnich drah
				if (!tumor[n])
				{
// poskozeni
					poskozeni[n] = 0.001; // <0, 1> podle toxinu v okoli
					prah_apop[n] = 0.0001 / meritko; // pst apoptozy <0, 1> v zavislosti na meritku (rychlejsi = mene iteraci pro dokonceni cyklu = mene vyhodnoceni pst)

// // zpracovani signalnich drah - vypocet parametru (delka cyklu, velikost bunky) // //
					delka_cyklu[n] = (t_S + t_G2 + t_M) / meritko; // delka cyklu (ziviny + mitogen)
					rust[n] = (r[n] * pow(2.0, (1.0 / 3.0)) - r[n]) / delka_cyklu[n]; // krok rustu (ziviny + rustovy faktor)
					prah_ziviny[n] = 0.5;
					prah_poskozeni[n] = 0.5;
					prah_deleni[n] = 0.1; // mitogeny
					metabolismus[n] = 0.001; // rychlost metabolismu bunky - pricitani v dane oblasti, ovlivnuje poskozeni. Snizovani postupne

// tumor ovlivnuje prah_apop (-1), prah_RF, prah_deleni, prah_poskozeni?, prah_ziviny?




				}


				// vytvoreni dcerinych bunek
				kam_x = ((5.0 * ((rand() % 1001) / 1000.0)) - 0.5);
				kam_y = ((5.0 * ((rand() % 1001) / 1000.0)) - 0.5);
				kam_z = ((5.0 * ((rand() % 1001) / 1000.0)) - 0.5);

				x.push_back(x[n] + kam_x);
				x.push_back(x[n] - kam_x);
				y.push_back(y[n] + kam_y);
				y.push_back(y[n] - kam_y);
				z.push_back(z[n] + kam_z);
				z.push_back(z[n] - kam_z);
				r.push_back(r[n]);
				r.push_back(r[n]);
				poz_r.push_back(poz_r[n]);
				poz_r.push_back(poz_r[n]);
				stav.push_back(1);
				stav.push_back(1);
				rust.push_back(rust[n]);
				rust.push_back(rust[n]);
				doba_zivota.push_back(0);
				doba_zivota.push_back(0);
				delka_cyklu.push_back(delka_cyklu[n]);
				delka_cyklu.push_back(delka_cyklu[n]);
				trvani_cyklu.push_back(0);
				trvani_cyklu.push_back(0);
				poskozeni.push_back(poskozeni[n]);
				poskozeni.push_back(poskozeni[n]);
				prah_apop.push_back(prah_apop[n]);
				prah_apop.push_back(prah_apop[n]);
				prah_ziviny.push_back(prah_ziviny[n]);
				prah_ziviny.push_back(prah_ziviny[n]);
				prah_poskozeni.push_back(prah_poskozeni[n]);
				prah_poskozeni.push_back(prah_poskozeni[n]);
				prah_deleni.push_back(prah_deleni[n]);
				prah_deleni.push_back(prah_deleni[n]);
				prah_RF.push_back(prah_RF[n]);
				prah_RF.push_back(prah_RF[n]);
				metabolismus.push_back(metabolismus[n]);
				metabolismus.push_back(metabolismus[n]);
				dotyku.push_back(dotyku[n]);
				dotyku.push_back(dotyku[n]);
				prekryti.push_back(0);
				prekryti.push_back(0);
				tumor.push_back(tumor[n]);
				tumor.push_back(tumor[n]);

				
				// vymazani materske bunky
				x.erase(x.begin() + n);
				y.erase(y.begin() + n);
				z.erase(z.begin() + n);
				r.erase(r.begin() + n);
				poz_r.erase(poz_r.begin() + n);
				stav.erase(stav.begin() + n);
				rust.erase(rust.begin() + n);
				doba_zivota.erase(doba_zivota.begin() + n);
				delka_cyklu.erase(delka_cyklu.begin() + n);
				trvani_cyklu.erase(trvani_cyklu.begin() + n);
				poskozeni.erase(poskozeni.begin() + n);
				prah_apop.erase(prah_apop.begin() + n);
				prah_ziviny.erase(prah_ziviny.begin() + n);
				prah_poskozeni.erase(prah_poskozeni.begin() + n);
				prah_deleni.erase(prah_deleni.begin() + n);
				prah_RF.erase(prah_RF.begin() + n);
				metabolismus.erase(metabolismus.begin() + n);
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);
				tumor.erase(tumor.begin() + n);

			}


			break;

		default :
			cout << "chyba: bunka " << n << " ma stav " << stav_bb << endl;
			break;
		}

// apoptoza kdyz je bunka bez dotyku
		//if (dotyku[n]==0)
		//{
		//	stav[n] = -1;
		//	trvani_cyklu[n] = 0;
		//}

// // mutace ?


		poskozeni[n] = poskozeni[n] * (1 - oprava); // opravne mechanismy bunky

// oznaceni bunek tumoru
		double tum_cyk = 500.0 / meritko; // najit zdroj rychlosti deleni tumorovych bunek!!!
		if (delka_cyklu[n] < tum_cyk)
		{
			tumor[n] = 1;
		}
		if (prah_apop[n] == -1)
		{
			tumor[n] = 1;
		}
//// zatim nahodne urceni
//		if (((rand() % 100001) / 100000.0) < (0.000001 * meritko))
//		{
//			tumor[n] = 1;
//			prah_apop[n] == -1;
//		}

// metabolismus
		int kde_x = round(x[n] / 20.0);
		int kde_y = round(y[n] / 20.0);
		int kde_z = round(z[n] / 20.0);

		int f = kde_x + (kde_y + 200 - 1) + (kde_z + ((200 - 1) * (200 - 1)));

		meta[f] += metabolismus[n]; // 

	}

// snizovani koncentrace metabolitu (lymf. system atd.)
	for (size_t i = 0; i < (200*200*200); i++)
	{
		meta[i] = meta[i] * 0.9;
	}

}


void bunky::pohyb(double meritko, bool omezeni, double omezeni_x, double omezeni_z)
{
	kolik = size(x);
	vector<double> x2(kolik);
	vector<double> y2(kolik);
	vector<double> z2(kolik);

	//for (size_t n = 0; n < kolik; n++) // prochazeni vsech bunek
	for (size_t n = 0; n < kolik; n++)
	{
		touch = 0;
		prekryv = 0.0;
		okoli = 0.0;
		okoli2 = 0.0;
		en_x = 0.0; en_y = 0.0; en_z = 0.0;
		en_x2 = 0.0; en_y2 = 0.0; en_z2 = 0.0;
		vzd_xx = 0; vzd_yy = 0; vzd_zz = 0;
		en1 = 0; en2 = 0;
		vector<double> vzd(kolik);

		for (size_t i = 0; i < kolik; i++) // vytvoreni vektoru vzdalenosti vsech bunek od n
		{
			vzd_xx = (x[n] - x[i]);
			vzd_yy = (y[n] - y[i]);
			vzd_zz = (z[n] - z[i]);
			vzdalenost = sqrt(pow(vzd_xx, 2.0) + pow(vzd_yy, 2.0) + pow(vzd_zz, 2.0)) - (r[n] + r[i]);
			vzd[i] = vzdalenost;

			//if (sqrt(pow(vzd_xx, 2.0) + pow(vzd_yy, 2.0) + pow(vzd_zz, 2.0)) < 0.01) // nahodny posun kdyz se prekryvaji
			//{
			//	cout << n << endl;
			//}
			
			if ((i != n))
			{
				if (vzdalenost <= 0.0) // dotyky a prekryti
				{
					touch += 1;
					okoli += 1.0;
					prekryv = prekryv + abs(vzdalenost);

					// vypocet energie posunu - deleni bunek (prekryti)
					energie = meritko * -vzdalenost/200;
					en_x = en_x + (vzd_xx * energie);
					en_y = en_y + (vzd_yy * energie);
					en_z = en_z + (vzd_zz * energie);
				}
				else if (vzdalenost <= 20.0) // pohyb k blizkemu okoli
				{
					okoli2 += 1.0;
					energie = meritko * -(0.001 / exp(vzdalenost/10)); // vypocet energie posunu

					en_x2 = en_x2 + (vzd_xx * energie);
					en_y2 = en_y2 + (vzd_yy * energie);
					en_z2 = en_z2 + (vzd_zz * energie);
				}
			}
		}


		vzd[n] = 100000; // vynechani pozice n (== 0)
		auto minimum = min_element(vzd.begin(), vzd.end()); // (bez nejblizsiho - pro delici se bunky)
		prekryti[n] = prekryv - abs(*minimum);
		dotyku[n] = touch;

		en1 = abs(sqrt(pow(en_x, 2.0) + pow(en_y, 2.0) + pow(en_z, 2.0)));
		en2 = abs(sqrt(pow(en_x2, 2.0) + pow(en_y2, 2.0) + pow(en_z2, 2.0)));

		// vypocet posuvu
		int kde_x = round(x[n] / 20.0);
		int kde_y = round(y[n] / 20.0);
		int kde_z = round(z[n] / 20.0);

		int f = kde_x + (kde_y + 200 - 1) + (kde_z + ((200 - 1) * (200 - 1))); // pro pocet oblasti ECM 200*200*200. 1 oblast = 20*20*20 um

		if (en1 >= en2) // prioritu ma rozdeleni bunek pred priblizenim
		{
			do_x = x[n] + ((en_x / okoli) * ECM_x[f]); // ECM ovlivnuje jak snadno se v danem smeru bunka pohybuje
			do_y = y[n] + ((en_y / okoli) * ECM_y[f]);
			do_z = z[n] + ((en_z / okoli) * ECM_z[f]);
		}
		else
		{
			do_x = x[n] + ((en_x / okoli2) * ECM_x[f]);
			do_y = y[n] + ((en_y / okoli2) * ECM_y[f]);
			do_z = z[n] + ((en_z / okoli2) * ECM_z[f]);
		}
		

//// omezeni rustu za hranici
		if (omezeni == 1)
		{
			//if ((abs(do_x) > 100.0) && (stav[n] == 1))
			if (abs(do_x - posun_x) > omezeni_x)
			{
				do_x = x[n];
				dotyku[n] += 1;
				//stav[n] = 0;
			}
			if (((do_y - posun_y) < 0.0))
			{
				do_y = y[n];
				dotyku[n] += 1;
			}
			if ((abs(do_z) > omezeni_z))
			{
				do_z = z[n];
				dotyku[n] += 1;
				//stav[n] = 0;
			}
		}

		// zamezeni rustu do zapornych cisel
		if (do_y < 0.0)
		{
			do_y = y[n];
			dotyku[n] += 1;
		}
		if (do_x < 0.0)
		{
			do_x = x[n];
			dotyku[n] += 1;
		}

		// posun bunek
		x2[n] = (do_x);
		y2[n] = (do_y);
		z2[n] = (do_z);
		
	}
	x.clear();
	y.clear();
	z.clear();
	x = x2;
	y = y2;
	z = z2;
}

void bunky::zmena_rozliseni(int pos_x, int pos_y)
{
	posun_x = pos_x / 2;
	posun_y = (pos_y / 2) - 100;
}