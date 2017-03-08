#ifndef MOUSE_H
#define MOUSE_H

#include "stdafx.h"
#include <cmath>
#include <iostream>
using namespace std;

class Mouse
{
public:

	int xnew = 0, ynew = 0;
	int xx = 0, yy = 0;
	int xold = 0, yold = 0;
	float znew = 0, zz = 0, zold = 0;

	void onLeftButton(int x, int y);
	void onRightButton(int x, float y);


};

#endif //MOUSE_H