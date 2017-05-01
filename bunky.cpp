#include "stdafx.h"
#include "bunky.h"


void bunky::inicializace(double meritko)
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
		int zf = 10; // zacatek nastaveni VYPOCTY
		prostor = param[zf]; // vstup do vypoctu prostorovych koncentraci
		pokles = param[zf + 1];
		kolonie = param[zf + 3];
		r_bunek = param[zf + 4];
		t_tumor = param[zf + 5];
		poc_dot = param[zf + 6];
		snizovani = param[zf + 7];
		oprava = param[zf + 8]; // kolik poskozeni bunky se opravi za 1 iteraci <0,1>
		deska = param[zf + 9];
		supresory = param[zf + 10];
		poskozeni_b = param[zf + 11];
		prah_ap = param[zf + 12];
	}

	// inicializace pocatecni kolonie
	for (size_t i = 0; i < kolonie; i++)
	{
		x.push_back(posun_x + (40 * (((rand() % 1001) / 1000.0) - 0.5)));
		y.push_back(posun_y + (40 * (((rand() % 1001) / 1000.0) - 0.5)));
		z.push_back(40 * (((rand() % 1001) / 1000.0) - 0.5));
		r.push_back(r_bunek * (1 - ((((rand() % 1001) / 1000.0) - 0.5) / 20.0)));
		poz_r.push_back(r_bunek);
		stav.push_back(1);
		doba_zivota.push_back(0);
		poskozeni.push_back(((rand() % 1001) / 1000.0));
		prah_apop.push_back(prah_ap);
		prah_ziviny.push_back(0);
		prah_poskozeni.push_back(poskozeni_b);
		prah_deleni.push_back(0.5); // RF
		metabolismus.push_back(0);
		dotyku.push_back(0);
		prekryti.push_back(0);
		tumor.push_back(0); // oznaceni zdravych bunek
		navrat.push_back(1);
		meta.push_back(0);
		zvny.push_back(0);

		rust.push_back((r[i] * pow(2.0, (1.0 / 3.0)) - r[i]) / (900 / meritko)); // krok zmeny velikosti
		delka_cyklu.push_back(900 / meritko); // nastavena delka cyklu
		trvani_cyklu.push_back(0); // aktualni doba v cyklu

	}

	// generovani ECM
	//for (size_t i = 0; i < (meze*meze*meze); i++)
	//{
	//	//double c1 = (rand() % 1001) / 1000.0;
	//	//double c2 = (rand() % 1001) / 1000.0;
	//	//double c3 = (rand() % 1001) / 1000.0;
	//	//double soucet_c = c1 + c2 + c3;
	//	//ECM_x[i] = c1 / soucet_c; // normalizovany nahodny smer v oblastech (1x1x1 = 20x20x20 um nebo rozl)
	//	//ECM_y[i] = c2 / soucet_c;
	//	//ECM_z[i] = c3 / soucet_c;

	//	//// stejny smer
	//	//ECM_x[i] = 0.8;
	//	//ECM_y[i] = 0.1;
	//	//ECM_z[i] = 0.1;

	//	//// bez ucinku
	//	//ECM_x[i] = 1;
	//	//ECM_y[i] = 1;
	//	//ECM_z[i] = 1;


	//	// koncentrace v oblastech (1x1x1 meta = 20x20x20 um nebo podle rozl)
	//	//zvny.push_back(0); // ziviny v prostoru
	//}
}

