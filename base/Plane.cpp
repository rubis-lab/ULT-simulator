#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "Plane.h"


Plane::Plane(Vector v1, Vector v2, Vector v3)
{
	init();
	insertVertex(v1);
	insertVertex(v2);
	insertVertex(v3);
}

Plane::Plane(Vector v1, Vector v2, Vector v3, Vector v4)
{
	init();
	insertVertex(v1);
	insertVertex(v2);
	insertVertex(v3);
	insertVertex(v4);
}

// load plane from a string
// TODO :: modify to read infinite plane
Plane::Plane(char* string)
{
	init();

	char buf[255];
	char *tok;

	strcpy(buf, string);
	char *str;
	char *context = NULL;
	
	if (buf[0] == 'O')
	{
		setBoundary(true);
		str = buf + 1;
	}
	else 
	{
		setBoundary(false);
		str = buf;
	}


	while((tok = strtok_s(str, "#\r\n", &context)) != NULL)
	{
		str = NULL;
		Vector point = Vector(tok);
		insertVertex(point);
	}
}

/*
Plane::Plane()
{
	init();
}
*/



//Infinite Plane
Plane::Plane(Vector v1, Vector vN)
{
	init();
	vertex.push_back(v1);
	vNormal = vN;
}

//Infinite Plane
Plane::Plane(Vector vN, double fD)
{
	init();
	Vector v1 = vN * (-fD);
	vertex.push_back(v1);
	vNormal = vN;
}


Plane::~Plane(void)
{
}


void Plane::init()
{
	vertex.clear();
	initProperty();
}

Vector Plane::getNormal()
{
	double resolution = 10000;
	Vector ret;
	ret = ((vertex[1] - vertex[0]) * (vertex[2] - vertex[0])).getUnitVector();

	for (size_t i = 3; i < vertex.size(); i++)
	{	
		if ((int)(ret & (vertex[i] - vertex[0])*resolution)/resolution != 0)
		{
			printf("Vertex %d doesn't lie on the plane\n", (int)i);
			printf("v0 : (%f, %f, %f)\n", vertex[0].x, vertex[0].y, vertex[0].z);
			printf("v1 : (%f, %f, %f)\n", vertex[1].x, vertex[1].y, vertex[1].z);
			printf("v2 : (%f, %f, %f)\n", vertex[2].x, vertex[2].y, vertex[2].z);
			if (vertex.size() > 3)
				printf("v3 : (%f, %f, %f)\n", vertex[3].x, vertex[3].y, vertex[3].z);
			printf("result of inner product : %f\n", ret & (vertex[i]-vertex[0]));
			printf("Calculated Norm = ");
			ret.println();
			getchar();
			exit(41);
		
		}
		
	}		

	return ret;
}

double Plane::getDistanceToPoint(Vector point)
{	
	return fabs((vertex[0] - point)&vNormal);
}

Vector Plane::getReflectedPoint(Vector point)
{
	Vector propNormal = vNormal * ((vertex[0] -  point) & vNormal);
	return point + (propNormal * 2);
}

int Plane::getSign(Vector point)
{
	return (int) ((point - vertex[0]) & vNormal);
}


double Plane::getSignedAnglePI(Vector &v1, Vector &v2)
{
	double angle = v1.getAnglePI(v2);
	double sign = 0.0;
	Vector vnCross = (v1 * v2).getUnitVector();

	if (vnCross.isEqual(vNormal))
		sign = 1.0;
	else if (vnCross.isEqual(-vNormal))
		sign = -1.0;
	else if (vnCross.getSize() == 0)
	{
		/* this is special case when v1 and v2 are parallel. 
		In this case simply igrore its signed.
		This is wrong, but at this time, sign is not needed.
		If sign is needed in the future, it should be modified.*/
		return angle;
	}
	else
	{		
		printf("\nPlane::getSignedAnglePI, can not find sign value. (error code:42)\n");
		for (size_t i = 0; i < vertex.size(); i++)
		{
			printf("\tvertex %d :", (int)i);
			vertex[i].println();
		}
		printf("\tv1=(%f, %f, %f)\n", v1.x, v1.y, v1.z);
		printf("\tv2=(%f, %f, %f)\n", v2.x, v2.y, v2.z);
		printf("\tvnCross=(%f, %f, %f)\n", vnCross.x, vnCross.y, vnCross.z);
		printf("\tvNormal=(%f, %f, %f)\n", vNormal.x, vNormal.y, vNormal.z);
		getchar();
		exit(42);
	}
	return angle * sign;
}


