#include "stdafx.h"
#include "vypocty.h"
#include "bunky.h"

bunky b;


double vypocty::ziviny(double xx, double yy, double zz, int vyber)
{
	if (vyber == 1)
	{
		//vysledek = pow((1.0 / 2.0), ((yy - b.posun_y) / 100.0)); // bez omezeni jsou vysledky pod osou > 1
		vysledek = pow((1.0 / 2.0), (abs(yy - b.posun_y) / 1000.0)); // gradient (normalne "/ 1000.0", 10.0 pro zvyrazneni efektu)
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / 200.0));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}
	

	return(vysledek);
}

double vypocty::toxiny(double xx, double yy, double zz, int vyber)
{
	if (vyber == 1)
	{
		//vysledek = 1 - pow((1.0 / 2.0), ((yy - b.posun_y) / 100.0));
		vysledek = 1 - pow((1.0 / 2.0), (abs(yy - b.posun_y) / 1000.0));
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = 1 - exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / 200.0));
	}
	else if (vyber == 0)
	{
		vysledek = 0;
	}

	return(vysledek);
}

double vypocty::mitogeny(double xx, double yy, double zz, int vyber)
{
	if (vyber == 1)
	{
		//vysledek = pow((1.0 / 2.0), ((yy - b.posun_y) / 100.0));
		vysledek = pow((1.0 / 2.0), (abs(yy - b.posun_y) / 1000.0));
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / 200.0));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}

	return(vysledek);
}

double vypocty::RF(double xx, double yy, double zz, int vyber)
{
	if (vyber == 1)
	{
		//vysledek = pow((1.0 / 2.0), ((yy - b.posun_y) / 100.0));
		vysledek = pow((1.0 / 2.0), (abs(yy - b.posun_y) / 1000.0));
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / 200.0));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}

	return(vysledek);
}

