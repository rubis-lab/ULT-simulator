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
private:

	std::vector<Plane*> planes;
	void constructor();
	void destructor();
};
