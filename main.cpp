#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "bunky.h"
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>


using namespace std;


// rozliseni displeje
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

// nacteni aktulniho casu pro logovani
static char *aktualni_cas(char *delka) {
	time_t cas = time(0);
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
	strftime(delka, 30, "%d.%m.%Y %H:%M:%S", localtime(&cas));
	return delka;
}

int main(int argc, char* const argv[])
{
	//std::thread t[4];

	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical); // velikost obrazovky

	int iteraci;
	int pocet_iteraci = 0;

	bool tum = 0, pokracovat = 0;
	double omezeni_x = 200.0;
	double omezeni_z = 200.0;
	int vyber = 1;

	// // casy // //
	double meritko = 1.0;
	// casy z[1], v minutach
	double t_G1 = 660.0;
	double t_S = 480.0;
	double t_G2 = 240.0;
	double t_M = 60.0;
	double t_Apop = 180.0; // zdroj[30]
	double t_cekani = 50.0; // cekani v G0(najit zdroj!!)
							// (J.Biol.Chem. - 1996 - Loyer - 11484 - 92.pdf, http://www.nature.com/articles/srep04012, 
							// http ://mcb.asm.org/content/23/7/2351.abstract, http://www.nature.com/onc/journal/v19/n49/full/1203858a.html)

	// prepsani parametru casu z config.ini
	string prvni, druhy;
	vector<double> param;
	ifstream soubor("config.ini");
	if (soubor.is_open())
	{
		while (soubor >> prvni >> druhy) // kazde volani getline skoci na dalsi radek, na konci = 0
		{
			param.push_back(std::stod(druhy));
		}
		soubor.close();

		// zapis parametru do promennych
		int zf = 1; // zacatek nastaveni MAIN
		t_G1 = param[zf];
		t_S = param[zf + 1];
		t_G2 = param[zf + 2];
		t_M = param[zf + 3];
		t_Apop = param[zf + 4];
		t_cekani = param[zf + 5];
	}
	else
	{
		ofstream soubor("config.ini"); // vytvoreni konfigu kdyz neexistuje

		soubor << "=MAIN= 0" << endl;
		soubor << "t_G1 660" << endl;
		soubor << "t_S 480" << endl;
		soubor << "t_G2 240" << endl;
		soubor << "t_M 60" << endl;
		soubor << "t_Apop 180" << endl;
		soubor << "t_cekani 50" << endl;
		soubor << "=VYPOCTY= 1" << endl;
		soubor << "prostor 50" << endl;
		soubor << "=BUNKY= 2" << endl;
		soubor << "kolonie 100" << endl;
		soubor << "r_bunek 15" << endl;
		soubor << "preskok 0" << endl;
		soubor << "meze 50" << endl;
		soubor << "rozl 20" << endl;
		soubor << "poc_dot 4" << endl;
		soubor << "poc_dot2 8" << endl;
		soubor << "snizovani 0.1" << endl;
		soubor << "oprava 0.01" << endl;
		soubor << "metabolismus_0 0.05" << endl;
		soubor << "deska 0" << endl;
		soubor << "supresory 0" << endl;
		soubor << "poskozeni_tum 0.75";

		soubor.close();
	}
	// vytvoreni souboru logu pokud neexistuje
	ifstream logfile("log.txt");
	if (!logfile.good())
	{
		char cas[30];

		ofstream logfile("log.txt");
		logfile << aktualni_cas(cas) << " - Vytvoreni logu" << endl;
		logfile.close();
	}


	int state = 0;
	double pom, pom2;
	bool omezeni = 0;

