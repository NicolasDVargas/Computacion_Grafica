
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "simpleobjloader.h"

using namespace std;
//PUNTOS//

void Puntos::setX(float x) {
	this->x = x;
}

void Puntos::setY(float y) {
	this->y = y;
}

void Puntos::setZ(float z) {
	this->z = z;
}

float Puntos::getX() {
	return this->x;
}

float Puntos::getY() {
	return this->y;
}

float Puntos::getZ() {
	return this->z;
}

Puntos::Puntos(){}

Puntos::Puntos(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
//OBJLOADER//

void OBJLoader::cargaCara(char *linea) {
	vector<Split> tokens = split (linea, " ");
	for ( int i = 0; i < 3 && tokens.size () > i; i++ ) {
		vector<Split> indice = split (tokens[ i + 1 ].split, "/");
		this->indices.push_back (atoi (indice[ 0 ].split) - 1);
		this->indicestext.push_back (atoi (indice[ 1 ].split) - 1);
		this->indicesnormales.push_back (atoi (indice[ 2 ].split) - 1);
	}
}

void OBJLoader::cargaPuntos(char *linea) {
	vector<Split> tokens = split (linea, " ");
	float x, y, z;
	x = atof (tokens[ 1 ].split);
	y = atof (tokens[ 2 ].split);
	if ( tokens.size () > 3 )
		z = atof (tokens[ 3 ].split);
	else
		z = 0.0;
	Puntos punto (x, y, z);
	this->vertices.push_back (punto);
}

void OBJLoader::cargaNormales(char *linea) {
	vector<Split> tokens = split (linea, " ");
	float x, y, z;
	x = atof (tokens[ 1 ].split);
	y = atof (tokens[ 2 ].split);
	if ( tokens.size () > 3 )
		z = atof (tokens[ 3 ].split);
	else
		z = 0.0;
	Puntos punto (x, y, z);
	this->normales.push_back (punto);
}

void OBJLoader::cargaText(char *linea) {
	vector<Split> tokens = split (linea, " ");
	float x, y, z;
	x = atof (tokens[ 1 ].split);
	y = atof (tokens[ 2 ].split);
	if ( tokens.size () > 3 )
		z = atof (tokens[ 3 ].split);
	else
		z = 0.0;
	Puntos punto (x, y, z);
	this->texturas.push_back (punto);
}

void OBJLoader::cargarOBJ(const char *archivo) {
	char linea[100];
	fstream arch (archivo, ios::in);
	while ( arch.getline (linea, 100)) {
		if ( strstr (linea, "v ") != NULL ) {
			cargaPuntos (linea);
		} else if ( strstr (linea, "vt ") != NULL ) {
			cargaText (linea);
		} else if ( strstr (linea, "f ")) {
			cargaCara (linea);
		} else if ( strstr (linea, "vn ")) {
			cargaNormales (linea);
		}
	}
	arch.close ();
}

OBJLoader::OBJLoader() {
}

vector<Puntos> OBJLoader::getVertices() {
	return this->vertices;
}

vector<int> OBJLoader::getIndices() {
	return this->indices;
}

vector<Puntos> OBJLoader::getTexturas() {
	return this->texturas;
}

vector<int> OBJLoader::getIndicesText() {
	return this->indicestext;
}

vector<Puntos> OBJLoader::getNormales() {
	return this->normales;
}

vector<int> OBJLoader::getIndicesNormales() {
	return this->indicesnormales;
}

//OTRAS//

vector<Split> split(char *ln, const char *delim) {
	Split dato;
	char linea[100];
	char *token;
	vector<Split> vect;
	strcpy (linea, ln);
	token = strtok (linea, delim);
	strcpy (dato.split, token);
	vect.push_back (dato);
	while ((token = strtok (NULL, delim)) != NULL ) {
		strcpy (dato.split, token);
		vect.push_back (dato);
	}
	return vect;
}
//eof

