#pragma once
#include <vector>
#include <stdint.h>
#include "Vector.h"

#define MIN_SEP 10

#ifdef ENV_LINUX
#define strtok_s strtok_r
#endif

class Plane
{
public:
enum plane_property_t
{
	PROPERTY_NONE = 0x0,
	PROPERTY_BOUNDARY = 0x1,
	PROPERTY_INFINITE = 0x2
};

private:
	std::vector <Vector> vertex;
	Vector getNormal();
	void initProperty();
	void init();
	
	
	
	double getSignedAnglePI(Vector &v1, Vector &v2);	
	uint32_t property;
	bool checkInside(Vector &p);
	int idx;


	
	

public:
	Plane(Vector v1, Vector v2, Vector v3);
	Plane(Vector v1, Vector v2, Vector v3, Vector v4);
	Plane(char *string);
	Plane(Vector v1, Vector vN);
	Plane(Vector vN, double fD);
//	Plane();
	~Plane(void);



	Vector vNormal;	

	double getDistanceToPoint(Vector point);
	Vector getReflectedPoint(Vector point);

	int getSign(Vector point);
	bool getCrossedPoint(Vector &p1, Vector &p2, Vector &ret);
	void insertVertex(Vector &v);
	Plane getReflectedPlane(Plane &plane);

	void getString(char *buf);
	Vector getVertex(int idx);
	int getVertexSize();	
	void print();
	void println();
	void setId(int idx);
	void setBoundary(bool isBoundary);
	bool isBoundary();
	void setInfinite(bool isInfinite);
	bool isInfinite();
};
