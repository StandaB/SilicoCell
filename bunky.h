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
#include <future>
#include <algorithm>
#include <functional>
#include <numeric>

using namespace std;



class bunky {

public:
	void bunky_cyklus(double nastaveni[]);
	void inicializace(double meritko, bool tum);
	void transform2(int poz_x, int poz_y, int poz_z, float screen_width, float screen_height);
	vector<double> pohyb(double meritko, int n);
	void ulozit();
	void nacist();


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

	//double* ECM_x = new double[200*200*200]; // velikost ECM pole
	//double* ECM_y = new double[200*200*200];
	//double* ECM_z = new double[200*200*200];

	int meze = 125;
	int rozl = 20;
	////double* RuFa = new double[meze * meze * meze]; // velikost pole rustovych faktoru pro bunky generujici RF
	vector<double> meta; // pole metabolitu
	vector<double> zvny; // pole zivin (odecet od kolik_zivin podle metabolismu bunek)

	vector<double> prah_ziviny;
	vector<double> prah_deleni;
	vector<double> delka_cyklu;
	vector<double> metabolismus;

	vector<int> poctyB1;
	vector<int> poctyB2;
	
private:
	vypocty vypocty;

	double kolik_zivin;
	double kyslik;
	double kolik_RF;
	double kolik_toxinu;

	bool preskok = 0;

	// parametry kolonie
	int kolonie = 100;
	double r_bunek = 15;
	vector<double> poz_r;
	int stav_bb;
	vector<int> doba_zivota;
	vector<double> poskozeni;
	double oprava;
	vector<double> prah_apop;
	vector<double> prah_RF;
	vector<int> dotyku;
	vector<double> prekryti;

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

	//double max_vzd = 100; // vzdalenost od stredu - pozice > max_vzd -> prechazi do G0 = omezeni oblasti vypoctu
	int poc_dot = 5; // maximalni pocet dotyku pro vstup do G1
	int poc_dot2 = 8;

	double snizovani = 0.9;
	double metabolismus_0 = 0.05;
	bool deska = 0;
	bool supresory = 0;
	double poskozeni_tum = 1;
	double vaha = 1;

	int mark1, mark2, mark3, mark4;
	
};

#endif // BUNKY_H