#include "DistanceScenario.h"


DistanceScenario::DistanceScenario(SimulatorArgument *args, Beacon *beacon)
{
	this->args = args;
	virtualBeacon = beacon;
	valid = false;
}
DistanceScenario::~DistanceScenario()
{
	reflectedPoints.clear();
}


bool DistanceScenario::setListener(Vector location, Vector facing)
{
	listenerLocation = location;
	listenerFacing = facing;

	bool ret = calculateDistance();

	return ret;
}



bool DistanceScenario::calculateDistance()
{
	reflectedPoints.clear();
	valid = false;
	distance = virtualBeacon->getLocation().getDistance(listenerLocation);
	if (distance > args->maxMeasurableDistance) return false;
	if (distance < args->minMeasurableDistance) return false;
	
	valid = checkValid();
	
	
	applicationError = getApplicationError();
	if (applicationError < 0) return false;			// out of valid angle range
	randomNoise = getRandomNoise(applicationError);

	
	return valid;
}

bool DistanceScenario::checkValid()
{
	std::vector<Vector> crossPoints;

	// check cross point and push to vector list
	// cross point should be checked backward
	
	crossPoints.push_back(listenerLocation);
	for (size_t i = virtualBeacon->vLocations.size() - 1; i > 0; i--)
	{
		Vector curCross = crossPoints.back();
		Vector newCross;
		Plane *curPlane = virtualBeacon->pPlanes[i-1];
		if (!curPlane->getCrossedPoint(curCross, virtualBeacon->vLocations[i], newCross))
		{
			// signal can't reflect this plane. impossible scenario
			return false;
		}
		crossPoints.insert(crossPoints.begin(),newCross);
	}

	Vector curCheckPoint = virtualBeacon->vLocations[0];
	Vector tempVector;
	int startPlaneIdx;
	int endPlaneIdx;
	bool isCross;
	for (size_t i = 0; i < crossPoints.size(); i++)
	{
		// check there are block planes.
		// planes containing start and end of points should be ignored
		// when i = 0, there is no start plane
		// i = size - 1, there is no end plane
		// crossPoints.size = plane.size + 1, because last crossPoint is listener location

		startPlaneIdx = endPlaneIdx = -1;
		
		if (i > 0) 
			startPlaneIdx = virtualBeacon->pPlanes[i - 1]->getPid();
		if (i < crossPoints.size() - 1)
			endPlaneIdx = virtualBeacon->pPlanes[i]->getPid();

		
		// check path blocking for all planes except start and end planes
		for (size_t j = 0; j < args->planes.size(); j++)
		{
			Plane *checkingPlane = args->planes.at(j);
			if (checkingPlane->getPid() == startPlaneIdx ||
				checkingPlane->getPid() == endPlaneIdx) 
				continue;
			
			isCross = checkingPlane->getCrossedPoint(curCheckPoint, crossPoints[i], tempVector);
			if (isCross) return false;
		}
		curCheckPoint = crossPoints[i];
	}

	//copy reflection points
	for (size_t i = 0; i < crossPoints.size() - 1; i++)
	{
		reflectedPoints.push_back(crossPoints[i]);
	}

	return true;
}

double DistanceScenario::getApplicationError()
{
	const double cL = 0.043;
	const double cB = 0.0289;
	const double cD = 0.0019;


	Vector location = virtualBeacon->getLocation();
	Vector vLToB = location - listenerLocation;
	Vector vBToL = listenerLocation - location;

	double aLToB = listenerFacing.getAngle(vLToB);
	double aBToL = Vector(0, 0, -1).getAngle(vBToL);
	double distance = vLToB.getSize();

	if (aLToB > args->validAngleRange / 2.0 || aBToL > args->validAngleRange / 2.0 /*limited angle*/) return -1;

	double applicationError = cL*aLToB + cB*aBToL + cD*distance;

	return applicationError;
}

double DistanceScenario::getRandomNoise(double baseError)
{
	return args->random.getGaussDist(baseError + args->distanceNoiseAvg, args->distanceNoiseDev);
}

double DistanceScenario::getExactDistance()
{
	return distance;
}

double DistanceScenario::getNoisyDistance()
{
	return distance + randomNoise;
}

double DistanceScenario::getDistance()
{
	return getNoisyDistance();
}

bool DistanceScenario::isValid()
{
	return valid;
}

int DistanceScenario::getBid()
{
	return virtualBeacon->getBid();
}

int DistanceScenario::getUserBid()
{
	return virtualBeacon->getUserBid();
}