void bunky::ini2()
{
	int mini = (sqrt(pow(x[0] - posun_x, 2.0) + pow(y[0] - posun_y, 2.0) + pow(z[0], 2.0)));
	int kde = 0;
	for (size_t i = 0; i < size(x); i++)
	{
		if ((sqrt(pow(x[i] - posun_x, 2.0) + pow(y[i] - posun_y, 2.0) + pow(z[i], 2.0))) <= mini)
		{
			kde = i;
			mini = (sqrt(pow(x[i] - posun_x, 2.0) + pow(y[i] - posun_y, 2.0) + pow(z[i], 2.0)));
		}
	}
	// vytvoreni bunky tumoru
	tumor[kde] = 1;
	prah_apop[kde] = -1;
	//if (!supresory)
	//{
	//	prah_poskozeni[kde] = poskozeni_tum; // rozsah <0,1>, 2 = poskozeni nema vliv
	//}
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
	//glTranslatef(0, 0, -poz_z);

	glTranslatef((screen_width / 2) + prumer_x, (screen_height / 2) + prumer_y, 0);
	glRotatef(poz_x, 0, 1, 0);
	glRotatef(poz_y, 1, 0, 0); // rotace objektu uprostred
	glTranslatef(-(screen_width / 2) - prumer_x, -(screen_height / 2) - prumer_y, 0);
}

template <class Type> class nasobeni_vec
{
private:

	Type kolikrat;

public:

	nasobeni_vec(const Type &_Val) : kolikrat(_Val) { }
	double operator()(Type &vec) const
	{
		return (vec * kolikrat);
	}

};

