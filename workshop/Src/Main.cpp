#include <limits>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "simpleobjloader.h"
#include <iostream>

#include "RgbImage.h"

GLuint texture[9];
bool ShowWrapper = false;
void DrawWrapper(float);

#define vivo 1
#define muriendo 0
#define muerto -1

float Noemit[4] = {0.9, 0.1, 0.1, 2.0};
int numMonsters = 0;
vector<int> disparando;

bool autoRotate = true;

int RunMode = 1;

float CurrentAngle = 0.0f; // Angle in degrees
float AnimateStep = 0.005f;

// Storage For One Texture ( NEW )

GLfloat qaWhiteAM[] = {	1.0, 1.0, 1.0, 1.0}; // White Ambient Material Color
GLfloat qaWhiteDF[] = {	1.0, 1.0, 1.0, 1.0}; // White Diffuse Material Color
GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0}; // White Color

GLfloat NormalX = 0;
GLfloat NormalY = 0;
GLfloat NormalZ = 0;

GLfloat qaAmbientLight[] = {1, .8, .8, 1.0}; 
GLfloat qaDiffuseLight[] = {.8, 1, .8, 1.0}; 
GLfloat qaSpecularLight[] = {.8, .8, 1, 1.0}; 


GLfloat qaLightPosition[] = {10, 10, 10, 1};

class Bala
{

public:
	Puntos origen;
	Puntos destino;
	Puntos actual;
	float offset;

	Bala() {}

	Bala(float origenX, float origenY, float origenZ, float destinoZ)
	{
		origen.setX(origenX);
		origen.setY(origenY);
		origen.setZ(origenZ);
		destino.setX(origenX);
		destino.setY(origenY);
		destino.setZ(destinoZ);
		actual.setX(origenX);
		actual.setY(origenY);
		actual.setZ(origenZ);
	}

	void bulletPosition()
	{
		float distance = sqrt(pow(this->origen.getX() - this->destino.getX(), 2) + pow(this->origen.getY() - this->destino.getY(), 2) + pow(this->origen.getZ() - this->destino.getZ(), 2));

		this->actual.setX(this->origen.getX() - distance * this->offset * ((this->origen.getX() - this->destino.getX()) / distance));
		this->actual.setY(this->origen.getY() - distance * this->offset * ((this->origen.getY() - this->destino.getY()) / distance));
		this->actual.setZ(this->origen.getZ() - distance * this->offset * ((this->origen.getZ() - this->destino.getZ()) / distance));
	}

	bool validarNave()
	{
		if (actual.getX() <= destino.getX() && actual.getY() <= destino.getY() && actual.getZ() <= destino.getZ())
		{
			return false;
		}
		return true;
	}

	bool validarMonstruo()
	{
		if (actual.getX() >= destino.getX() && actual.getY() >= destino.getY() && actual.getZ() >= destino.getZ())
		{
			return false;
		}
		return true;
	}

	void DrawBullets()
	{
		glPushMatrix();
		{
			float SphAmbRed[4] = {0.5, 0.0, 0.0, 1.0};
			float SphSpecular[4] = {1, 1, 1, 0.5};
			float SphShininess = 70;

			float Lt0spec[4] = {0.5, 0.2, 0.2, 1.0};
			float dirI[4] = {1, 0, 0, 0};

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, SphAmbRed);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SphSpecular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SphShininess);
			glLightfv(GL_LIGHT0, GL_POSITION, dirI);
			glBindTexture(GL_TEXTURE_2D, texture[1]);

			glTranslatef(actual.getX(), 0, 0);
			glTranslatef(0, actual.getY(), 0);
			glTranslatef(0, 0, actual.getZ());

			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt0spec);
			glutSolidSphere(0.2, 70, 70);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Noemit);
		}
		glPopMatrix();
	}
};

class Nave
{
public:
	OBJLoader objLoader;
	vector<Puntos> InsideNormals;
	vector<Puntos> InsidePoints;
	vector<int> InsidePointer;
	vector<int> InsideTexturePointer;
	vector<Puntos> InsideTexture;
	bool disparo = false;
	GLfloat moveX, moveY, moveZ;
	int textura;
	int vidas = 1;
	Bala bala;
	float xmin = numeric_limits<float>::max(), ymin = numeric_limits<float>::max(), zmin = numeric_limits<float>::max();
	float xmax = numeric_limits<float>::min(), ymax = numeric_limits<float>::min(), zmax = numeric_limits<float>::min();
	int estado = vivo;
	float animRot = 0;
	float animSize = 0.05;

