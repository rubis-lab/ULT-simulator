#include <stdlib.h>
#include <stdio.h>
#include "PlaneList.h"

PlaneList::PlaneList()
{
	constructor();
}

PlaneList::~PlaneList()
{
	destructor();
}

void PlaneList::reset()
{
	destructor();
	constructor();
}

void PlaneList::constructor()
{
}

void PlaneList::destructor()
{
	for (size_t i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}
	planes.clear();
}

size_t PlaneList::size()
{
	return planes.size();
}

Plane* PlaneList::at(int idx)
{
	return planes[idx];
}

void PlaneList::addPlane(Plane plane)
{
	Plane *newPlane = new Plane(plane);
	newPlane->setId(planes.size());

	planes.push_back(newPlane);
}

bool PlaneList::checkInside(Vector location)
{
	for (size_t i = 0; i < planes.size(); i++)
	{
		if (!planes[i]->isBoundary()) continue;
		if (planes[i]->getSign(location) > 0)
			return false;
	}
	return true;
}

void PlaneList::load(const char *filename)
{
	reset();

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("can't open plane list file %s\n", filename);
		exit(20);
	}

	const int bufSize = 1024;
	char buf[bufSize + 1];

	while(fgets(buf, bufSize, fp) != NULL)
	{
		addPlane(Plane(buf));
	}
	fclose(fp);


}

void PlaneList::save(const char *filename)
{
	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		printf("PlaneList::save, can't open file %s\n", filename);
		exit(21);
	}

	for (size_t i = 0; i < planes.size(); i++)
	{
		Plane* plane = planes[i];
		char buf[1024];

		plane->getString(buf);

		if (plane->isBoundary())
			fprintf(fp, "O");

		fprintf(fp, "%s\n", buf);
	}
	fclose(fp);
}