void bunky::bunky_cyklus(double nastaveni[])
{
	double t_G1 = nastaveni[0];
	double t_S = nastaveni[1];
	double t_G2 = nastaveni[2];
	double t_M = nastaveni[3];
	double t_Apop = nastaveni[4];
	double t_cekani = nastaveni[5];
	double meritko = nastaveni[6];
	int vyber = nastaveni[7];
	int omezeni_r = nastaveni[8];
	bool niceni = nastaveni[9];



	kolik = size(x);

	for (size_t n = 0; n < kolik; n++)
	{

		// koncentrace latek v okoli bunky
		kolik_zivin = vypocty.ziviny(x[n], y[n], z[n], vyber, prostor, pokles);
		kyslik = vypocty.kyslik(x[n], y[n], z[n], vyber, prostor, pokles);
		kolik_RF = vypocty.RF(x[n], y[n], z[n], vyber, prostor, pokles);

		if (niceni)
		{
			kolik_toxinu = vypocty.toxiny(x[n], y[n], z[n], vyber, prostor, pokles);
		}
		else
		{
			kolik_toxinu = 0.0;
		}

		// metabolity
		//int kde_x = round(x[n] / rozl);
		//int kde_y = round(y[n] / rozl);
		//int kde_z = round(z[n] / rozl);
		//int f = kde_x + (kde_y + meze - 1) + (kde_z + ((meze - 1) * (meze - 1)));
		kolik_zivin = kolik_zivin - zvny[n]; // spotreba zivin;
		if (kolik_zivin < 0.0)
		{
			kolik_zivin = 0.0;
		}

	// // zpracovani signalnich drah - vypocet parametru (delka cyklu, velikost bunky) // //
		//delka_cyklu[n] = t_S + t_G2 + t_M; // delka cyklu

		if (tumor[n] == 1) // tumor si vytvari vlastni RF
		{
			//kolik_zivin = 1.0;
			kolik_RF = 1.0;
		}

		prah_deleni[n] = 0.5 - kolik_zivin;
		prah_ziviny[n] = 0.1;

		if (prah_deleni[n] < 0.1)
		{
			prah_deleni[n] = 0.1;
		}

		bool Akt = 0;
		if (kolik_RF > prah_deleni[n])
		{
			Akt = 1;
		}

		bool prechod_G1 = 0;
		if (kolik_zivin >= prah_ziviny[n])
		{
			if (Akt == 1)
			{
				prechod_G1 = 1;
			}
		}
		if (tumor[n] == 1) // tumor vzdy pokracuje
		{
			prechod_G1 = 1;
			//poskozeni[n] += 0.0001; // pri nedostatku zivin dochazi k autofagii
		}


		double HIF1 = 0.0, PHD = 0.0, PDH = 0.0;

		PHD = (kyslik + (1.0 - meta[n])) / 2.0;
		HIF1 = ((double(Akt) + metabolismus[n]) + (1.0 - PHD)) / 3.0;
		PDH = 1.0 - HIF1;

		metabolismus[n] = (((kolik_RF + kolik_zivin) * double(prechod_G1)) + PDH) / 3.0;

		meta[n] = (meta[n] + metabolismus[n] / 2.0) * (1.0 - snizovani); // mnozstvi metabolitu v prostoru
		zvny[n] = (zvny[n] + metabolismus[n] / 2.0) * (1.0 - snizovani); // spotreba zivin odpovida rychlosti metabolismu


		// regulace poctu (utlacovane a poskozene jsou ve stresu -> vyssi pst apoptozy)
		if (doba_zivota[n] > (200.0 / meritko)) // nove rozdelene bunky maji cas se od sebe odsunout
		{
			poskozeni[n] = poskozeni[n] + kolik_toxinu + ((meta[n] + prekryti[n]/2.5) / 10.0); // vysoke prekryti bunek (utlacovani) a toxiny v okoli zpusobuji poskozeni
		}

		// regulace velikosti
		double TOR = ((10.0 + (double(Akt) * (kolik_RF + kolik_zivin + kyslik) / 3.0)) / 10.0) - 0.05; // hodnoty 0.95 - 1.05 (5 % rozsah polomeru)
		//poz_r[n] = (r_bunek - (tumor[n] * preskok) - (prekryti[n] / 20.0)) * TOR;
		poz_r[n] = (r_bunek - (prekryti[n] / 20.0)) * TOR;

		if (poz_r[n] < (r_bunek / 2.0))
		{
			poz_r[n] = r_bunek / 2.0;
		}

		if (r[n] < (r_bunek / 2.0))
		{
			r[n] = r_bunek / 2.0;
		}

		//rust[n] = (poz_r[n] * pow(2.0, (1.0 / 3.0)) - poz_r[n]) / delka_cyklu[n]; // krok rustu


		//if (poskozeni[n] < 1.0e-6) // mensi hodnoty poskozeni nema smysl ukladat
		//{
		//	poskozeni[n] = 0.0;
		//}

		if (stav[n] != -1)
		{
			if (kolik_RF < 0.05) // nedostatek RF -> bunka umira [Cell Size Regulation in Mammalian Cells.pdf]
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if (kolik_zivin < 0.05)
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if (kyslik < 0.05)
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
			if ((tumor[n] == 1) && (supresory == 1)) // tumor supresory
			{
				stav[n] = -1; // apoptoza
				trvani_cyklu[n] = 0;
			}
		}

		if (x[n] != x[n]) // osetreni hodnot NaN pri chybe vypoctu
		{
			stav[n] = -1; // apoptoza
			trvani_cyklu[n] = 10000;
		}


// // bunecny cyklus // //
		stav_bb = stav[n];
		mt19937 gen(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count()); // seed pro nahodna cisla pst
		uniform_int_distribution<unsigned long long> nah_c(0.0, 1000000000.0);

		switch (stav_bb) {

		case -1: // apoptoza - zmensovani velikosti, vymazani

			trvani_cyklu[n]++;
			rust[n] = (poz_r[n] / (t_Apop / meritko));
			r[n] = r[n] - rust[n]; // zmensovani bunky

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
				meta.erase(meta.begin() + n);
				zvny.erase(zvny.begin() + n);

				pocet_A = pocet_A + 1;
				kolik = size(x);
			}

			break;

		case 0: // klid G0

			doba_zivota[n] += 1;
			trvani_cyklu[n]++;

			if (doba_zivota[n] > (200.0 / meritko)) // nove rozdelene bunky maji cas se rozdelit
			{
				// ne-/vratna faze G0 (podle poskozeni DNA)
				if ((tumor[n] == 0) && (poskozeni[n] > (prah_poskozeni[n] / 2.0)))
				{
					navrat[n] = 0;
				}
				if (poskozeni[n] > prah_poskozeni[n])
				{
					stav[n] = -1; // apoptoza
					trvani_cyklu[n] = 0;
				}
			}

			// stabilizace velikosti
			r[n] = r[n] - ((r[n] - poz_r[n]) / (100 / meritko));


			mark1 = 0;
			if ((dotyku[n] < (poc_dot-1)) && navrat[n]) // podle [26] zavisi na poctu dotyku(hustote prostredi)
			{
				trvani_cyklu[n] += 1; // jak dlouho jsou priznive podminky(= synteza proteinu)
				if (trvani_cyklu[n] > (t_cekani / meritko)) // cekani na syntezu proteinu
				{
					mark1 = 1;
				}
			}
			else
			{
				trvani_cyklu[n] = trvani_cyklu[n] - 1; // pokles na 0 v nepriznivych podminkach - degradace proteinu
				if (trvani_cyklu[n] < 0)
				{
					trvani_cyklu[n] = 0;
				}
			}


			if ((mark1 + int(prechod_G1) + tumor[n]) >= 2)
			{
				stav[n] = 1; // vstup do G1
				trvani_cyklu[n] = 0;
			}

			break;

		case 1: // faze G1

			doba_zivota[n] += 1; // doba zivota bunky
			trvani_cyklu[n] += 1; // doba ve fazi

			mark3 = 1;
			if (doba_zivota[n] > (200.0 / meritko)) // hned po deleni jsou bunky "imunni"
			{
				//if ((dotyku[n] >= poc_dot) && ((tumor[n] - supresory) != 1)) // maximalni pocet povolenych dotyku, pro tumor bez supresoru nema efekt
				//{
				//	mark3 = 0;
				//	stav[n] = 0; // prechod do G0
				//	trvani_cyklu[n] = 0;
				//	//navrat[n] = 0;
				//}
				//if ((poskozeni[n] > (prah_poskozeni[n] / 2.0)) && ((tumor[n] - supresory) != 1))
				//{
				//	mark3 = 0;
				//	stav[n] = 0; // senescentni G0
				//	trvani_cyklu[n] = 0;
				//	navrat[n] = 0;
				//}
				if ((dotyku[n] >= poc_dot) && (tumor[n] == 0)) // maximalni pocet povolenych dotyku, pro tumor bez supresoru nema efekt
				{
					mark3 = 0;
					stav[n] = 0; // prechod do G0
					trvani_cyklu[n] = 0;
					//navrat[n] = 0;
				}
				if ((poskozeni[n] > (prah_poskozeni[n] / 2.0)) && (tumor[n] == 0))
				{
					mark3 = 0;
					stav[n] = 0; // senescentni G0
					trvani_cyklu[n] = 0;
					navrat[n] = 0;
				}
				//if ((kolik_zivin == 0) && ((tumor[n] - supresory) != 1))
				//{
				//	mark3 = 0;
				//	stav[n] = -1; // apoptoza
				//	trvani_cyklu[n] = 0;
				//}
				//if ((kyslik == 0) && ((tumor[n] - supresory) != 1))
				//{
				//	mark3 = 0;
				//	stav[n] = -1; // apoptoza
				//	trvani_cyklu[n] = 0;
				//}
			}

			zrychleni = 1.0;
			if (tumor[n] == 1)
			{
				zrychleni = t_tumor; // nastavitelne zrychleni rustu tumoru
			}

			vaha = (1.0 - ((metabolismus[n] - 0.8))) * zrychleni; // pomalejsi metabolismus -> delsi cyklus

			if ((trvani_cyklu[n] > (t_G1 * vaha / meritko)) && (stav[n] == 1))
			{
				mark1 = 1;
				if (prechod_G1 == 0)
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
				else if (((nah_c(gen) / 1000000000.0) - poskozeni[n]) < (prah_apop[n] / meritko)) // kontrolni bod
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
				else
				{
					trvani_cyklu[n] = trvani_cyklu[n] - 1;
					if (trvani_cyklu[n] < 0)
					{
						trvani_cyklu[n] = 0;
					}
				}
			}

			//// tumor preskakuje G1
			//if (((preskok + tumor[n]) == 2) && (stav[n] == 1))
			//{
			//	stav[n] = 2; // vstup do bunecneho cyklu
			//	trvani_cyklu[n] = 0;
			//}

			break;

		case 2: // cyklus

			doba_zivota[n] += 1; // doba ve fazi

			zrychleni = 1.0;
			if (tumor[n] == 1)
			{
				zrychleni = t_tumor; // nastavitelne zrychleni rustu tumoru
			}

			vaha = (1.0 - ((metabolismus[n] - 0.8))) * zrychleni; // pomalejsi metabolismus -> delsi cyklus
			if (vaha > 1.5)
			{
				vaha = 1.5;
			}
			else if (vaha < 0.8)
			{
				vaha = 0.8;
			}

			delka_cyklu[n] = t_S + t_G2 + t_M; // delka cyklu
			delka_cyklu[n] = delka_cyklu[n] * vaha / meritko; // vahovani
			rust[n] = (poz_r[n] * pow(2.0, (1.0 / 3.0)) - poz_r[n]) / delka_cyklu[n]; // krok rustu

			if (trvani_cyklu[n] < delka_cyklu[n])
			{
				r[n] = r[n] + rust[n]; // rust bunky

				trvani_cyklu[n] = trvani_cyklu[n] + 1;
			}
			else if (trvani_cyklu[n] == delka_cyklu[n])
			{
				trvani_cyklu[n] = trvani_cyklu[n] + 1;

				if (((nah_c(gen) / 1000000000.0) - poskozeni[n]) < (2.0 * prah_apop[n] / meritko)) // 2x kontrolni bod
				{
					stav[n] = -1; // apoptoza
					trvani_cyklu[n] = 0;
				}
			}
			else // deleni
			{
				r[n] = r[n] / pow(2.0, (1.0 / 3.0)); // polovicni objem puvodni bunky

				// vytvoreni dcerinych bunek
				srand(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count()); // nahodna cisla

				kam_x = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				while (abs(kam_x) < 1.0)
				{
					kam_x = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				}
				kam_y = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				while (abs(kam_y) < 1.0)
				{
					kam_y = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				}
				kam_z = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				while (abs(kam_z) < 1.0)
				{
					kam_z = 5.0 * (((rand() % 1001) / 1000.0) - 0.5);
				}

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
				navrat.push_back(1);
				navrat.push_back(1);
				meta.push_back(meta[n] / 2.0);
				meta.push_back(meta[n] / 2.0);
				zvny.push_back(zvny[n] / 2.0);
				zvny.push_back(zvny[n] / 2.0);

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
				meta.erase(meta.begin() + n);
				zvny.erase(zvny.begin() + n);

			}


			break;

		default:
			cout << "chyba: bunka " << n << " ma stav " << stav_bb << endl;
			break;
		}

		prechod_G1 = 0;


		double kolikrat = (1 - (oprava * meritko));
		for (size_t i = 0; i < size(x); i++)
		{
			poskozeni[i] = poskozeni[i] * kolikrat;
		}
		//std::transform(poskozeni.begin(), poskozeni.end(), poskozeni.begin(), nasobeni_vec<double>(kolikrat)); // opravne mechanismy bunky

	}


	//snizovani koncentrace metabolitu (lymf. system atd.)
	//double kolikrat = (1 - snizovani);
	//std::transform(meta.begin(), meta.end(), meta.begin(), nasobeni_vec<double>(kolikrat));
	//std::transform(zvny.begin(), zvny.end(), zvny.begin(), nasobeni_vec<double>(kolikrat));


	// pohyb
	vector<double> vystup;
	for (size_t ko = 0; ko < size(x); ko++)
	{
		vystup = pohyb(meritko, ko);

		do_x = vystup[0];
		do_y = vystup[1];
		do_z = vystup[2];
		prekryti[ko] = vystup[3];
		dotyku[ko] = vystup[4];

		// omezeni rustu za hranici
		if (omezeni_r > 0)
		{
			if (sqrt(pow(do_x - posun_x, 2.0) + pow(do_z, 2.0)) > omezeni_r) // omezeni valec
			{
				do_x = x[ko];
				do_z = z[ko];
				dotyku[ko] += dotyku[ko];
			}
			//if (sqrt(pow(do_x - posun_x, 2.0) + pow(do_y - posun_y, 2.0) + pow(do_z, 2.0)) > omezeni_r) // omezeni koule
			//{
			//	do_x = x[ko];
			//	do_y = y[ko];
			//	do_z = z[ko];
			//	dotyku[ko] += dotyku[ko];
			//}
		}
		if (deska == 1)
		{
			if ((do_y - posun_y) > 0.0)
			{
				do_y = posun_y;
				dotyku[ko] += dotyku[ko];
			}
		}

		// posun bunek
		x[ko] = do_x;
		y[ko] = do_y;
		z[ko] = do_z;
	}


