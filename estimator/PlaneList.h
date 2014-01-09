#pragma once

#include "Plane.h"
#include <vector>

class PlaneList
{
public:
	PlaneList();
	~PlaneList();


	void reset();
	size_t size();
	Plane* at(int idx);
	void addPlane(Plane plane);
	bool checkInside(Vector location);
	void load(const char *filename);


private:

	std::vector<Plane*> planes;
	void constructor();
	void destructor();
};
