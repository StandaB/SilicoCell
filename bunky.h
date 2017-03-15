#ifndef BUNKY_H
#define BUNKY_H

#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <vector>
#include "vypocty.h"
//#include <thread>

using namespace std;



class bunky {

public:
	//void bunky_cyklus(int od_x, int kam_x);
	void bunky_cyklus(double t_G1, double t_S, double t_G2, double t_M, double t_Apop, double t_cekani, double meritko, int vyber);
	void inicializace(double meritko, bool tum);
	void transform2(int poz_x, int poz_y, int poz_z);
	void pohyb(double meritko, bool omezeni, double omezeni_x, double omezeni_z);
	void zmena_rozliseni(int pos_x, int pos_y);

	int posun_x = 512;
	int posun_y = 384 - 100;

	vector<double> x;
	vector<double> y;
	vector<double> z;
	vector<double> r;
	vector<int> stav;
	int pocet_A = 0;

	vector<bool> tumor;

	double* ECM_x = new double[200*200*200]; // velikost ECM pole
	double* ECM_y = new double[200*200*200];
	double* ECM_z = new double[200*200*200];

	double* meta = new double[200*200*200]; // velikost pole metabolitu
	
private:
	vypocty vypocty;

	double kolik_zivin;
	double kolik_RF;
	double kolik_toxinu;
	double kolik_mitogenu;

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
	vector<double> delka_cyklu;
	vector<double> trvani_cyklu;
	vector<double> prah_ziviny;
	vector<double> prah_poskozeni;
	vector<double> prah_deleni;
	vector<double> metabolismus;

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

	double max_vzd = 100; // vzdalenost od stredu - pozice > max_vzd -> prechazi do G0 = omezeni oblasti vypoctu
	int poc_dot = 4; // maximalni pocet dotyku pro vstup do G1



	int mark1, mark2, mark3, mark4;
	
};

#endif // BUNKY_H