//std::vector<std::future<int>> futures;
//
//for (int i = 0; i < size(x); ++i)
//{
//	futures.push_back(std::async(bunky::pohyb, nast, i));
//}
//
//for (auto &e : futures)
//{
//	e.get();
//}


	// pocitani bunek
	int pocT = std::accumulate(tumor.begin(), tumor.end(), 0);
	int G0 = 0, G1 = 0, M = 0;
	for (size_t i = 0; i < size(x); i++)
	{
		if (stav[i] == 0)
		{
			G0++;
		}
		else if (stav[i] == 1)
		{
			G1++;
		}
		else if (stav[i] == 2)
		{
			M++;
		}
	}
	poctyB1.push_back(size(x) - pocT);
	poctyB2.push_back(pocT);
	poctyG0.push_back(G0);
	poctyG1.push_back(G1);
	poctyM.push_back(M);
}


vector<double> bunky::pohyb(double meritko, int n)
{

	kolik = size(x);

	touch = 0;
	prekryv = 0.0;
	okoli = 0.0;
	okoli2 = 0.0;
	en_x = 0.0; en_y = 0.0; en_z = 0.0;
	en_x2 = 0.0; en_y2 = 0.0; en_z2 = 0.0;
	vzd_xx = 0; vzd_yy = 0; vzd_zz = 0;
	en1 = 0; en2 = 0;
	vector<double> vzd(kolik);

#pragma loop(hint_parallel(4))
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
				//energie = meritko * (exp(r[n] + r[i] - vzdalenost), 5.0 / 2.0) * (1 / (5 * 2500)) * sqrt((r[n] + r[i]) / (r[n] + r[i])) / 100;
				//energie = meritko * -vzdalenost/100;
				//energie = meritko * pow(vzdalenost, 2.0) / 500;
				energie = meritko * (1.0 - exp(vzdalenost)) / 100.0;

				if (energie > 0.5)
				{
					energie = 0.5;
				}
				en_x = en_x + (vzd_xx * energie);
				en_y = en_y + (vzd_yy * energie);
				en_z = en_z + (vzd_zz * energie);
			}
			else if (vzdalenost <= r_bunek) // pohyb k blizkemu okoli
			{
				okoli2 += 1.0;

				//energie = -meritko * (12.57 * vzdalenost * vzdalenost) * pow(0.0053, (3.0 / 2.0)) * exp(-0.0167 * vzdalenost * vzdalenost) / 100.0; // funkce Maxwell-Boltzmann (simulace adheze)
				//energie = meritko * -(0.001 / exp(vzdalenost/10)); // vypocet energie posunu
				//energie = meritko * abs((exp(r[n] + r[i] - vzdalenost), 5.0 / 2.0) * (1.0 / (5.0 * 2500.0)) * sqrt((r[n] + r[i]) / (r[n] + r[i])) / 100.0); // komplexni cisla!!
				energie = meritko * (-1.0 / exp(vzdalenost / 2.0)) / 300.0;

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

	if ((en1 + (prekryv / 2.0)) >= en2) // prioritu ma rozdeleni bunek pred priblizenim, cim vic prekryti tim spis se bunky posunou od sebe
	{
		do_x = x[n] + ((en_x));
		do_y = y[n] + ((en_y));
		do_z = z[n] + ((en_z));
	}
	else
	{
		do_x = x[n] + ((en_x2));
		do_y = y[n] + ((en_y2));
		do_z = z[n] + ((en_z2));
	}

	vector<double> vystup;
	vystup.push_back(do_x);
	vystup.push_back(do_y);
	vystup.push_back(do_z);
	vystup.push_back(prekryv);
	vystup.push_back(touch);
	return vystup;
}