	void crearBala()
	{
		bala.origen.setX(-moveX);
		bala.origen.setY(moveY);
		bala.origen.setZ(moveZ);
		bala.destino.setX(-moveX);
		bala.destino.setY(moveY);
		GLfloat destinoZ = moveZ - 30;
		bala.destino.setZ(destinoZ);
		bala.actual.setX(moveX);
		bala.actual.setY(moveY);
		bala.actual.setZ(moveZ);
		bala.offset = 0;
	}

	void DrawWrapper(float r)
	{

		glColor4f(0.7, 0.3, 0.5, 0.7);
		float xc = ((xmax - xmin) / 2) + xmin;
		float yc = ((ymax - ymin) / 2) + ymin;
		float zc = ((zmax - zmin) / 2) + zmin;
		glTranslatef(xc, yc, zc);
		glutWireSphere(r / 2, 16, 16);
	}

	void DrawSpaceship()
	{
		switch (estado)
		{
		case vivo:
			DrawAlive();
			break;
		case muriendo:
			DrawDying();
			break;
		case muerto:
			vidas--;
			system("clear");
			cout << "Vidas: " << vidas << endl;
			if (vidas >= 0)
				estado = vivo;
			break;
		default:
			break;
		}
	}

	void DrawAlive()
	{

		for (unsigned int i = 0; i < this->InsidePointer.size(); i += 3)
		{
			for (int j = 2; j >= 0; j--)
			{

				float x = InsidePoints[InsidePointer[i + j]].getX();
				float y = InsidePoints[InsidePointer[i + j]].getY();
				float z = InsidePoints[InsidePointer[i + j]].getZ();
				if (x < xmin)
					xmin = x;
				if (x > xmax)
					xmax = x;
				if (y < ymin)
					ymin = y;
				if (y > ymax)
					ymax = y;
				if (z < zmin)
					zmin = z;
				if (z > zmax)
					zmax = z;
			}
		}

		glPushMatrix();
		{

			glShadeModel(GL_SMOOTH);

			glRotatef(180, 0, 1, 0);
			glTranslatef(moveX, 0, 0);
			glTranslatef(0, moveY, 0);
			glScalef(0.5, 0.5, 0.5);

			glBindTexture(GL_TEXTURE_2D, texture[textura]);
			glBegin(GL_QUADS);
			{
				if (InsidePointer.size() == InsideTexturePointer.size())
				{

					for (unsigned int i = 0; i < InsidePointer.size(); i += 3)
					{
						for (int j = 2; j >= 0; j--)
						{

							float teX = InsideTexture[InsideTexturePointer[i + j]].getX();
							float teY = InsideTexture[InsideTexturePointer[i + j]].getY();
							float x = InsidePoints[InsidePointer[i + j]].getX();
							float y = InsidePoints[InsidePointer[i + j]].getY();
							float z = InsidePoints[InsidePointer[i + j]].getZ();
							glTexCoord2f(teX, teY);
							glVertex3f(x, y, z);
							x = InsideNormals[InsidePointer[i + j]].getX();
							y = InsideNormals[InsidePointer[i + j]].getY();
							z = InsideNormals[InsidePointer[i + j]].getZ();
							glNormal3f(x, y, z);
						}
					}
				}

				if (ShowWrapper)
				{
					float dx = (xmax - xmin) / 2;
					float dy = (ymax - ymin) / 2;
					float dz = (zmax - zmin) / 2;
					float r = sqrt(dx * dx + dy * dy + dz * dz);
					DrawWrapper(r);
					// ShowWrapper = false;
				}
			}
			glEnd();
		}
		glPopMatrix();
	}

