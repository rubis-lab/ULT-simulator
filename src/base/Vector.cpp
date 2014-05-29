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

bool dEQ(double x, double y)
{
	const double precision = 0.0001;
	return fabs(x - y) <= precision;
}

Vector::Vector(bool isNotNull)
{
	x = y = z = 0;
	isNull = !isNotNull;
}

Vector::Vector(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector::Vector(char *string)
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

	return ret;
}

Vector Vector::operator -(Vector v)
{
	Vector ret;
	ret.x = this->x - v.x;
	ret.y = this->y - v.y;
	ret.z = this->z - v.z;
	
	return ret;
}

Vector Vector::operator -()
{
	Vector ret;
	ret.x = - this->x;
	ret.y = - this->y;
	ret.z = - this->z;

	return ret;
}

Vector Vector::operator *(Vector v)
{
	Vector ret;
	ret.x = this->y * v.z - this->z * v.y;
	ret.y = this->z * v.x - this->x * v.z;
	ret.z = this->x * v.y - this->y * v.x;

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

	return ret;
}

double Vector::getSize()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}
double Vector::getSqrSize()
{
	return pow(x,2) + pow (y, 2) + pow (z, 2);
}
Vector Vector::getUnitVector()
{	
	double size = getSize();
	if (size == 0) return Vector();
	return Vector(x / size, y / size, z / size);
}

double Vector::getDistance(Vector v)
{
	return sqrt(this->getSqrDistance(v));	
}

double Vector::getSqrDistance(Vector v)
{
	return pow(x - v.x, 2) +  pow(y - v.y, 2) + pow (z - v.z, 2);
}


bool Vector::isEqual(const Vector& v)
{
	return dEQ(x, v.x) && dEQ(y, v.y) && dEQ(z, v.z);	
}


double Vector::getAngle(Vector &v)
{
	double angle_pi = getAnglePI(v);
	return (angle_pi / M_PI * 180);
}


double Vector::getAnglePI(Vector &v)
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

Vector Vector::getCompCross(Vector &v)
{
	return Vector(x * v.x, y * v.y, z * v.z);
}

void Vector::print()
{
	printf("(%4.3f, %4.3f, %4.3f)", x, y, z);
}

void Vector::println()
{
	print();
	printf("\n");
}