// GUI
	system("cls"); // vymazani obsahu okna
	if ((argc != 2) && (argc != 7)) { // chybne zadani
		cout << "pouziti: " << argv[0] << " <pocet iteraci> <casove meritko> <omezeni x> <omezeni z> <prostorovy model> <tumor>" << endl;
		cout << "Napr. bunky 2000 2 150 150 1 y" << endl;
		cout << "pro napovedu: " << argv[0] << " -h" << endl;
		cout << "pro standardni nastaveni: " << argv[0] << " -s (5 000 iteraci, meritko 2.5, omezeni 200x200 um, model plochy, s tumorem)" << endl;
	}
	else if ((strcmp(argv[1], "h") == 0) || strcmp(argv[1], "-h") == 0) { // napoveda

		cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
		cout << "\nSilicoCell model\n\n";

		cout << "Pro spravne nastaveni simulace je potreba zadat 6 parametru v nasledujicim poradi:\n";
		cout << "	1. Pocet iteraci (jedna iterace odpovida jedne minute bunecneho cyklu)\n";
		cout << "	2. Casove meritko (uroven zrychleni vypoctu: meritko 5 -> 1 iterace = 5 minut)\n";
		cout << "		- optimalni rozsah 1 az 5\n";
		cout << "	3. omezeni osy x pro zmenseni zobrazovaneho objemu a snizeni vypocetnich naroku\n";
		cout << "		- optimalni rozsah 100 az 200\n";
		cout << "		- 0 pro vypnuti omezeni\n";
		cout << "	4. omezeni osy z pro zmenseni zobrazovaneho objemu a snizeni vypocetnich naroku\n";
		cout << "		- optimalni rozsah 100 az 200\n";
		cout << "		- 0 pro vypnuti omezeni\n";
		cout << "	5. prostorovy model rozlozeni latek\n";
		cout << "		- 0 pro zadny model (idealni podminky)\n";
		cout << "		- 1 pro plosny model\n";
		cout << "		- 2 pro model cevy\n";
		cout << "	6. simulace rustu tumoru (y/n)\n\n";

		cout << "Ovladani:\n";
		cout << "-Leve tlacitko mysi + pohyb mysi: otaceni zobrazeni\n";
		cout << "-Prave tlacitko mysi + posuv mysi: zobrazeni rezu objektem\n";
		cout << "-Klavesa Ctrl: prepnuti zobrazeni kolonie / jen tumor\n";
		cout << "-Klavesa Esc: ukonceni simulace\n\n\n";

		cout << "Stanislav Belehradek, 2017\n";
		cout << "xbeleh05@stud.feec.vutbr.cz\n";
		cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
	}
	else if ((strcmp(argv[1], "s") == 0) || strcmp(argv[1], "-s") == 0) { // standardni nastaveni
		iteraci = 5000;
		meritko = 2.5;
		omezeni = 1;
		omezeni_x = 200;
		omezeni_z = 200;
		vyber = 1;
		tum = 1;
		pokracovat = 1;
	}

