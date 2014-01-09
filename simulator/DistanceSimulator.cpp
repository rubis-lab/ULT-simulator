#include "DistanceSimulator"


SimulatedDistance::SimulatedDistance(SimulatorArgument *args, Beacon *beacon)
{
	this->args = args;
	virtualBeacon = beacon;
}


void SimulatedDistance::setListener(Vector location, Vector facing)
{
	listenerLocation = location;
	listenerFacing = facing;

	calculateDistance();
}

double getExactDistance()
{
	return distance;
}

double getNoisyDistance()
{
	return distance + randomNoise;
}

void SimulatedDistance::calculateDistance()
{
	distance = virtualBeacon->getLocation().getDistance(listenerLocation);
	applicationError = getApplicationError();
	randomNoise = getRandomNoise(applicationError);
}

double SimulatedDistance::getApplicationError()
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

	if (aLToB > args->validAngleRange || aBToL > args->validAngleRange /*limite angle*/) return -1;

	double applicationError = cL*aLToB + cB*aBToL + cD*distance;

	return applicationError;
}

double SimulatedDistance::getRandomNoise(double baseError)
{
	return Random.getGaussDist(baseError + noiseAvg, noiseDev);
}
