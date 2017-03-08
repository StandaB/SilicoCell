#include "stdafx.h"
#include "Mouse.h"


void Mouse::onLeftButton(int x, int y)
{
	xnew = xold - xx + x; // pokracovani v pohybu mysi ze starych souradnic
	ynew = yold - yy + y;
}

void Mouse::onRightButton(int x, float y)
{
	//znew = abs(zold - (zz / 200) + (y/200)); // vypocet zvetseni
	//if (znew < 0.5) { znew = 0.5; }; // zamezeni prilis maleho zvetseni
	znew = zold - zz + y;
}
