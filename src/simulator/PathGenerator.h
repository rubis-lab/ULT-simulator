#pragma once

#include "SimulatorArgument.h"
#include "ListenerMover.h"

class PathGenerator
{
public:
	PathGenerator();
	~PathGenerator();

	void generatePath(SimulatorArgument *args, std::vector<ListenerInfo> &path);
	void generatePathForPD(SimulatorArgument *args, std::vector<ListenerInfo> &path);
	

protected:
	int margin;
	int hallway_x;
	int hallway_y;

	SimulatorArgument *args;


	Vector getPointRND();
	Vector getPointCW(Vector vPos);
	Vector getPointACW(Vector vPos);
	Vector getPoint(Vector vRange, Vector vOffset);
	int getArea(Vector vPos);


};
