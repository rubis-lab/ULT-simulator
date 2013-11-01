#pragma once

#include <string.h>

#ifndef ENV_LINUX
char* strtok_r(char* str, char* delim, char** context);
#endif

class Vector
{
public:
	Vector(bool isValid);
	~Vector(void);
	Vector(double x, double y, double z, bool isValid);
	Vector(char* string, bool isValid);


private:
	double getAngleRelative(Vector &v);

public:
	double x, y, z;
	Vector operator + (Vector v);
	Vector operator - (Vector v);
	Vector operator - ();
	Vector operator * (Vector v);
	Vector operator * (double val);
	double operator & (Vector v);
	Vector GetUnitVector();
	double GetSize();
	double GetSqrSize();
	double GetDistance(Vector v);
	double GetSqrDistance(Vector v);
	bool IsEqual(const Vector &v);
	double GetAngle(Vector &v);	
	double GetAnglePI(Vector &v);
	void Print();
	void Println();
	Vector GetCompCross(Vector &v);
};
