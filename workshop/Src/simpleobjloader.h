
#ifndef _SOBJLOADER_H
#define    _SOBJLOADER_H

#include <vector>

using namespace std;

class Puntos {
private:
	float x;
	float y;
	float z;
public:
	void setX(float x);

	void setY(float y);

	void setZ(float z);

	float getX();

	float getY();

	float getZ();

	Puntos(float x, float y, float z);

	Puntos();
};

class OBJLoader {
private:
	vector<Puntos> vertices;
	vector<Puntos> texturas;
	vector<Puntos> normales;
	vector<int> indices;
	vector<int> indicestext;
	vector<int> indicesnormales;

	void cargaCara(char *linea);

	void cargaPuntos(char *linea);

	void cargaText(char *linea);

	void cargaNormales(char *linea);

public:
	void cargarOBJ(const char *archivo);

	OBJLoader();

	vector<Puntos> getVertices();

	vector<int> getIndices();

	vector<Puntos> getTexturas();

	vector<int> getIndicesText();

	vector<Puntos> getNormales();

	vector<int> getIndicesNormales();
};

struct Split {
	char split[100];
};

vector<Split> split(char *linea, const char *delim);

#endif