#include "PathGenerator.h"
#include "Random.h"


PathGenerator::PathGenerator()
{
}
PathGenerator::~PathGenerator()
{
}


void PathGenerator::generatePath(SimulatorArgument *args, std::vector<ListenerInfo>& path)
{
	this->args = args;
	ListenerMover listenerMover(args);
	path.clear();

	margin = 50;
	hallway_x = args->width / 16;
	hallway_y = args->width / 16;

	Vector vPos = Vector(-args->width / 2 + margin, args->length / 2 - margin,0);
	for (int i = 0; i < 5000; i++)
	{		
		double r_speed;
		while ((r_speed = Random.getGaussDist(args->speedAvg, args->speedDev)) <= 0);

		
		switch (args->moveType)
		{
		default:
		case SIM_WALKER::RANDOM: vPos = getPointRND();	break;
		case SIM_WALKER::CLOCKWISE: vPos = getPointCW(vPos); break;
		case SIM_WALKER::PLANE_DETECTION:	// ignore in this case
		case SIM_WALKER::ANTI_CLOCKWISE: vPos = getPointACW(vPos); break;
		}
		listenerMover.setPath(vPos, r_speed);
		printf("Path Generating .... %3d\r", (int)(i / 5000.0 * 100));
	}
	printf("\n");

	for (int i = 0; i < args->eventSize; i++)
	{
		ListenerInfo listener;
		bool ret = listenerMover.moveNext(listener);
		if (!ret)
		{
			printf("Warning: generated events(%d) are smaller than events size(%d)\n", i, args->eventSize);
			break;
		}
		path.push_back(listener);
	}
}



Vector PathGenerator::getPoint(Vector vRange, Vector vOffset)
{
	Vector vPos;
	vPos.x = Random.getUniformDist() * vRange.x + vOffset.x;
	vPos.y = Random.getUniformDist() * vRange.y + vOffset.y;
	vPos.z = Random.getUniformDist() * vRange.z + vOffset.z;

	return vPos;
}


Vector PathGenerator::getPointRND()
{
	Vector vRange, vOffset;
	vRange = Vector(args->width - margin * 2, args->length - margin * 2,args->height / 2);
	vOffset = Vector(margin - args->width / 2, margin - args->length / 2, args->height / 4);

	return getPoint(vRange, vOffset);
}

/* Area INFO
(1/8 + margin)
+-+-------+-+
|2|   3   |4|
|-+-------+-|
| |       | |
|1|   8   |5|
| |       | |
|-+-------+-|
|0|   7   |6|
+-+-------+-+
*/

int PathGenerator::getArea(Vector vPos)
{
	int x_border = (args->width / 2) - (args->width / 8) -  margin;
	int y_border = (args->length / 2) - (args->length / 8) -  margin;

	if (vPos.x <= - x_border)
	{
		if (vPos.y >= y_border) return 0;
		if (vPos.y <= -y_border) return 2;
		return 1;
	}
	if (vPos.x >= x_border)
	{
		if (vPos.y >= y_border) return 6;
		if (vPos.y <= -y_border) return 4;
		return 5;
	}
	if (vPos.y >= y_border) return 7;
	if (vPos.y <= -y_border) return 3;
	return 8;
}

Vector PathGenerator::getPointCW(Vector vPos)
{
	Vector vRange, vOffset;

	vRange.z = args->height / 2;
	vOffset.z = args->height / 4;
	int area = getArea(vPos);
	switch (area)
	{
	case 0:
	case 1:
		vRange.x = hallway_x;
		vRange.y = vPos.y + args->length / 2 - margin;
		vOffset.x = -args->width/2 + margin;
		vOffset.y = -args->length / 2 + margin;
		break;
	case 2:
	case 3:
		vRange.x = -vPos.x + args->width / 2 - margin;
		vRange.y = hallway_y;
		vOffset.x = vPos.x;
		vOffset.y = - args->length / 2 + margin;
		break;	
	case 4:
	case 5:
		vRange.x = hallway_x;
		vRange.y = - vPos.y + args->length / 2 -margin ;
		vOffset.x = args->width / 2 - margin - hallway_x;
		vOffset.y = vPos.y;			
		break;
	case 6:
	case 7:
		vRange.x = vPos.x + args->width/2 - margin;
		vRange.y = hallway_y;
		vOffset.x = - args->width / 2 + margin;
		vOffset.y = args->length /2 - margin - hallway_y;
		break;
	}
	return getPoint(vRange, vOffset);
}

Vector PathGenerator::getPointACW(Vector vPos)
{
	Vector vRange, vOffset;

	vRange.z = args->height / 2;
	vOffset.z = args->height / 4;
	int area = getArea(vPos);
	switch (area)
	{
	case 0:
	case 7:
		vRange.x = -vPos.x + args->width / 2 - margin;
		vRange.y = hallway_y;
		vOffset.x = vPos.x;
		vOffset.y = args->length /2 - margin - hallway_y;
		break;
	case 5:
	case 6:
		vRange.x = hallway_x;
		vRange.y = vPos.y + args->length / 2 - margin;
		vOffset.x = args->width / 2 - margin - hallway_x;
		vOffset.y = -args->length / 2 + margin;
		break;	
	case 3:
	case 4:
		vRange.x = vPos.x + args->width/2 - margin;
		vRange.y = hallway_y;
		vOffset.x = - args->width / 2 + margin;
		vOffset.y = - args->length / 2 + margin;
		break;
	case 1:
	case 2:
		vRange.x = hallway_x;
		vRange.y = - vPos.y + args->length / 2 -margin ;
		vOffset.x = -args->width/2 + margin;
		vOffset.y = vPos.y;			
		break;
	}
	return getPoint(vRange, vOffset);
}
