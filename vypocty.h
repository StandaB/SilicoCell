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
	double ziviny(double xx, double yy, double zz, int vyber, float prostor);
	double kyslik(double xx, double yy, double zz, int vyber, float prostor);
	double toxiny(double xx, double yy, double zz, int vyber, float prostor);
	double RF(double xx, double yy, double zz, int vyber, float prostor);
	double vysledek;


private:

};




#endif //VYPOCTY_H