void bunky::ulozit()
{
	ifstream d("kolonie.dat");
	if (!d.good())
	{
		ofstream d("kolonie.dat");
		d.close();
	}
	else
	{
		ofstream d;
		d.open("kolonie.dat", std::ofstream::out | std::ofstream::trunc);
		d.close();
	}

	ofstream data("kolonie.dat");
	for (size_t i = 0; i < size(x); i++)
	{
			//		1						 2							 3				 4				 5					 6					 7					 8
			data << x[i] - posun_x << "	" << y[i] - posun_y << "	" << z[i] << "	" << r[i] << "	" << poz_r[i] << "	" << stav[i] << "	" << rust[i] << "	" << doba_zivota[i] << "	";
			//		9						 10							 11						 12						 13							 14
			data << delka_cyklu[i] << "	" << trvani_cyklu[i] << "	" << poskozeni[i] << "	" << prah_apop[i] << "	" << prah_ziviny[i] << "	" << prah_poskozeni[i] << "	";
			//		15						 16							 17					 18						 19					 20					 21					 22
			data << prah_deleni[i] << "	" << metabolismus[i] << "	" << dotyku[i] << "	" << prekryti[i] << "	" << tumor[i] << "	" << navrat[i] << "	" << meta[i] << "	" << zvny[i] << endl;
	}
	data.close();
}

