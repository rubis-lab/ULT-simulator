#pragma once
#include <vector>
#include "Vector.h"
#include "Plane.h"
#include "Beacon.h"
#include "BeaconList.h"


class Measurement                                                                    
{                                                                                    
public:                                                                              
	
	Measurement(Beacon* beacon);
	void measure(unsigned long timestamp, double distance);
	int getUserBid();
	Vector getLocation();

	void clearNVSS();
	void incNVSS();
	int getNVSS();
	double getDistance();
	unsigned long getTimestamp();
	void resetIterator();
	bool nextVBeacon();
	bool isValidDistance();


private:
	Beacon* beacon;                                                                  
	double distance;
	unsigned long timestamp;
	int NVSS;
};                                                                                    

class MeasurementCondition
{
public:
	MeasurementCondition():
			minSize(3),
			validSize(4),
			strictValidSize(false),
			smallerNVSSFirst(true),
			shortDistanceFirst(true),
			timeWindow(1000),
			minValidDistance(1)
	{
	}

	~MeasurementCondition(){}

	int minSize;
	int validSize;
	bool strictValidSize; 			// if # of measurements is smaller than validSize, do not calculate

	// below two flags decide sort policy when measurements are selected.
	// smallerNVSSFirst is prior to shortDistanceFirst
	bool smallerNVSSFirst;			// smaller N_VSS selected first.
	bool shortDistanceFirst;		// if true, shortest n measures are selected. otherwise, randomly select
	bool canCalculate(int measurementsSize)
	{
		if (measurementsSize < minSize) return false;
		if (strictValidSize && measurementsSize < validSize) return false;
		return true;
	}

	unsigned long timeWindow;
	
	int minValidDistance;
};
class MeasurementList
{
public:
	MeasurementList(int lid, BeaconList *beacons, PlaneList *planes);
	~MeasurementList();
	void measure(int userBid, unsigned long timestamp, double distance);
	void setMeasurementCondition(MeasurementCondition condition);
	int getLid();

	void makeSnapshot(unsigned long currentTime);
	std::vector<Measurement*> getMeasurements();		//call makeSnapshot before getting measurements

	size_t size();
	void setupVBeacon();
	bool nextVBeacon(int idx);
	void resetVBeacon(int idx);

public:
	

private:
	int findBeaconIndexByUserId(int userBid);
	std::vector<Measurement*> getRandomList(std::vector<Measurement*> filteredMeasurements);
	std::vector<Measurement*> getSortedList(std::vector<Measurement*> filteredMeasurements);
	std::vector<Measurement*> getFilteredMeasurements(unsigned long CurrentTime, unsigned long timeWindow);

private:
	const int lid;
	std::vector<Measurement> allMeasurements;
	std::vector<Measurement*> measurements;
	int lastSeq;
	MeasurementCondition condition;
	PlaneList *planes;

};
