#include "stdafx.h"
#include "vypocty.h"
#include "bunky.h"

bunky b;


double vypocty::ziviny(double xx, double yy, double zz, int vyber, float prostor)
{
	if (vyber == 1)
	{
		vysledek = abs(pow((1.0 / 2.0), (abs(yy - b.posun_y) / prostor)) - 0); // -0.xx kdyz u zdroje neni 100% zivin
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / prostor));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}
	else if (vyber == 3)
	{
		vysledek = exp(-(sqrt( pow((xx - b.posun_x), 2.0) + pow((yy - b.posun_y), 2.0) + pow(zz, 2.0))) / prostor);
	}
	
	if (vysledek < 0)
	{
		vysledek = 0;
	}
	else if (vysledek > 1)
	{
		vysledek = 1;
	}

	return(vysledek);
}

double vypocty::kyslik(double xx, double yy, double zz, int vyber, float prostor)
{
	if (vyber == 1)
	{
		vysledek = abs(pow((1.0 / 2.0), (abs(yy - b.posun_y) / prostor)) - 0);
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / prostor));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}
	else if (vyber == 3)
	{
		vysledek = exp(-(sqrt(pow((xx - b.posun_x), 2.0) + pow((yy - b.posun_y), 2.0) + pow(zz, 2.0))) / prostor);
	}

	if (vysledek < 0)
	{
		vysledek = 0;
	}
	else if (vysledek > 1)
	{
		vysledek = 1;
	}

	return(vysledek);
}

double vypocty::toxiny(double xx, double yy, double zz, int vyber, float prostor)
{
	//if (vyber == 1)
	//{
	//	vysledek = 1 - pow((1.0 / 2.0), (abs(yy - b.posun_y) / prostor));
	//}
	//else if (vyber == 2)
	//{
	//	double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
	//	double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
	//	vysledek = 1 - exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / prostor));
	//}
	//else if (vyber == 0)
	//{
	//	vysledek = 0;
	//}
	//else if (vyber == 3)
	//{
	//	vysledek = exp(-(sqrt(pow((xx - b.posun_x), 2.0) + pow((yy - b.posun_y), 2.0) + pow(zz, 2.0))) / prostor);
	//}
	//
	//if (vysledek < 0)
	//{
	//	vysledek = 0;
	//}
	//else if (vysledek > 1)
	//{
	//	vysledek = 1;
	//}

	vysledek = 0; // bez toxinu

	return(vysledek);
}

double vypocty::RF(double xx, double yy, double zz, int vyber, float prostor)
{
	if (vyber == 1)
	{
		vysledek = abs(pow((1.0 / 2.0), (abs(yy - b.posun_y) / prostor)) - 0);
	}
	else if (vyber == 2)
	{
		double param_x = (20.0 * sin(zz / 40.0)) + b.posun_x - xx;
		double param_y = (40.0 * cos(zz / 50.0)) + b.posun_y - yy;
		vysledek = exp(-(sqrt(pow(param_x, 2.0) + pow(param_y, 2.0)) / prostor));
	}
	else if (vyber == 0)
	{
		vysledek = 1;
	}
	else if (vyber == 3)
	{
		vysledek = exp(-(sqrt(pow((xx - b.posun_x), 2.0) + pow((yy - b.posun_y), 2.0) + pow(zz, 2.0))) / prostor);
	}

	if (vysledek < 0)
	{
		vysledek = 0;
	}
	else if (vysledek > 1)
	{
		vysledek = 1;
	}

	return(vysledek);
}


