#include "PlaneList.h"

PlaneList::PlaneList()
{
}

PlaneList::~PlaneList()
{
}

size_t PlaneList::size()
{
	return list.size();
}

Plane* PlneList::at(int idx)
{
	return list[idx];
}