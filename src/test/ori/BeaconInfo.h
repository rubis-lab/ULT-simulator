#pragma once
#include "Vector.h"
#include "time.h"

class BeaconInfo
{
private:
	void reset();
public:
	BeaconInfo();
	BeaconInfo(int beacon_id, Vector location);
	~BeaconInfo(void);

public: 
	void SetLocation(Vector pos);
	Vector location;
	int beacon_id;
	clock_t timestamp;
	double distance;
	int temp, temp2;
	Vector vTemp;
	Vector vTemp2;

};