	void DrawDying()
	{
		glPushMatrix();
		{
			float color[4] = {0.9, 0.3, 0.0, 1.0};
			float SphSpecular[4] = {1, 1, 1, 0.5};
			float zeroPos[4] = {0, 0, 0, 1};
			float Lt0spec[4] = {1.0, 0, 0, 1.0};

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt0spec);
			glBindTexture(GL_TEXTURE_2D, texture[3]);

			glTranslatef(-this->moveX, 0, 0);
			glTranslatef(0, this->moveY, 0);
			glTranslatef(0, 0, this->moveZ);

			glRotatef(animRot, 0.0, 1.0, 0);

			glutSolidSphere(animSize, 70, 70);
		}
		glPopMatrix();
		animRot += 0.01;
		animSize += 0.0005;
		if (animRot > 360.0)
		{
			animRot -= 360.0 * floor(animRot / 360.0);
		}
		if (animSize > 1)
		{
			estado = muerto;
		}
	}

	bool Collision(float x, float y, float z)
	{
		if (estado == vivo)
		{
			Puntos v3 = Puntos(x, y, z);

			float dx = (xmax - xmin) / 2;
			float dy = (ymax - ymin) / 2;
			float dz = (zmax - zmin) / 2;
			float r = sqrt(dx * dx + dy * dy + dz * dz) / 2;

			Puntos v4 = Puntos(r + -moveX, r / 2 + moveY, r + moveZ);
			Puntos v5 = Puntos(-moveX - r, moveY - r / 2, moveZ - r);

			if (v3.getX() <= v4.getX() && v3.getX() >= v5.getX())
			{
				if ((v3.getY() <= v4.getY() && v3.getY() >= v5.getY()))
				{
					if (v3.getZ() >= v5.getZ())
					{
						return true;
					}
				}
			}
			return false;
		}
		return false;
	}

	void DrawObjBullet()
	{

		bala.offset += 0.0008;
		bala.bulletPosition();

		if (!bala.validarNave())
		{
			bala = Bala();
			disparo = false;
		}
		else
		{
			bala.DrawBullets();
		}
	}
};

Nave nave;

class monstruo
{
public:
	OBJLoader objLoader;
	vector<Puntos> InsideNormals;
	vector<Puntos> InsidePoints;
	vector<int> InsidePointer;
	vector<int> InsideTexturePointer;
	vector<Puntos> InsideTexture;
	bool disparo = false;
	int textura;
	int estado = vivo;
	float animRot = 0;
	float animSize = 0.1;
	float k = 0;

	Bala bala;

	GLfloat MmoveX, MmoveY, MmoveZ;
	float xmin = numeric_limits<float>::max(), ymin = numeric_limits<float>::max(), zmin = numeric_limits<float>::max();
	float xmax = numeric_limits<float>::min(), ymax = numeric_limits<float>::min(), zmax = numeric_limits<float>::min();
	bool leftBorder = false;

	void crearBala()
	{
		bala.origen.setX(MmoveX);
		bala.origen.setY(MmoveY);
		bala.origen.setZ(MmoveZ);
		bala.destino.setX(MmoveX);
		bala.destino.setY(MmoveY);
		GLfloat destinoZ = MmoveZ - MmoveZ + 1;
		bala.destino.setZ(destinoZ);
		bala.actual.setX(MmoveX);
		bala.actual.setY(MmoveY);
		bala.actual.setZ(MmoveZ);
		bala.offset = 0;
		
	}

	void DrawMonster()
	{
		switch (estado)
		{
		case vivo:
			DrawAlive();
			break;
		case muriendo:
			DrawDying();
			break;

		default:
			break;
		}
	}

	void DrawAlive()
	{
		for (unsigned int i = 0; i < InsidePointer.size(); i += 3)
		{
			for (int j = 2; j >= 0; j--)
			{

				float x = InsidePoints[InsidePointer[i + j]].getX();
				float y = InsidePoints[InsidePointer[i + j]].getY();
				float z = InsidePoints[InsidePointer[i + j]].getZ();

				if (x < xmin)
					xmin = x;
				if (x > xmax)
					xmax = x;
				if (y < ymin)
					ymin = y;
				if (y > ymax)
					ymax = y;
				if (z < zmin)
					zmin = z;
				if (z > zmax)
					zmax = z;
			}
		}
		if (!checkCollisionCircle())
		{

			glPushMatrix();
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, qaWhiteDF);
				glScalef(0.8, 0.8, 0.8);

				glTranslatef(this->MmoveX, 0, 0);
				glTranslatef(0, this->MmoveY, 0);
				glTranslatef(0, 0, this->MmoveZ);

				glBindTexture(GL_TEXTURE_2D, texture[textura]);
				glBegin(GL_QUADS);
				{
					if (InsidePointer.size() == InsideTexturePointer.size())
					{

						for (unsigned int i = 0; i < InsidePointer.size(); i += 3)
						{
							for (int j = 2; j >= 0; j--)
							{

								float teX = InsideTexture[InsideTexturePointer[i + j]].getX();
								float teY = InsideTexture[InsideTexturePointer[i + j]].getY();
								float x = InsidePoints[InsidePointer[i + j]].getX();
								float y = InsidePoints[InsidePointer[i + j]].getY();
								float z = InsidePoints[InsidePointer[i + j]].getZ();
								glTexCoord2f(teX, teY);

								glVertex3f(x, y, z);
								x = InsideNormals[InsidePointer[i + j]].getX();
								y = InsideNormals[InsidePointer[i + j]].getY();
								z = InsideNormals[InsidePointer[i + j]].getZ();
								glNormal3f(x, y, z);
							}
						}
					}
				}
				glEnd();
			}
			glPopMatrix();