////// testovani
//	pokracovat = 1;
//	meritko = 2;
//	iteraci = 2000;

	if (argc == 7 || pokracovat == 1)
	{
		if (!pokracovat)
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
			if (pom >= 1 && pom <= 10)
			{
				meritko = pom;
			}
			else
			{
				meritko = 1;
			}
			stringstream(argv[3]) >> pom;
			if (pom > 0)
			{
				omezeni_x = pom;
				omezeni = 1;
			}
			stringstream(argv[4]) >> pom2;
			if (pom2 > 0)
			{
				omezeni_z = pom2;
				omezeni = 1;
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
			else
			{
				vyber = 0;
			}
			if ((strcmp(argv[6], "y") == 0) || strcmp(argv[6], "-y") == 0)
			{
				tum = 1;
			}
		}

		bunky bunky;
		bunky.inicializace(meritko, tum);


		cout << "----------------------------------------------\n";
		if (omezeni == 1)
		{
			cout << "nastaveno meritko " << meritko << "x pro pocet iteraci " << iteraci << ". Modelovany objem omezen na " << omezeni_x << "x" << omezeni_z << " um." << endl;
		}
		else
		{
			cout << "nastaveno meritko " << meritko << "x pro pocet iteraci " << iteraci << ". Modelovany objem neni omezen." << endl;
		}
		if (tum == 1)
		{
			cout << "Simulace rustu tumoru\n";
		}
		cout << "----------------------------------------------\n";
		
		char cas[30];
		ofstream logfile("log.txt", std::ios_base::app | std::ios_base::out);
		logfile << aktualni_cas(cas) << " - Zahajeni simulace (meritko " << meritko << "; " << iteraci << " iteraci; omezeni " << omezeni_x << "x" << omezeni_z << "; tumor " << tum << ")" << endl;
		logfile.close();

		// vypocet a zobrazeni
		// zapnuti okna
		sf::ContextSettings con_set;
		con_set.antialiasingLevel = 4;
		con_set.depthBits = 24;
		con_set.stencilBits = 8;
		con_set.majorVersion = 3;
		con_set.minorVersion = 0;

		float screen_width = round(0.75 * horizontal); // skoro fullscreen
		float screen_height = round(0.75 * vertical);

		sf::RenderWindow window(sf::VideoMode(screen_width, screen_height, 32), "SilicoCell Model", sf::Style::Close, con_set);	// inicializace okna

		glClearDepth(1.f);
		glClearColor(1.f, 1.f, 1.f, 0.f);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glDepthMask(GL_TRUE);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, screen_width, screen_height, 0, -350, 350);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glScalef((screen_width / 1280), (screen_height / 720), 1.0f);

		window.setVerticalSyncEnabled(true);


		bool odejit = 0;
		int mxnew = 0, mynew = 0;
		int mxx = 0, myy = 0;
		int mxold = 0, myold = 0;
		float mznew = 0, mzz = 0, mzold = 0;
		bool zobrazeni = 0;

		auto cas_start = std::chrono::high_resolution_clock::now();

		while (window.isOpen() && !odejit) {
			sf::Event Event;
			while (window.pollEvent(Event)) {
				if (Event.type == sf::Event::Closed)
				{
					window.close();

					if (pocet_iteraci <= iteraci)
					{
						char cas[30];
						ofstream logfile("log.txt", std::ios_base::app | std::ios_base::out);
						logfile << aktualni_cas(cas) << " - Program ukoncen pri iteraci " << pocet_iteraci << ", pocet bunek: " << size(bunky.x) << endl;
						logfile.close();
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.close();

					if (pocet_iteraci <= iteraci)
					{
						char cas[30];
						ofstream logfile("log.txt", std::ios_base::app | std::ios_base::out);
						logfile << aktualni_cas(cas) << " - Program ukoncen pri iteraci " << pocet_iteraci << ", pocet bunek: " << size(bunky.x) << endl;
						logfile.close();
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
				{
					if (zobrazeni)
					{
						zobrazeni = 0;
					}
					else
					{
						zobrazeni = 1;
					}
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (Event.type == sf::Event::MouseButtonPressed)
					{
						state = 1;
						myy = sf::Mouse::getPosition(window).y;
						mxx = sf::Mouse::getPosition(window).x;
					}
					mxnew = mxold - mxx + sf::Mouse::getPosition(window).x;
					mynew = myold + myy - sf::Mouse::getPosition(window).y;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					if (Event.type == sf::Event::MouseButtonPressed)
					{
						state = 2;
						mzz = sf::Mouse::getPosition(window).y;
					}
					mznew = mzold - mzz + sf::Mouse::getPosition(window).y;
				}
				if (Event.type == sf::Event::MouseButtonReleased)
				{
					if (state == 1)
					{
						mxold = mxnew;
						myold = mynew;
					}
					else if (state == 2)
					{
						mzold = mznew;
					}
					state = 0;
				}
			}

			if (state > 0)
			{
				bunky.transform2(mxnew, mynew, mznew, screen_width, screen_height); // rotace podle polohy mysi
			}

			// vypocet cyklu
			if (pocet_iteraci < iteraci)
			{
				
				//bunky.pohyb(meritko, omezeni, omezeni_x, omezeni_z);

				bunky.bunky_cyklus(t_G1, t_S, t_G2, t_M, t_Apop, t_cekani, meritko, vyber, omezeni, omezeni_x, omezeni_z);

				pocet_iteraci += 1;
				cout << "\riterace: " << pocet_iteraci << " | pocet bunek: " << size(bunky.x);
			}
			else if (pocet_iteraci == iteraci)
			{
				cout << "\n\nKonec simulace." << endl; // ukonceni vypoctu

				auto cas_konec = std::chrono::high_resolution_clock::now();
				std::chrono::duration<float> cas_rozdil = cas_konec - cas_start;

				char cas[30];
				ofstream logfile("log.txt", std::ios_base::app | std::ios_base::out);
				logfile << aktualni_cas(cas) << " - Ukonceni simulace. Delka vypoctu: " << cas_rozdil.count() << " sekund, " << size(bunky.x) << " bunek" << endl;
				logfile.close();

				pocet_iteraci += 1;
			}

			// vykresleni
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_POINT_SMOOTH);

			for (size_t i = 0; i < size(bunky.x); i++)
			{
				glPointSize((2.f * bunky.r[i]));
				glBegin(GL_POINTS);

				if (bunky.tumor[i] == 1) {
					if (bunky.stav[i] == 0)
					{
						glColor3f(0.5, 0.5, 0.7);
					}
					else if (bunky.stav[i] == 1)
					{
						glColor3f(0.3, 0.4, 0.8);
					}
					else if (bunky.stav[i] == 2)
					{
						glColor3f(0.2, 0.3, 0.8);
					}
				}
				else
				{
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
				}

				if (!(bunky.tumor[i] == 0 && zobrazeni == 1))
				{
					glVertex3f(bunky.x[i] + (screen_width / 2), bunky.y[i] + (screen_height / 2), bunky.z[i]);
				}
				
				glEnd();

			}

			window.display(); // zobrazeni
			//sf::sleep(sf::milliseconds(1));
		}
	}

}





