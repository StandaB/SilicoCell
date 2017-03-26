#include "stdafx.h"
#include "bunky.h"
#include <time.h>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <chrono>
#include <fstream>
#include <string>


void bunky::inicializace(double meritko, bool tum)
{
	srand(time(0)); // inicializace generatoru nahodnych cisel

	// nacteni parametru z config.ini
	string prvni, druhy;
	vector<double> param;
	ifstream soubor("config.ini");
	if (soubor.is_open())
	{
		while (soubor >> prvni >> druhy) // kazde volani getline skoci na dalsi radek, na konci = 0
		{
			param.push_back(std::stod(druhy));
			//cout << druhy << endl;
		}
		soubor.close();

		// zapis parametru do promennych
		int zf = 8; // zacatek nastaveni VYPOCTY
		prostor = param[zf]; // vstup do vypoctu prostorovych koncentraci
		kolonie = param[zf + 2];
		r_bunek = param[zf + 3];
		preskok = param[zf + 4];
		meze = param[zf + 5];
		rozl = param[zf + 6];
		poc_dot = param[zf + 7];
		snizovani = param[zf + 8];
		oprava = param[zf + 9]; // kolik poskozeni bunky se opravi za 1 iteraci <0,1>
		metabolismus_0 = param[zf + 10];
		deska = param[zf + 11];
		supresory = param[zf + 12];
	}

	// inicializace pocatecni kolonie
	for (size_t i = 0; i < kolonie; i++)
	{
		x.push_back(posun_x + (30 * (((rand() % 1001) / 1000.0) - 0.5)));
		y.push_back(posun_y + (30 * (((rand() % 1001) / 1000.0) - 0.5)));
		z.push_back(30 * (((rand() % 1001) / 1000.0) - 0.5));
		r.push_back(r_bunek * (1 - ((((rand() % 1001) / 1000.0) - 0.5) / 20.0)));
		poz_r.push_back(r_bunek);
		stav.push_back(1);
		doba_zivota.push_back(1);
		poskozeni.push_back(((rand() % 1001) / 1000.0));
		prah_apop.push_back(0.0001 / meritko);
		prah_ziviny.push_back(0);
		prah_poskozeni.push_back(0.5);
		prah_deleni.push_back(0.5); // RF
		metabolismus.push_back(0);
		dotyku.push_back(0);
		prekryti.push_back(0);
		tumor.push_back(0); // oznaceni zdravych bunek
		navrat.push_back(1);

		rust.push_back((r[i] * pow(2.0, (1.0 / 3.0)) - r[i]) / (780 / meritko)); // krok zmeny velikosti
		delka_cyklu.push_back(780 / meritko); // nastavena delka cyklu
		trvani_cyklu.push_back(0); // aktualni doba v cyklu

	}

// generovani ECM
	for (size_t i = 0; i < (meze*meze*meze); i++)
	{
		//double c1 = (rand() % 1001) / 1000.0;
		//double c2 = (rand() % 1001) / 1000.0;
		//double c3 = (rand() % 1001) / 1000.0;
		//double soucet_c = c1 + c2 + c3;
		//ECM_x[i] = c1 / soucet_c; // normalizovany nahodny smer v oblastech (1x1x1 = 20x20x20 um nebo rozl)
		//ECM_y[i] = c2 / soucet_c;
		//ECM_z[i] = c3 / soucet_c;

		//// stejny smer
		//ECM_x[i] = 0.8;
		//ECM_y[i] = 0.1;
		//ECM_z[i] = 0.1;

		//// bez ucinku
		//ECM_x[i] = 1;
		//ECM_y[i] = 1;
		//ECM_z[i] = 1;


		meta.push_back(0); // koncentrace metabolitu v oblastech (1x1x1 meta = 20x20x20 um nebo podle rozl)
		zvny.push_back(0); // ziviny v prostoru
	}

	if (tum == 1)
	{
		// vytvoreni bunky tumoru
		x[1] = posun_x; // uprostred kolonie
		y[1] = posun_y;
		z[1] = 0;
		tumor[1] = 1;
		prah_apop[1] = -1;
		poskozeni[1] = 0;
		rust[1] = (r[1] * pow(2.0, (1.0 / 3.0)) - r[1]) / delka_cyklu[1];
		if (supresory)
		{
			prah_poskozeni[1] = 2; // rozsah <0,1>, 2 = poskozeni nema vliv
		}
	}

}