			if (ShowWrapper)
			{
				glPushMatrix();
				{
					float dx = (xmax - xmin) / 2;
					float dy = (ymax - ymin) / 2;
					float dz = (zmax - zmin) / 2;
					float r = sqrt(dx * dx + dy * dy + dz * dz);
					DrawWrapper(r);
				}
				glPopMatrix();
			}

			MmoveY = 3*sin(k); 
			MmoveZ += (0.5*cos(k))/100; 
			k+=0.001;

			(leftBorder ? MmoveX -= 0.001 * (4 / numMonsters) : MmoveX += 0.001 * (4 / numMonsters));

			if (MmoveX >= 5)
			{	
				leftBorder = true;
			}
			if (MmoveX <= -5)
			{
				leftBorder = false;
			}
		}
		else
		{
			estado = muriendo;
		}
	}

	void DrawDying()
	{
		glPushMatrix();
		{

			float color[4] = {0.9, 0.3, 0.0, 1.0};
			float SphSpecular[4] = {1, 1, 1, 0.5};
			float zeroPos[4] = {0, 0, 0, 1};
			float Lt0spec[4] = {1.0, 0, 0, 1.0};

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt0spec);
			glBindTexture(GL_TEXTURE_2D, texture[3]);

			glTranslatef(this->MmoveX, 0, 0);
			glTranslatef(0, this->MmoveY, 0);
			glTranslatef(0, 0, this->MmoveZ);

			glRotatef(animRot, 0.0, 1.0, 0);

			glutSolidSphere(animSize, 70, 70);
		}
		glPopMatrix();
		animRot += 0.01;
		animSize += 0.0005;
		if (animRot > 360.0)
		{
			animRot -= 360.0 * floor(animRot / 360.0);
		}
		if (animSize > 1)
		{
			estado = muerto;
			numMonsters--;
		}
	}

	bool checkCollisionCircle()
	{
		if (nave.disparo)
		{
			float dx = (xmax - xmin) / 2;
			float dy = (ymax - ymin) / 2;
			float dz = (zmax - zmin) / 2;
			float r = sqrt(dx * dx + dy * dy + dz * dz)/2;

			Bala plomo = nave.bala;
			Puntos v4 = Puntos(r + MmoveX, r + MmoveY, r + MmoveZ);
			Puntos v5 = Puntos(MmoveX - r, MmoveY - r, MmoveZ - r);
			Puntos v3 = plomo.actual;

			if (v3.getX() <= v4.getX() && v3.getX() >= v5.getX())
			{
				if ((v3.getY() <= v4.getY() && v3.getY() >= v5.getY()))
				{
					if (v3.getZ() <= v4.getZ() && v3.getZ() >= v5.getZ())
					{
						return true;
					}
				}
			}
			return false;
		}
		else
		{
			return false;
		}
	}

	void DrawWrapper(float r)
	{
		glScalef(0.5, 0.5, 0.5);

		glTranslatef(this->MmoveX, 0, 0);
		glTranslatef(0, this->MmoveY, 0);
		glTranslatef(0, 0, this->MmoveZ);

		float xc = ((xmax - xmin) / 2) + xmin;
		float yc = ((ymax - ymin) / 2) + ymin;
		float zc = ((zmax - zmin) / 2) + zmin;
		glTranslatef(xc, yc, zc);
		glutWireSphere(r, 16, 16);
	}

	bool DrawObjBullet()
	{

		bala.offset += 0.0008;
		bala.bulletPosition();

		if (!bala.validarMonstruo())
		{
			bala = Bala();
			disparo = false;
			disparando.pop_back();
			return false;
		}
		else
		{
			bala.DrawBullets();
			return (nave.Collision(bala.actual.getX(), bala.actual.getY(), bala.actual.getZ()));
		}
	}
};

