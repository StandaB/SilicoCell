#include "stdafx.h"
#include "Settings.h"


Settings::Settings(int w, int h) {

	//int vel = 1; // zvetseni

	//	glClearColor(0.0, 0.0, 0.0, 0.0);
	//	glClearDepth(1.0);
	//	glViewport(0, 0, w, h);
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadIdentity();
	//	glOrtho(-10, 150, -10, 150, -100, 150);
	//	glScalef(vel*1.0f, vel*1.25f, vel*1.0f);
	//	glEnable(GL_DEPTH_TEST);
	//	glDepthFunc(GL_LESS);
	//	glDepthRange(-10, 10);
	//	glShadeModel(GL_SMOOTH);
	//	glPolygonMode(GL_FRONT, GL_FILL);
	//	glPolygonMode(GL_BACK, GL_FILL);
	//	glDisable(GL_CULL_FACE);
	//	turnOnLights();
	inicialise(w,h);
	}

void Settings::onResize(int width, int height) {
	//int vel = 2; // zvetseni
	inicialise(width, height);
	//glScalef(vel*1.0f, vel*1.25f, vel*1.0f);
}

void Settings::inicialise(int width, int height) {
	//glViewport(0, 0, width, height); // inicializace okna, rozmery, nasvetleni objektu atd ze cviceni
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	////glOrtho(-100, 200, -100, 200, -300, 300);
	//glOrtho(0, 800, 0, 600, -500, 500);
	//glScalef(1.0f, 1.0f, 1.0f);
	//glClearColor(1.0, 1.0, 1.0, 0.0);
	//glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	////glDepthFunc(GL_LESS);
	////glShadeModel(GL_SMOOTH);
	//glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_BACK, GL_FILL);
	////glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	////glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	////glMaterialfv(GL_FRONT, GL_SPECULAR, materialShine);
	////glMaterialfv(GL_FRONT, GL_SHININESS, materialShineFactor);
	////glLightfv(GL_LIGHT0, GL_POSITION, poziceSvetla);
	////turnOnLights(); // zapnuti svetel
	//turnOffLights();


	glClearDepth(1.f);
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -500, 500);
	glScalef(1.0f, 1.0f, 1.0f);
}

void Settings::turnOnLights() {

	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);
}

void Settings::turnOffLights() {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

const sf::ContextSettings &Settings::getWindowSet() const {
	return windowSet;
}