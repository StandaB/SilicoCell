#ifndef VYPOCTY_H
#define VYPOCTY_H

#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class vypocty {
public:
	double ziviny(double xx, double yy, double zz, int vyber);
	double kyslik(double xx, double yy, double zz, int vyber);
	double toxiny(double xx, double yy, double zz, int vyber);
	//double mitogeny(double xx, double yy, double zz, int vyber);
	double RF(double xx, double yy, double zz, int vyber);
	double vysledek;


private:

};




#endif //VYPOCTY_H