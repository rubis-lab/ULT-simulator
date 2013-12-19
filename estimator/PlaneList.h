#pragma once

#include "Plane.h"
#include <vector>

class PlaneList
{
public:
	PlaneList();
	~PlaneList();


	size_t size();
	Plane* at(int idx);
private:

	std::vector<Plane*> list;
};
