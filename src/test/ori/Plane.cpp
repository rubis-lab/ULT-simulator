#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "Plane.h"
//#include "gsl/gsl_math.h"

#define new DEBUG_NEW

Plane::Plane(Vector v1, Vector v2, Vector v3, bool isBoundary)
{
	vertex.clear();
	InsertVertex(v1);
	InsertVertex(v2);
	InsertVertex(v3);

	this->isBoundary = isBoundary;
}

Plane::Plane(Vector v1, Vector v2, Vector v3, Vector v4, bool isBoundary)
{
	vertex.clear();
	InsertVertex(v1);
	InsertVertex(v2);
	InsertVertex(v3);
	InsertVertex(v4);
	this->isBoundary = isBoundary;
}

Plane::Plane(char* string)
{
	char buf[255];
	char *tok;

	vertex.clear();

	strcpy(buf, string);
	char *str;
	char *context = NULL;
	
	if (buf[0] == 'O')
	{
		isBoundary = true;
		str = buf + 1;
	}
	else 
	{
		isBoundary = false;
		str = buf;
	}


	while((tok = strtok_s(str, "#\r\n", &context)) != NULL)
	{
		str = NULL;
		Vector point = Vector(tok);
		InsertVertex(point);
	}
}


Plane::Plane()
{
	vertex.clear();
	isValid = false;
	isBoundary = false;
}



Plane::~Plane(void)
{
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
			ret.Println();
			getchar();
			exit(41);
		
		}
		
	}		

	isValid = true;

	return ret;
}

double Plane::getDistanceToPoint(Vector point)
{	
	return fabs((vertex[0] - point)&vNormal);
}

Vector Plane::GetReflectedPoint(Vector point)
{
	Vector propNormal = vNormal * ((vertex[0] -  point) & vNormal);
	return point + (propNormal * 2);
}

int Plane::GetSign(Vector point)
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
			vertex[i].Println();
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
		this->Println();
		printf("point = ");
		p.Println();
		
		printf("Plane::checkInside, can not determine inclusion. (error code:43)\n");
		printf("\tangleSum = %f\n", angleSum);
		getchar();
		exit(43);
	}
}

Vector Plane::GetCrossedPoint(Vector &p1, Vector &p2)
{
	Vector ret;
	
	if (GetSign(p1) * GetSign(p2) >= 0)
	{
		// two points are outside of plane
		ret.isValid = false;
		return ret;
	}

	Vector vDir = p2 - p1;
	double dist1 = getDistanceToPoint(p1);
	double dist2 = getDistanceToPoint(p2);
	ret = p1 + (vDir * (dist1 / (dist1 + dist2)));

	ret.isValid = checkInside(ret);

	return ret;
}




void Plane::InsertVertex(Vector &v)
{
	vertex.push_back(v);
	if (vertex.size() >= 3) 
		vNormal = getNormal();
}

Plane Plane::GetReflectedPlane(Plane &plane)
{
	Plane ret = plane;
	ret.vertex.clear();
	
	for (size_t i = 0; i < plane.vertex.size(); i++)
	{
		Vector reflectPoint = this->GetReflectedPoint(plane.vertex[i]);
		ret.InsertVertex(reflectPoint);
	}
	
	return ret;
}


void Plane::GetString(char *string)
{
	char buf[255];
	int offset = 0;

	for (size_t i = 0; i < vertex.size(); i++)
	{
		offset += sprintf(buf + offset, "(%.5f,%.5f,%.5f)#", vertex[i].x, vertex[i].y, vertex[i].z);
		if (offset > (int)sizeof(buf))
		{
			printf("Plane::GetString, buffer overflow\n");
			getchar();
			exit(44);
		}
	}
	strcpy(string, buf);
}

Vector Plane::GetVertex(int idx)
{
	return vertex[idx];
}

int Plane::GetVertexSize()
{
	return vertex.size();
}

bool Plane::IsBoundary()
{
	return isBoundary;
}

void Plane::Print()
{
	printf("PID (%d) :", idx);
	for (size_t i = 0; i < vertex.size(); i++)
	{
		vertex[i].Print();
		printf(" # ");
	}
}

void Plane::Println()
{
	Print();
	printf("\n");
}