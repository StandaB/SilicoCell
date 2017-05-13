#ifndef BUNKY_H
#define BUNKY_H

#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <vector>
#include "vypocty.h"
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <random>

using namespace std;

class bunky {

public:
	void bunky_cyklus(double nastaveni[]);
	void inicializace(double meritko);
	void ini2();
	void transform2(int poz_x, int poz_y, int poz_z, float screen_width, float screen_height);
	vector<double> pohyb(double meritko, int n);
	void ulozit();
	void nacist();
	void vymazani();


	int posun_x = 0;
	int posun_y = 50;

	vector<double> x;
	vector<double> y;
	vector<double> z;
	vector<double> r;
	vector<int> stav;
	int pocet_A = 0;
	double prostor = 100;

	vector<int> tumor;

	vector<double> meta; // tvorba metabolitu
	vector<double> zvny; // spotreba zivin (odecet od kolik_zivin podle metabolismu bunek)

	vector<double> prah_ziviny;
	vector<double> prah_deleni;
	vector<double> delka_cyklu;
	vector<double> metabolismus;

	vector<int> poctyB1;
	vector<int> poctyB2;
	vector<int> poctyG0;
	vector<int> poctyG1;
	vector<int> poctyM;

	bool supresory = 0;
	
private:
	vypocty vypocty;

	double kolik_zivin;
	double kyslik;
	double kolik_RF;
	double kolik_toxinu;

	double t_tumor = 1.0;

	// parametry kolonie
	int kolonie = 100;
	double r_bunek = 15.0;
	vector<double> poz_r;
	int stav_bb;
	vector<int> doba_zivota;
	vector<double> poskozeni;
	double oprava;
	double poskozeni_b;
	double prah_ap;
	vector<double> prah_apop;
	vector<double> prah_RF;
	vector<int> dotyku;
	vector<double> prekryti;
	double pokles;

	vector<double> rust;
	vector<double> trvani_cyklu;
	vector<double> prah_poskozeni;
	vector<bool> navrat;

	double prumer_x;
	double prumer_y;
	double prumer_z;

	double kam_x;
	double kam_y;
	double kam_z;

	double vzd_xx;
	double vzd_yy;
	double vzd_zz;

	double vzdalenost;
	int touch;
	double prekryv;
	double okoli, okoli2;
	double energie;
	double en_x, en_x2;
	double en_y, en_y2;
	double en_z, en_z2;
	double en1, en2;
	double do_x;
	double do_y;
	double do_z;

	int kolik;

	int poc_dot = 5; // maximalni pocet dotyku pro vstup do G1

	double snizovani = 0.9;
	bool deska = 0;
	double vaha = 1.0;
	double zrychleni = 1.0;

	int mark1, mark2, mark3, mark4;
	
};

#endif // BUNKY_H