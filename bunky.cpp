#include "stdafx.h"
#include "bunky.h"
#include <time.h>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

void bunky::inicializace()
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
		poskozeni.push_back((rand() % 1001) / 1000.0);
		prah_apop.push_back(0.0001);
		dotyku.push_back(0);
		prekryti.push_back(0);

		rust.push_back(0); // krok zmeny velikosti
		delka_cyklu.push_back(0); // nastavena delka cyklu
		trvani_cyklu.push_back(0); // aktualni doba v cyklu

	}
	oprava = 0.01; // kolik chyb se opravi <0,1>

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


//void bunky::bunky_cyklus(int od_x, int kam_x)
void bunky::bunky_cyklus(double t_G1, double t_S, double t_G2, double t_M, double t_Apop, double t_cekani, double meritko, int vyber)
{
	kolik = size(x);

	for (size_t n = 0; n < kolik; n++)
	{

// poskozeni
		poskozeni[n] = 0.001; // <0, 1> podle toxinu v okoli
		prah_apop[n] = 0.0001 * meritko; // pst apoptozy <0, 1> v zavislosti na meritku (rychlejsi = mene iteraci pro dokonceni cyklu = mene vyhodnoceni pst)



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
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);

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

			mark2 = 0;
			if (kolik_mitogenu > prah_deleni) // pritomnost mitogenu
			{
				mark2 = 1;
			}

			if (poskozeni[n] <= (10 * prah_apop[n])) // vratna/nevratna G0
			{
				if ((mark1 + mark2) == 2)
				{
					stav[n] = 1; // vstup do G1
					trvani_cyklu[n] = 0;
				}
			}


			//if (kolik_mitogenu > prah_deleni2) // v nadprahovem pripade neni reseno malo prostoru a bunka rovnou vstupuje do G1
			//{
			//	stav[n] = 1;
			//	trvani_cyklu[n] = 0;
			//}

// prah RF
			prah_RF = 0.5;
			if (kolik_RF < prah_RF) // nedostatek RF -> bunka umira [Cell Size Regulation in Mammalian Cells.pdf]
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}

			break;

		case 1 : // faze G1

			doba_zivota[n] += 1; // doba zivota bunky
			trvani_cyklu[n] += 1; // doba ve fazi

			mark4 = 1;
			if (dotyku[n] > poc_dot) // maximalni pocet povolenych dotyku
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

// // zpracovani signalnich drah - vypocet parametru (delka cyklu, velikost bunky) // //
				delka_cyklu[n] = (t_S + t_G2 + t_M) / meritko; // delka cyklu (ziviny + mitogen)
				rust[n] = (r[n] * pow(2.0, (1.0 / 3.0)) - r[n]) / delka_cyklu[n]; // krok rustu (ziviny + rustovy faktor)
				prah_ziviny = 0.5;
				prah_poskozeni = 0.5;


				mark1 = 1;
				if (kolik_zivin < prah_ziviny)
				{
					mark1 = 0;
					stav[n] = 0; // prechod do G0
					trvani_cyklu[n] = 0;
				}

				mark2 = 0;
				if (kolik_mitogenu > prah_deleni)
				{
					mark2 = 1;
				}

				mark3 = 1;
				if (poskozeni[n] > prah_poskozeni)
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


// metabolismus?

				trvani_cyklu[n] = trvani_cyklu[n] + 1;

				if ((((rand() % 1000001) / 1000000.0) / poskozeni[n]) < prah_apop[n]) // kontrolni bod
				{
					stav[n] = -1; // apoptoza
					trvani_cyklu[n] = 0;
				}
			}
			else // deleni
			{
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
				r.push_back(r[n] / pow(2.0, (1.0 / 3.0)));
				r.push_back(r[n] / pow(2.0, (1.0 / 3.0)));
				poz_r.push_back(poz_r[n]);
				poz_r.push_back(poz_r[n]);
				stav.push_back(1);
				stav.push_back(1);
				rust.push_back(0);
				rust.push_back(0);
				doba_zivota.push_back(0);
				doba_zivota.push_back(0);
				delka_cyklu.push_back(0);
				delka_cyklu.push_back(0);
				trvani_cyklu.push_back(0);
				trvani_cyklu.push_back(0);
				poskozeni.push_back(poskozeni[n]);
				poskozeni.push_back(poskozeni[n]);
				prah_apop.push_back(prah_apop[n]);
				prah_apop.push_back(prah_apop[n]);
				dotyku.push_back(0);
				dotyku.push_back(0);
				prekryti.push_back(0);
				prekryti.push_back(0);

				
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
				dotyku.erase(dotyku.begin() + n);
				prekryti.erase(prekryti.begin() + n);

			}


			break;

		default :
			cout << "chyba: bunka " << n << " ma stav " << stav_bb << endl;
			break;
		}

// // mutace ?


		poskozeni[n] = poskozeni[n] * (1 - oprava); // opravne mechanismy bunky


	}
	//x.shrink_to_fit();
	//y.shrink_to_fit();
	//z.shrink_to_fit();
	//r.shrink_to_fit();
	//poz_r.shrink_to_fit();
	//stav.shrink_to_fit();
	//rust.shrink_to_fit();
	//doba_zivota.shrink_to_fit();
	//delka_cyklu.shrink_to_fit();
	//trvani_cyklu.shrink_to_fit();
	//poskozeni.shrink_to_fit();
	//prah_apop.shrink_to_fit();
	//dotyku.shrink_to_fit();
	//prekryti.shrink_to_fit();
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
					energie = meritko * -vzdalenost/100;
					en_x = en_x + (vzd_xx * energie);
					en_y = en_y + (vzd_yy * energie);
					en_z = en_z + (vzd_zz * energie);
				}
				else if (vzdalenost <= 20.0) // pohyb k blizkemu okoli
				{
					okoli2 += 1.0;
					energie = meritko * -(0.001 / exp(vzdalenost/5)); // vypocet energie posunu

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
		if (en1 >= en2) // prioritu ma rozdeleni bunek pred priblizenim
		{
			do_x = x[n] + (en_x / okoli);
			do_y = y[n] + (en_y / okoli);
			do_z = z[n] + (en_z / okoli);
		}
		else
		{
			do_x = x[n] + (en_x2 / okoli2);
			do_y = y[n] + (en_y2 / okoli2);
			do_z = z[n] + (en_z2 / okoli2);
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

		// posun bunek
		x2[n] = do_x;
		y2[n] = do_y;
		z2[n] = do_z;
		
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