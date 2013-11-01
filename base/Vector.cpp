#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Vector.h"

#ifndef ENV_LINUX
char* strtok_r(char* str, char* delim, char** context)
{
	return strtok_s(str, delim, context);
}
#endif

Vector::Vector(bool isValid)
{
	x = y = z = 0;
//	this->isValid = isValid;
}

Vector::Vector(double x, double y, double z, bool isValid = true)
{
	this->x = x;
	this->y = y;
	this->z = z;
//	isValid = true;
}

Vector::Vector(char *string, bool isValid = true)
{
	char buf[255];
	strcpy(buf, string + 1);
	
	char *tok;
	char *context = NULL;

	tok = strtok_r(buf, ",)", &context);
	this->x = atof(tok);
	tok = strtok_r(NULL, ",)", &context);
	this->y = atof(tok);
	tok = strtok_r(NULL, ",)", &context);
	this->z = atof(tok);
	isValid = true;
}

Vector::~Vector(void)
{
}

Vector Vector::operator +(Vector v)
{
	Vector ret;
	ret.x = this->x + v.x;
	ret.y = this->y + v.y;
	ret.z = this->z + v.z;
	ret.isValid = this->isValid & v.isValid;

	return ret;
}

Vector Vector::operator -(Vector v)
{
	Vector ret;
	ret.x = this->x - v.x;
	ret.y = this->y - v.y;
	ret.z = this->z - v.z;
	ret.isValid = this->isValid & v.isValid;
	
	return ret;
}

Vector Vector::operator -()
{
	Vector ret;
	ret.x = - this->x;
	ret.y = - this->y;
	ret.z = - this->z;
	ret.isValid = this->isValid;

	return ret;
}

Vector Vector::operator *(Vector v)
{
	Vector ret;
	ret.x = this->y * v.z - this->z * v.y;
	ret.y = this->z * v.x - this->x * v.z;
	ret.z = this->x * v.y - this->y * v.x;
	ret.isValid = this->isValid & v.isValid;

	return ret;
}
	
double Vector::operator &(Vector v)
{
	return this->x * v.x + this->y * v.y + this->z * v.z;	
}

Vector Vector::operator *(double val)
{
	Vector ret;
	ret.x = x * val;
	ret.y = y * val;
	ret.z = z * val;
	ret.isValid = this->isValid;

	return ret;
}

double Vector::GetSize()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}
double Vector::GetSqrSize()
{
	return pow(x,2) + pow (y, 2) + pow (z, 2);
}
Vector Vector::GetUnitVector()
{	
	double size = getSize();
	if (size == 0) return Vector(false);
	return Vector(x / size, y / size, z / size);
}

double Vector::GetDistance(Vector v)
{
	return sqrt(this->getSqrDistance(v));	
}

double Vector::GetSqrDistance(Vector v)
{
	return pow(x - v.x, 2) +  pow(y - v.y, 2) + pow (z - v.z, 2);
}

bool dEQ(double x, double y)
{
	const double precision = 0.0001;
	return fabs(x - y) <= precision;
}

bool Vector::IsEqual(const Vector& v)
{
	return (this->isValid & v.isValid) 
		&& dEQ(x, v.x) && dEQ(y, v.y) && dEQ(z, v.z);	
}


double Vector::GetAngle(Vector &v)
{
	double angle_pi = getAnglePI(v);
	return (angle_pi / M_PI * 180);
}


double Vector::GetAnglePI(Vector &v)
{
	Vector vNorm = this->getUnitVector();
	double vDist = v & vNorm;	
	double value = (vDist / v.getSize());
	if (value > 1)
		value = 1.0;
	if (value < -1)
		value = -1.0;
		
	double angle_pi = acos(value);	
	return angle_pi;
}

Vector Vector::GetCompCross(Vector &v)
{
	return Vector(x * v.x, y * v.y, z * v.z);
}

void Vector::Print()
{
	printf("(%4.3f, %4.3f, %4.3f)", x, y, z);
}

void Vector::Println()
{
	Print();
	printf("\n");
}