monstruo monstruo1;
monstruo monstruo2;
monstruo monstruo3;
monstruo monstruo4;



void static CalcNormal(GLfloat point1_x, GLfloat point1_y, GLfloat point1_z,
					   GLfloat point2_x, GLfloat point2_y, GLfloat point2_z,
					   GLfloat point3_x, GLfloat point3_y, GLfloat point3_z)
{

	// Calculate vectors
	GLfloat value1_x = point2_x - point1_x;
	GLfloat value1_y = point2_y - point1_y;
	GLfloat value1_z = point2_z - point1_z;

	GLfloat value2_x = point3_x - point1_x;
	GLfloat value2_y = point3_y - point1_y;
	GLfloat value2_z = point3_z - point1_z;

	// Get cross product of vectors
	// See https://i.ytimg.com/vi/0wSDKkRrNFA/maxresdefault.jpg
	NormalX = (value1_y * value2_z) - (value2_y * value1_z);
	NormalY = (value1_z * value2_x) - (value2_z * value1_x);
	NormalZ = (value1_x * value2_y) - (value2_x * value1_y);

	// Normalise final vector
	float vLen = sqrt((NormalX * NormalX) + (NormalY * NormalY) + (NormalZ * NormalZ));

	NormalX = (float)NormalX / vLen;
	NormalY = (float)NormalY / vLen;
	NormalZ = (float)NormalZ / vLen;
}

void initLighting()
{

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Set lighting intensity and color
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

	// Set the light position
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
}

void loadTextureFromFile(std::string filename, int index)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	RgbImage theTexMap(filename.c_str());
	glGenTextures(1, &texture[index]); // Create The Texture
	glBindTexture(GL_TEXTURE_2D, texture[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST , GL_LINEAR are different interpolation methods for the textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST , GL_LINEAR are different interpolation methods for the textures

	// Typical Texture Generation Using Data From The Bitmap (( 8 bits red + 8 Bits green + 8 bits blue ) x N pixels in image )
	glTexImage2D(GL_TEXTURE_2D, 0, 3, theTexMap.GetNumCols(), theTexMap.GetNumRows(), 1, GL_RGB, GL_UNSIGNED_BYTE,
				 theTexMap.ImageData());
}

static void
myDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
	int i, j;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;

	ringDelta = 2.0 * M_PI / rings;
	sideDelta = 2.0 * M_PI / nsides;

	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	for (i = rings - 1; i >= 0; i--)
	{
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glBegin(GL_QUAD_STRIP);
		phi = 0.0;
		for (j = nsides; j >= 0; j--)
		{
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = R + r * cosPhi;

			glTexCoord2f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi);
			glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
			glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist, r * sinPhi);
		}
		glEnd();
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
	}
}


void MyGlutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings)
{
	myDoughnut(innerRadius, outerRadius, nsides, rings);
}

void Draw()
{

	float SphShininess = 70; 
	float SphAmbDiffBlue[4] = {1.0, 1.0, 0.8, 1.0};
	float SphSpecular[4] = {1, 1, 1, 1.0};

	CurrentAngle += AnimateStep;
	if (CurrentAngle > 360.0)
	{
		CurrentAngle -= 360.0 * floor(CurrentAngle / 360.0); 
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, SphAmbDiffBlue);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glPushMatrix();

	glScalef(10, 15, 70);
	glRotatef(90, 0.0, 1.0, 0.0);
	glTranslated(0.0, -0.4, 0.0);
	glRotatef(CurrentAngle, 0.0, 0.0, 1.0);

	MyGlutSolidTorus(0.9, 1, 61, 37);
	glPopMatrix();
}

