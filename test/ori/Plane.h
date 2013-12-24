#pragma once
#include <vector>
#include "Vector.h"

#define MIN_SEP 10

#ifdef ENV_LINUX
#define strtok_s strtok_r
#endif

class Plane
{
private:
	std::vector <Vector> vertex;
	Vector getNormal();
	double getSignedAnglePI(Vector &v1, Vector &v2);	
	bool isBoundary;
	

public:
	Plane(Vector v1, Vector v2, Vector v3, bool isBoundary = false);
	Plane(Vector v1, Vector v2, Vector v3, Vector v4, bool isBoundary = false);
	Plane(char *string);
	Plane();
	~Plane(void);



//	Vector pos[3];
	Vector vNormal;	

	double getDistanceToPoint(Vector point);
	Vector GetReflectedPoint(Vector point);
	int idx;
	bool isValid;

	int GetSign(Vector point);
	Vector GetCrossedPoint(Vector &p1, Vector &p2);
	void InsertVertex(Vector &v);
	Plane GetReflectedPlane(Plane &plane);

	void GetString(char *buf);
	Vector GetVertex(int idx);
	int GetVertexSize();	
	bool IsBoundary();
	void Print();
	void Println();
	bool checkInside(Vector &p);
	
};