void bunky::nacist()
{
	ifstream d("kolonie.dat");
	if (d.good())
	{
		x.clear();
		y.clear();
		z.clear();
		r.clear();
		poz_r.clear();
		stav.clear();
		rust.clear();
		doba_zivota.clear();
		delka_cyklu.clear();
		trvani_cyklu.clear();
		poskozeni.clear();
		prah_apop.clear();
		prah_ziviny.clear();
		prah_poskozeni.clear();
		prah_deleni.clear();
		metabolismus.clear();
		dotyku.clear();
		prekryti.clear();
		tumor.clear();
		navrat.clear();
		meta.clear();
		zvny.clear();

		string a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22;
		while (d >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7 >> a8 >> a9 >> a10 >> a11 >> a12 >> a13 >> a14 >> a15 >> a16 >> a17 >> a18 >> a19 >> a20 >> a21 >> a22) // kazde volani getline skoci na dalsi radek, na konci = 0
		{
			x.push_back(std::stod(a1) + posun_x);
			y.push_back(std::stod(a2) + posun_y);
			z.push_back(std::stod(a3));
			r.push_back(std::stod(a4));
			poz_r.push_back(std::stod(a5));
			stav.push_back(std::stod(a6));
			rust.push_back(std::stod(a7));
			doba_zivota.push_back(std::stod(a8));
			delka_cyklu.push_back(std::stod(a9));
			trvani_cyklu.push_back(std::stod(a10));
			poskozeni.push_back(std::stod(a11));
			prah_apop.push_back(std::stod(a12));
			prah_ziviny.push_back(std::stod(a13));
			prah_poskozeni.push_back(std::stod(a14));
			prah_deleni.push_back(std::stod(a15));
			metabolismus.push_back(std::stod(a16));
			dotyku.push_back(std::stod(a17));
			prekryti.push_back(std::stod(a18));
			tumor.push_back(std::stod(a19));
			navrat.push_back(std::stod(a20));
			meta.push_back(std::stod(a21));
			zvny.push_back(std::stod(a22));
		}
		d.close();

	}
}

void bunky::vymazani()
{
	for (size_t i = 0; i < size(x); i++)
	{
		if (((x[i] - posun_x) > 0) && ((y[i] - posun_y) > 0))
		{
			stav[i] = -1;
			trvani_cyklu[i] = 0;
		}
	}
}