// Check whether crossed point is inside vertexes
// if the sum of the angles <Vi_P_Vi+1 is 2*PI then P is inside
// if it is 0 then P is outside
bool Plane::checkInside(Vector &p)
{
	double angleSum = 0;
	for (size_t i = 0; i< vertex.size() ; i++)
	{
		Vector v1, v2;
		v1 = vertex[i] - p;		
		v2 = vertex[(i+1) % vertex.size()] - p;
		
		angleSum += getSignedAnglePI(v1, v2);
	}
	
	const double resolution = 10000.0;
	double pi2_res = ((int)(2* M_PI * resolution)) / resolution;
	angleSum = ((int)(angleSum * resolution)) / resolution;


	if (angleSum == pi2_res)
		return true;
	else if (angleSum == 0.0)
		return false;
	else 
	{
		printf("\n");
		this->println();
		printf("point = ");
		p.println();
		
		printf("Plane::checkInside, can not determine inclusion. (error code:43)\n");
		printf("\tangleSum = %f\n", angleSum);
		getchar();
		exit(43);
	}
}



// crossed point between this plane and (p1->p2)
bool Plane::getCrossedPoint(Vector &p1, Vector &p2, Vector &ret)
{
	if (getSign(p1) * getSign(p2) >= 0)
	{
		return false;
		// two points are on outside or inside of plane at the same time
	}

	Vector vDir = p2 - p1;
	double dist1 = getDistanceToPoint(p1);
	double dist2 = getDistanceToPoint(p2);
	ret = p1 + (vDir * (dist1 / (dist1 + dist2)));
	
	return checkInside(ret);
}




void Plane::insertVertex(Vector &v)
{
	vertex.push_back(v);
	if (vertex.size() >= 3) 
		vNormal = getNormal();
}

Plane Plane::getReflectedPlane(Plane &plane)
{
	Plane ret = plane;
	ret.vertex.clear();
	
	for (size_t i = 0; i < plane.vertex.size(); i++)
	{
		Vector reflectPoint = this->getReflectedPoint(plane.vertex[i]);
		ret.insertVertex(reflectPoint);
	}
	
	return ret;
}


void Plane::getString(char *string)
{
	char buf[255];
	int offset = 0;

	for (size_t i = 0; i < vertex.size(); i++)
	{
		offset += sprintf(buf + offset, "(%.5f,%.5f,%.5f)#", vertex[i].x, vertex[i].y, vertex[i].z);
		if (offset > (int)sizeof(buf))
		{
			printf("Plane::getString, buffer overflow\n");
			getchar();
			exit(44);
		}
	}
	strcpy(string, buf);
}

Vector Plane::getVertex(int idx)
{
	return vertex[idx];
}

int Plane::getVertexSize()
{
	return vertex.size();
}


void Plane::print()
{
	printf("PID (%d) :", pid);
	for (size_t i = 0; i < vertex.size(); i++)
	{
		vertex[i].print();
		printf(" # ");
	}
}

void Plane::println()
{
	print();
	printf("\n");
}

void Plane::setId(int pid)
{
	this->pid = pid;
}
int Plane::getPid()
{
	return pid;
}

void Plane::initProperty()
{
	this->property = PROPERTY_NONE;
}

void Plane::setBoundary(bool isBoundary)
{
	if (isBoundary)
		property |= PROPERTY_BOUNDARY;
	else
		property &= ~PROPERTY_BOUNDARY;
}
bool Plane::isBoundary()
{
	return (bool)property & PROPERTY_BOUNDARY;
}

void Plane::setInfinite(bool isInfinite)
{
	if (isInfinite)
		property |= PROPERTY_INFINITE;
	else
		property &= ~PROPERTY_INFINITE;
}

bool Plane::isInfinite()
{
	return (bool)property & PROPERTY_INFINITE;
}
