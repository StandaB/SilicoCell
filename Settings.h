#ifndef SETTINGS_H
#define SETTINGS_H


#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "bunky.h"
#include "wtypes.h"

using namespace std;


class Settings {
public:
	Settings(int w, int h);
	void onResize(int width, int height);
	void turnOnLights();
	void turnOffLights();

	bunky bunky;

	const sf::ContextSettings &getWindowSet() const;

private:
	void inicialise(int width, int height);
	GLfloat materialAmbient[4] = { 0.8f, 0.5f, 0.5f, 0.0f }; // uprava vlastnosti osvetleni
	GLfloat materialDiffuse[4] = { 0.8f, 0.4f, 0.4f, 0.0f };
	GLfloat materialShine[4] = { 0.8f, 0.4f, 0.4f, 0.0f };
	GLfloat materialShineFactor[1] = { 50.0f };
	GLfloat poziceSvetla[4] = { 1.0f, 0.8f, 0.5f, 0.0f };
	sf::ContextSettings windowSet;
};

#endif //SETTINGS_H