void bunky::transform2(int poz_x, int poz_y, int poz_z, float screen_width, float screen_height)
{
	// vypocet stredu pro otaceni
	prumer_x = 0.0;
	prumer_y = 0.0;
	for (size_t i = 0; i < size(x); i++)
	{
		prumer_x = prumer_x + x[i];
		prumer_y = prumer_y + y[i];
	}
	prumer_x = prumer_x / size(x);
	prumer_y = prumer_y / size(x);

	// otaceni
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -poz_z);

	glTranslatef((screen_width / 2) + prumer_x, (screen_height / 2) + prumer_y, 0);
	glRotatef(poz_x, 0, 1, 0);
	glRotatef(poz_y, 1, 0, 0); // rotace objektu uprostred
	glTranslatef(-(screen_width / 2) - prumer_x, -(screen_height / 2) - prumer_y, 0);
}


void bunky::bunky_cyklus(double t_G1, double t_S, double t_G2, double t_M, double t_Apop, double t_cekani, double meritko, int vyber)
{
	kolik = size(x);

	for (size_t n = 0; n < kolik; n++)
	{

		// koncentrace latek v okoli bunky
		kolik_zivin = vypocty.ziviny(x[n], y[n], z[n], vyber, prostor);
		kyslik = vypocty.kyslik(x[n], y[n], z[n], vyber, prostor);
		kolik_RF = vypocty.RF(x[n], y[n], z[n], vyber, prostor);
		kolik_toxinu = vypocty.toxiny(x[n], y[n], z[n], vyber, prostor);

		// metabolity
		int kde_x = round(x[n] / rozl);
		int kde_y = round(y[n] / rozl);
		int kde_z = round(z[n] / rozl);
		int f = kde_x + (kde_y + meze - 1) + (kde_z + ((meze - 1) * (meze - 1)));
		double mtb = meta[f];
		double ziv = zvny[f]; // spotreba zivin v regionu f
		kolik_zivin = kolik_zivin - ziv;
		if (kolik_zivin < 0)
		{
			kolik_zivin = 0;
		}
		if (kolik_RF < 0)
		{
			kolik_RF = 0;
		}

// poskozeni
		poskozeni[n] += kolik_toxinu / 1000; // podle toxinu v okoli

	// // zpracovani signalnich drah - vypocet parametru (delka cyklu, velikost bunky) // //
		delka_cyklu[n] = t_S + t_G2 + t_M; // delka cyklu
		if (tumor[n] == 1)
		{
			//kolik_zivin = 1.0;
			kolik_RF = 1.0;
		}

		prah_deleni[n] = 0.5 - kolik_zivin;
		if (prah_deleni[n] < 0)
		{
			prah_deleni[n] = 0;
		}

		int Akt = 0;
		if (kolik_RF > prah_deleni[n])
		{
			Akt = 1;
		}

		prah_ziviny[n] = 0.2 * Akt;

		bool prechod_G1 = 0;
		if ((kolik_zivin >= prah_ziviny[n]) && (Akt == 1))
		{
			prechod_G1 = 1;
		}
		else if (tumor[n] == 1)
		{
			prechod_G1 = 1;
		}


		double HIF1 = 0, PHD = 0, PDH = 0;

		PHD = (1 - mtb) * (kyslik);
		HIF1 = (round((Akt + metabolismus[n])) / 2.0) * (1 - PHD);
		PDH = 1 - HIF1;

		metabolismus[n] = (kolik_RF + kolik_zivin) * prechod_G1 * PDH * metabolismus_0;
		if (metabolismus[n] < 0)
		{
			metabolismus[n] = 0;
		}

		mtb += metabolismus[n]; // pricitani metabolitu z bunky do prostoru
		if (mtb > 1)
		{
			mtb = 1;
		}
		meta[f] = mtb; // mnozstvi metabolitu v prostoru
		zvny[f] = 2 * metabolismus[n]; // spotreba zivin odpovida rychlosti metabolismu (50% ucinnost)

		delka_cyklu[n] = (delka_cyklu[n] * (1 - metabolismus[n])) / meritko; // rychlejsi metabolismus -> kratsi cyklus

		rust[n] = (r[n] * pow(2.0, (1.0 / 3.0)) - r[n]) / delka_cyklu[n]; // krok rustu (ziviny + rustovy faktor)


		// // bunecny cyklus // //
		stav_bb = stav[n];
		switch (stav_bb) {

		case -1 : // apoptoza - zmensovani velikosti, vymazani

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
				metabolismus.erase(metabolismus.begin() + n);
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);
				tumor.erase(tumor.begin() + n);
				navrat.erase(navrat.begin() + n);

				pocet_A = pocet_A + 1;
				kolik = size(x);
			}

			break;

		case 0 : // klid G0

			doba_zivota[n] += 1;


// regulace velikosti (podle zivin a RF)
			//poz_r[n]; // pozadovana velikost bunky (15 pro kuzi)


// regulace poctu (utlacovane jsou ve stresu -> vyssi pst apoptozy)
			kolik_toxinu += mtb;
			poskozeni[n] += (kolik_toxinu / 1000.0) * (1 + tumor[n]); // tumor ma vic poskozeni (mutace)
			if (prekryti[n] > 10)
			{
				poskozeni[n] += 0.001;
			}


			// ne-/vratna faze G0 (podle poskozeni DNA)
			mark1 = 0;
			if ((poskozeni[n] >= 0.25))
			{
				navrat[n] = 0;
			}
			if ((dotyku[n] <= poc_dot) && navrat[n]) // podle[26] zavisi na poctu dotyku(hustote prostredi)
			{
				trvani_cyklu[n] += 1; // jak dlouho jsou priznive podminky(= synteza proteinu)
				if (trvani_cyklu[n] > (t_cekani / meritko)) // cekani na syntezu proteinu
				{
					mark1 = 1;
				}
			}
			else
			{
				trvani_cyklu[n] = trvani_cyklu[n] - 1; // pokles na 0 v nepriznivych podminkach
				if (trvani_cyklu[n] < 0)
				{
					trvani_cyklu[n] = 0;
				}
			}


			if ((mark1 + prechod_G1) == 2)
			{
				stav[n] = 1; // vstup do G1
				trvani_cyklu[n] = 0;
			}

			if (kolik_RF < 0.01) // nedostatek RF -> bunka umira [Cell Size Regulation in Mammalian Cells.pdf]
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if (kolik_zivin < 0.01)
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if (kyslik < 0.01)
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if (poskozeni[n] > prah_poskozeni[n])
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}

			break;

		case 1 : // faze G1

			doba_zivota[n] += 1; // doba zivota bunky
			trvani_cyklu[n] += 1; // doba ve fazi

			mark3 = 1;
			if ((dotyku[n] > poc_dot) && ((tumor[n] - supresory) != 1)) // maximalni pocet povolenych dotyku, pro tumor bez supresoru nema efekt
			{
				mark3 = 0;
				stav[n] = 0; // prechod do G0
				trvani_cyklu[n] = 0;
			}
			if ((kolik_zivin == 0) && ((tumor[n] - supresory) != 1))
			{
				mark3 = 0;
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if ((kyslik == 0) && ((tumor[n] - supresory) != 1))
			{
				mark3 = 0;
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}

			if ((trvani_cyklu[n] > (t_G1 / meritko)) && (stav[n] == 1))
			{
				mark1 = 1;
				if (!prechod_G1)
				{
					mark1 = 0;
					stav[n] = 0; // prechod do G0
					trvani_cyklu[n] = 0;
				}

				mark2 = 1;
				if (poskozeni[n] > prah_poskozeni[n])
				{
					mark2 = 0;
					stav[n] = -1; // apoptoza
					trvani_cyklu[n] = 0;
				}

				// restrikcni bod
				if ((mark1 + mark2 + mark3) == 3)
				{
					stav[n] = 2; // vstup do bunecneho cyklu
					trvani_cyklu[n] = 0;
				}
			}

			// tumor preskakuje G1
			if (((preskok + tumor[n]) == 2) && (stav[n] == 1))
			{
				stav[n] = 2; // vstup do bunecneho cyklu
				trvani_cyklu[n] = 0;
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

				// vytvoreni dcerinych bunek
				srand(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()); // nahodna cisla
				kam_x = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				kam_y = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				kam_z = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);

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
				metabolismus.push_back(metabolismus[n]);
				metabolismus.push_back(metabolismus[n]);
				dotyku.push_back(dotyku[n]);
				dotyku.push_back(dotyku[n]);
				prekryti.push_back(0);
				prekryti.push_back(0);
				tumor.push_back(tumor[n]);
				tumor.push_back(tumor[n]);
				navrat.push_back(navrat[n]);
				navrat.push_back(navrat[n]);
				
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
				metabolismus.erase(metabolismus.begin() + n);
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);
				tumor.erase(tumor.begin() + n);
				navrat.erase(navrat.begin() + n);

			}


			break;

		default :
			cout << "chyba: bunka " << n << " ma stav " << stav_bb << endl;
			break;
		}