void propDisparo()
{
	srand(time(0));
	float prop = ((rand() % 10)) / 10;
	if (prop < 0.3)
	{
		bool algo = false;
		int algoMas = 0;
		while (!algo && algoMas < 8)
		{
			srand(time(0));
			int monstruo = ((rand() % 4) + 0);
			switch (monstruo)
			{
			case 1:
				if (monstruo1.estado == vivo && !monstruo1.disparo)
				{
					monstruo1.crearBala();
					monstruo1.disparo = true;
					disparando.insert(disparando.begin(), monstruo);
					algo = true;
				}
				else
				{
					algoMas++;
				}
				break;
			case 2:
				if (monstruo2.estado == vivo && !monstruo2.disparo)
				{
					monstruo2.crearBala();
					monstruo2.disparo = true;
					disparando.insert(disparando.begin(), monstruo);
					algo = true;
				}
				else
				{
					algoMas++;
				}
				break;
			case 3:
				if (monstruo3.estado == vivo && !monstruo3.disparo)
				{
					monstruo3.crearBala();
					monstruo3.disparo = true;
					disparando.insert(disparando.begin(), monstruo);
					algo = true;
				}
				else
				{
					algoMas++;
				}
				break;
			case 4:
				if (monstruo4.estado == vivo && !monstruo4.disparo)
				{
					monstruo4.crearBala();
					monstruo4.disparo = true;
					disparando.insert(disparando.begin(), monstruo);
					algo = true;
				}
				else
				{
					algoMas++;
				}
				break;
			default:
				algoMas++;
				break;
			}
		}
	}
}

void DrawMonsterFire()
{
	bool leDio;

	for (int i = 0; i < disparando.size(); i++)
	{
		switch (disparando[i])
		{
		case 1:
			leDio = monstruo1.DrawObjBullet();
			break;
		case 2:
			leDio = monstruo2.DrawObjBullet();
			break;
		case 3:
			leDio = monstruo3.DrawObjBullet();
			break;
		case 4:
			leDio = monstruo4.DrawObjBullet();
			break;
		default:
			break;
		}
	}

	if (leDio)
		nave.estado = muriendo;
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5);

	Draw();

	nave.DrawSpaceship();

	if (nave.disparo)
	{
		nave.DrawObjBullet();
	}

	if (numMonsters > 0)
	{
		monstruo1.DrawMonster();
		monstruo2.DrawMonster();
		monstruo3.DrawMonster();
		monstruo4.DrawMonster();
	}

	if (disparando.size() > 0)
	{
		DrawMonsterFire();
	}


	glFlush();
	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_S);
	if (nave.vidas >= 0 && numMonsters > 0)
	{
		glutPostRedisplay();
	}
	else
	{
		if (nave.estado == muerto)
		{
			system("clear");
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "/////////////////									////////////////////" << endl;
			cout << "/////////////////				GAME OVER				////////////////////" << endl;
			cout << "/////////////////									////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			exit(0);
		}
		else
		{
			glutPostRedisplay();
		}

		if (numMonsters == 0)
		{
			system("clear");
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "/////////////////									////////////////////" << endl;
			cout << "/////////////////				Felicidades!!				////////////////////" << endl;
			cout << "/////////////////	     		Defendiste al planeta con exito			////////////////////" << endl;
			cout << "/////////////////			    	    :D      	 			////////////////////" << endl;
			cout << "/////////////////									////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
			cout << "" << endl;	
			exit(0);
		}
	}
}

void resizeWindow(int x, int y)
{
	if (y == 0 || x == 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(50.0,
				   (GLdouble)x / (GLdouble)y,
				   0.2,
				   50.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);
}

void keyboard(unsigned char key, int x, int y)
{
	propDisparo();
	key = tolower(key);

	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (nave.moveY <= 2 && nave.estado == vivo)
			nave.moveY += 0.1;
		break;
	case 'a':
		if (nave.moveX <= 3 && nave.estado == vivo)
			nave.moveX += 0.1;
		break;
	case 's':
		if (nave.moveY > -2 && nave.estado == vivo)
			nave.moveY -= 0.1;
		break;
	case 'd':
		if (nave.moveX > -3 && nave.estado == vivo)
			nave.moveX -= 0.1;
		break;
	case 32:
		if (!nave.disparo && nave.estado == vivo)
		{
			nave.crearBala();
			nave.disparo = true;
		}
		break;
	default:
		break;
	}
}

