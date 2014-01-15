#include "BeaconInfo.h"

BeaconInfo::BeaconInfo(int beacon_id, Vector location)
{
	reset();
	this->beacon_id = beacon_id;
	this->location = location;
}

BeaconInfo::BeaconInfo()
{
	reset();
}

BeaconInfo::~BeaconInfo(void)
{
}

void BeaconInfo::SetLocation(Vector pos)
{
	this->location = pos;

}


void BeaconInfo::reset()
{
	beacon_id = -1;
	location.isValid = false;
	timestamp = 0;
	distance = 0.0;
	temp = temp2 = 0;	
}