// // mutace ?


		poskozeni[n] = poskozeni[n] * (1 - (oprava * meritko)); // opravne mechanismy bunky

	}

	//snizovani koncentrace metabolitu (lymf. system atd.)
	for (size_t i = 0; i < (meze*meze*meze); i++)
	{
		meta[i] = meta[i] * (1 - (snizovani * meritko));
		zvny[i] = zvny[i] * (1 - (snizovani * meritko));
	}

}


void bunky::pohyb(double meritko, bool omezeni, double omezeni_x, double omezeni_z)
{
	kolik = size(x);

#pragma loop(hint_parallel(2)) 
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
			
			if ((i != n))
			{
				if (vzdalenost <= 0.0) // dotyky a prekryti
				{
					touch += 1;
					okoli += 1.0;
					prekryv = prekryv + abs(vzdalenost);

					// vypocet energie posunu - deleni bunek (prekryti)
					energie = meritko * -vzdalenost/100;
					if (energie > 0.3)
					{
						energie = 0.3;
					}
					en_x = en_x + (vzd_xx * energie);
					en_y = en_y + (vzd_yy * energie);
					en_z = en_z + (vzd_zz * energie);
				}
				else if (vzdalenost <= 20.0) // pohyb k blizkemu okoli
				{
					okoli2 += 1.0;
					energie = meritko * -(0.001 / exp(vzdalenost/10)); // vypocet energie posunu
					if (energie > 0.5)
					{
						energie = 0.5;
					}

					en_x2 = en_x2 + (vzd_xx * energie);
					en_y2 = en_y2 + (vzd_yy * energie);
					en_z2 = en_z2 + (vzd_zz * energie);
				}
			}
		}


		vzd[n] = 100000; // vynechani pozice n (== stejna bunka)
		auto minimum = min_element(vzd.begin(), vzd.end()); // (bez nejblizsiho - pro delici se bunky)
		prekryti[n] = prekryv - abs(*minimum);
		dotyku[n] = touch;

		en1 = abs(sqrt(pow(en_x, 2.0) + pow(en_y, 2.0) + pow(en_z, 2.0)));
		en2 = abs(sqrt(pow(en_x2, 2.0) + pow(en_y2, 2.0) + pow(en_z2, 2.0)));

		// vypocet posuvu
		//int kde_x = round(x[n] / rozl);
		//int kde_y = round(y[n] / rozl);
		//int kde_z = round(z[n] / rozl);
		//
		//int f = kde_x + (kde_y + meze - 1) + (kde_z + ((meze - 1) * (meze - 1))); // pro pocet oblasti ECM 200*200*200. 1 oblast = 20*20*20 um nebo podle rozl
		//
		//if (en1 >= en2) // prioritu ma rozdeleni bunek pred priblizenim
		//{
		//	do_x = x[n] + ((en_x / okoli) * ECM_x[f]); // ECM ovlivnuje jak snadno se v danem smeru bunka pohybuje
		//	do_y = y[n] + ((en_y / okoli) * ECM_y[f]);
		//	do_z = z[n] + ((en_z / okoli) * ECM_z[f]);
		//}
		//else
		//{
		//	do_x = x[n] + ((en_x / okoli2) * ECM_x[f]);
		//	do_y = y[n] + ((en_y / okoli2) * ECM_y[f]);
		//	do_z = z[n] + ((en_z / okoli2) * ECM_z[f]);
		//}

		if ((en1 + (prekryv / 10)) >= en2) // prioritu ma rozdeleni bunek pred priblizenim, cim vic prekryti tim spis se bunky posunou od sebe
		{
			do_x = x[n] + ((en_x / okoli)); // ECM ovlivnuje jak snadno se v danem smeru bunka pohybuje
			do_y = y[n] + ((en_y / okoli));
			do_z = z[n] + ((en_z / okoli));
		}
		else
		{
			do_x = x[n] + ((en_x2 / okoli2));
			do_y = y[n] + ((en_y2 / okoli2));
			do_z = z[n] + ((en_z2 / okoli2));
		}
		

	// omezeni rustu za hranici
		if (omezeni == 1)
		{
			//if ((abs(do_x) > 100.0) && (stav[n] == 1))
			if (abs(do_x - posun_x) > omezeni_x)
			{
				do_x = x[n];
				dotyku[n] += 1;
				//stav[n] = 0;
			}
			if (deska == 1)
			{
				if (((do_y - posun_y) > 0.0))
				{
					do_y = y[n];
					dotyku[n] += 1;
				}
			}
			if ((abs(do_z) > omezeni_z))
			{
				do_z = z[n];
				dotyku[n] += 1;
				//stav[n] = 0;
			}
		}

		// posun bunek
		x[n] = (do_x);
		y[n] = (do_y);
		z[n] = (do_z);
		
	}
}