void loadOBJs()
{
	nave.objLoader.cargarOBJ("../Media/spaceship.obj");
	nave.InsidePoints = nave.objLoader.getVertices();
	nave.InsideNormals = nave.objLoader.getNormales();
	nave.InsidePointer = nave.objLoader.getIndices();
	nave.InsideTexturePointer = nave.objLoader.getIndicesText();
	nave.InsideTexture = nave.objLoader.getTexturas();
	nave.textura = 0;
	nave.bala = Bala();
	nave.crearBala();

	monstruo1.objLoader.cargarOBJ("../Media/enemy1.obj");
	monstruo1.InsidePoints = monstruo1.objLoader.getVertices();
	monstruo1.InsideNormals = monstruo1.objLoader.getNormales();
	monstruo1.InsidePointer = monstruo1.objLoader.getIndices();
	monstruo1.InsideTexturePointer = monstruo1.objLoader.getIndicesText();
	monstruo1.InsideTexture = monstruo1.objLoader.getTexturas();
	monstruo1.textura = 4;
	monstruo1.MmoveY = 2;
	monstruo1.MmoveZ = -15;
	monstruo1.leftBorder = false;
	monstruo1.bala = Bala();
	monstruo1.crearBala();
	monstruo1.k=(3.14*3)/2;
	numMonsters++;

	monstruo3.objLoader.cargarOBJ("../Media/enemy1.obj");
	monstruo3.InsidePoints = monstruo3.objLoader.getVertices();
	monstruo3.InsideNormals = monstruo3.objLoader.getNormales();
	monstruo3.InsidePointer = monstruo3.objLoader.getIndices();
	monstruo3.InsideTexturePointer = monstruo3.objLoader.getIndicesText();
	monstruo3.InsideTexture = monstruo3.objLoader.getTexturas();
	monstruo3.textura = 4;
	monstruo3.MmoveY = -2;
	monstruo3.MmoveZ = -15;
	monstruo3.leftBorder = false;
	monstruo3.bala = Bala();
	monstruo3.crearBala();
	monstruo3.k=3.14/2;
	numMonsters++;

	monstruo2.objLoader.cargarOBJ("../Media/enemy3.obj");
	monstruo2.InsidePoints = monstruo2.objLoader.getVertices();
	monstruo2.InsideNormals = monstruo2.objLoader.getNormales();
	monstruo2.InsidePointer = monstruo2.objLoader.getIndices();
	monstruo2.InsideTexturePointer = monstruo2.objLoader.getIndicesText();
	monstruo2.InsideTexture = monstruo2.objLoader.getTexturas();
	monstruo2.textura = 7;
	monstruo2.MmoveY = -2;
	monstruo2.MmoveZ = -20;
	monstruo2.leftBorder = true;
	monstruo2.bala = Bala();
	monstruo2.crearBala();
	monstruo2.k=(3.14*3)/2;
	numMonsters++;

	

	monstruo4.objLoader.cargarOBJ("../Media/enemy3.obj");
	monstruo4.InsidePoints = monstruo4.objLoader.getVertices();
	monstruo4.InsideNormals = monstruo4.objLoader.getNormales();
	monstruo4.InsidePointer = monstruo4.objLoader.getIndices();
	monstruo4.InsideTexturePointer = monstruo4.objLoader.getIndicesText();
	monstruo4.InsideTexture = monstruo4.objLoader.getTexturas();
	monstruo4.textura = 7;
	monstruo4.MmoveY = 2;
	monstruo4.MmoveZ = -20;
	monstruo4.leftBorder = true;
	monstruo4.bala = Bala();
	monstruo4.crearBala();
	monstruo4.k=3.14/2;
	numMonsters++;
}

int main(int argc, char **argv)
{
	loadOBJs();
	system("clear");
	cout << "Como Jugar" << endl;
	cout << "			arriba = w\n\n izquierda = a						derecha = d\n\n			abajo = s" << endl;
	cout << endl
		 << "	    Dispara con la barra espaciadora" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(10, 20);
	glutCreateWindow("Space Invaders");

	std::string filename[8] = {"../Media/pintura1.bmp", "../Media/pintura2.bmp", "../Media/galaxy.bmp", "../Media/explotion.bmp", "../Media/pintura3.bmp", "../Media/pintura4.bmp", "../Media/alien.bmp","../Media/pintura5.bmp"};
	for (int i = 0; i < 8; i++)
		loadTextureFromFile(filename[i], i);

	initLighting();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}