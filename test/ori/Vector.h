#pragma once

#include <string.h>

#ifndef ENV_LINUX
char* strtok_r(char* str, char* delim, char** context);
#endif

class Vector
{
public:
	Vector(bool isValid = true);
	~Vector(void);
	Vector(double x, double y, double z);
	Vector(char* string);


private:
	double getAngleRelative(Vector &v);

public:
	double x, y, z;
	bool isValid;
	Vector operator + (Vector v);
	Vector operator - (Vector v);
	Vector operator - ();
	Vector operator * (Vector v);
	Vector operator * (double val);
	double operator & (Vector v);
	Vector getUnitVector();
	double getSize();
	double getSqrSize();
	double getDistance(Vector v);
	double getSqrDistance(Vector v);
	bool isEqual(const Vector &v);
	double getAngle(Vector &v);	
	double getAnglePI(Vector &v);
	double temp;
	void Print();
	void Println();
	Vector getCompCross(Vector &v);
};
