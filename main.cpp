#include "stdafx.h"
#include "Settings.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "Mouse.h"
#include "bunky.h"
#include <string>
#include <string.h>
#include <sstream>

using namespace std;


void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

int main(int argc, char* const argv[])
{
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical); // velikost obrazovky

	int iteraci;
	int pocet_iteraci = 0;

	bool omezeni = 0;
	double omezeni_x = 200.0;
	double omezeni_z = 200.0;
	int vyber = 1;

	////////////////////////////////////////// nastaveni casu
	double meritko = 1.0;
	// casy z[1], v minutach
	double t_G1 = 660.0;
	double t_S = 480.0;
	double t_G2 = 240.0;
	double t_M = 60.0;
	double t_Apop = 180.0; // zdroj[30]
	double t_regulace_velikosti = 1440.0; // za 24 hodin se zmeni polomer o 1 um(najit zdroj!!)
	double t_cekani = 50.0; // cekani v G0(najit zdroj!!)
									  // (J.Biol.Chem. - 1996 - Loyer - 11484 - 92.pdf, http://www.nature.com/articles/srep04012, 
									  // http ://mcb.asm.org/content/23/7/2351.abstract, http://www.nature.com/onc/journal/v19/n49/full/1203858a.html)


	int state = 0;
	Mouse mys;


	double pom, pom2;

	// GUI
	if ((argc != 2) && (argc != 6)) { // chybne zadani
		cout << "pouziti: " << argv[0] << " <pocet iteraci> <casove meritko> <omezeni x> <omezeni z> <prostorovy model>" << endl;
	}
	else if ((strcmp(argv[1], "h") == 0) || strcmp(argv[1], "-h") == 0) { // napoveda
		cout << "SilicoCell model 2017" << endl;



	}
	else if (argc == 6)
	{
		stringstream(argv[1]) >> pom;
		if (pom >= 1 && pom <= 100000)
		{
			iteraci = pom;
		}
		else
		{
			iteraci = 1000;
		}
		stringstream(argv[2]) >> pom;
		if (pom >= 0 && pom <= 5)
		{
			meritko = pom;
		}
		else
		{
			meritko = 1;
		}
		stringstream(argv[3]) >> pom;
		stringstream(argv[4]) >> pom2;
		if ((pom > 0) && (pom2 > 0))
		{
			omezeni = 1;
			omezeni_x = pom;
			omezeni_z = pom2;
		}
		stringstream(argv[5]) >> pom;
		if (pom == 1)
		{
			vyber = 1;
		}
		else if (pom == 2)
		{
			vyber = 2;
		}

		bunky bunky;
		bunky.inicializace();

		cout << "----------------------------------------------\n";
		cout << "nastaveno meritko " << meritko << "x pro pocet iteraci " << iteraci << ". Modelovany objem omezen na " << omezeni_x << "x" << omezeni_z << " um." << endl;
		cout << "----------------------------------------------\n";

		// vypocet a zobrazeni
		// zapnuti okna
		sf::ContextSettings context;
		context.antialiasingLevel = 4;
		context.depthBits = 32;
		context.stencilBits = 8;


		sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "SilicoCell Model", sf::Style::Close | sf::Style::Resize, context);	// inicializace okna
		Settings(1024, 768);

		window.setVerticalSyncEnabled(true);

		bool odejit = 0;
		while (window.isOpen() && !odejit) {
			sf::Event Event;
			while (window.pollEvent(Event)) {
				if (Event.type == sf::Event::Closed)
				{
					window.close();
				}
				//if (Event.type == sf::Event::Resized)
				//{
				//	Settings onResize(window.getSize().x, window.getSize().y);
				//}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.close();
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (Event.type == sf::Event::MouseButtonPressed)
					{
						state = 1;
						mys.yy = sf::Mouse::getPosition(window).y;
						mys.xx = sf::Mouse::getPosition(window).x; // ziskani pozice mysi pro otaceni
					}
					mys.onLeftButton(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					if (Event.type == sf::Event::MouseButtonPressed)
					{
						state = 2;
						mys.zz = sf::Mouse::getPosition(window).y; // ziskani pozice mysi pro posun v ose z (zvetseni)
					}
					mys.onRightButton(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				}
				if (Event.type == sf::Event::MouseButtonReleased)
				{
					if (state == 1)
					{
						mys.xold = mys.xnew; // ulozeni hodnot pozice mysi
						mys.yold = mys.ynew;
					}
					else if (state == 2)
					{
						mys.zold = mys.znew;
					}
					state = 0;
				}
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			bunky.transform2(mys.xnew, mys.ynew, mys.znew); // rotace podle polohy mysi


															// vypocet cyklu
			if (pocet_iteraci < iteraci)
			{
				bunky.bunky_cyklus(t_G1, t_S, t_G2, t_M, t_Apop, t_cekani, meritko, vyber);

				bunky.pohyb(meritko, omezeni, omezeni_x, omezeni_z);

				pocet_iteraci += 1;
				cout << "\riterace: " << pocet_iteraci << " | pocet bunek: " << size(bunky.x);
			}
			else if (pocet_iteraci == iteraci)
			{
				cout << "\n\nkonec" << endl; // ukonceni vypoctu
											 //cout << "\nkonecny pocet bunek: " << size(bunky.x) << endl;
				cout << "pocet apoptoz: " << bunky.pocet_A << endl;
				pocet_iteraci += 1;
			}
			//else
			//{
			//	cout << "vypnout program? A/N" << endl;
			//	getline(cin, text);
			//	if (text == "A" || text == "a")
			//	{
			//		odejit = 1;
			//	}
			//}

			//if ((pocet_iteraci % 500) == 0)
			//{
			//	cout << pocet_iteraci << ": " << size(bunky.x) << endl;
			//	//cout << bunky.prekryti[1] << endl;
			//	//cout << bunky.dotyku[1] << endl;
			//	//cout << bunky.r[1] << endl;
			//}


			// vykresleni
			glEnable(GL_POINT_SMOOTH);
			//glPointSize(5); // bodova mrizka
			//glColor3f(0.0, 0.0, 0.0);
			//glBegin(GL_POINTS);
			//glVertex3f(0.0, 0.0, 0.0);
			//glVertex3f(30.0, 30.0, 30.0);
			//glVertex3f(30.0, 0.0, 0.0);
			//glVertex3f(0.0, 30.0, 0.0);
			//glVertex3f(0.0, 0.0, 30.0);
			//glVertex3f(0.0, 30.0, 30.0);
			//glVertex3f(30.0, 30.0, 0.0);
			//glVertex3f(30.0, 0.0, 30.0);
			//glEnd();
			//glPointSize(30);
			//glBegin(GL_POINTS);
			//glVertex3f(15.0, 15.0, 15.0);
			//glEnd();
			//sf::CircleShape shape(30);
			//shape.setPosition(100, 100);
			//shape.setFillColor(sf::Color(100, 250, 50));
			//App.draw(shape);


			//window.clear();
			for (size_t i = 0; i < size(bunky.x); i++)
			{
				glPointSize((2.f * bunky.r[i]));
				glBegin(GL_POINTS);

				if (bunky.stav[i] == -1)
				{
					glColor3f(0.5, 0.0, 0.0);
				}
				else if (bunky.stav[i] == 0)
				{
					glColor3f(0.5, 0.5, 0.5);
				}
				else if (bunky.stav[i] == 1)
				{
					glColor3f(0.7, 0.7, 0.0);
				}
				else if (bunky.stav[i] == 2)
				{
					glColor3f(0.3, 0.8, 0.1);
				}
				else
				{
					glColor3f(0.0, 0.0, 0.0);
				}

				glVertex3f(bunky.x[i], bunky.y[i], bunky.z[i]);
				glEnd();

			}

			window.display(); // zobrazeni
		}
	}

}





