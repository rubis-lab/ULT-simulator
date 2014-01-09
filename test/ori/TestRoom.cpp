#include <stdlib.h>
#include <string.h>
#include "TestRoom.h"


#define RAND_WIDTH(r) (r * width - (width / 2))
#define RAND_LENGTH(r) (r * length - (length / 2))
#define RAND_HEIGHT(r) (r * (height / 2) + (height / 2))
#define MIN(a,b) (a<b?a:b)


TestRoom::TestRoom(LocationEstimator *estimator, Random *random, Argument args)
{
	this->width = args.RoomWidth;
	this->length = args.RoomLength;
	this->height = args.RoomHeight;
//	nBeacon = args.cNBeacon;

	this->estimator = estimator;
	this->Reset();
	this->random = random;

//	SetRandomBeacon(args.cNBeacon);		// random deployment without considering distances between beacons
//	SetRandomBeacon2(args.cNBeacon);		// fair random deployment. Deploy beacons on its allocated region
//	SetBeaconLocationFixed(args.cNBeacon);
//	setBeaconList(estimator);
//	SetBeaconLocationFixed(args.SIM_NBeacon);

	this->args = args;	
	if (args.SIM_BeaconDeploy == SIM::AUTO)
		SetBeaconByCoulomb(args.SIM_NBeacon);
//		SetBeaconLocationFixed(args.SIM_NBeacon);
	else
		setBeaconFromFile(args.SIM_BeaconConfFile);
	setReflectedBeacons();
	this->errAvg = args.SIM_EstimationErr;
	this->errDev = args.SIM_EstimationDev;
	this->angleRange = args.SIM_AngleRange / 2;
}

TestRoom::~TestRoom(void)
{
}


void TestRoom::SetBeacon(int bid, double x, double y, double z)
{
	estimator->SetBeaconInfo(bid, Vector(x, y, z));
}

void TestRoom::SetError(double errAvg, double errDev)
{
	this->errAvg = errAvg;
	this->errDev = errDev;
}


void TestRoom::Reset()
{
	
	beaconList.clear();
	reflectList.clear();

	estimator->Reset();
//	estimator->SetPlanes(width, length, height);
	vListener = Vector(0, 0, 0);
	uvFace = Vector(0, 0, 10).getUnitVector();
	errAvg = 5;	// default 5cm
	errDev = 1;

}
void TestRoom::Reset(int width, int length, int height)
{
	this->width = width;
	this->length = length;
	this->height = height;
	this->Reset();
}

void TestRoom::setBeaconList(LocationEstimator *estimator)
{
	for (int i = 0; i < estimator->beaconListSize; i++)
	{
		if (estimator->beaconList[i] == NULL) continue;
		Vector vLocation = estimator->beaconList[i]->location;
		BeaconInfo beacon = BeaconInfo(i, vLocation);
		beaconList.push_back(beacon);
	}
}

void TestRoom::SetListener(Vector location, Vector vFace, long timestamp)
{
	vListener = location;
	uvFace = vFace.getUnitVector();
	setHeardDistance(timestamp, errAvg, errDev);
}


void TestRoom::setReflectedBeacons(BeaconInfo beacon,reflectionInfo reflect)
{
	reflectList.push_back(reflect);	
	if (reflect.n_plane == MAX_REF_CNT) return;

	for (size_t i = 0; i < estimator->planeList.size(); i++)
	{
		reflectionInfo new_reflect = reflect;
		BeaconInfo new_beacon = beacon;

		if (estimator->planeList[i].getDistanceToPoint(beacon.location) < MIN_SEP)
			continue;

		bool flag = true;
		for (int j = 0; j < new_reflect.n_plane; j++)
		{			
			if (new_reflect.plane[j].idx == (int)i)
			{
				flag = false;
				break;
			}
		}
		if (!flag) continue;

		new_beacon.location = estimator->planeList[i].GetReflectedPoint(beacon.location);
		new_reflect.beacon = new_beacon;

		// only valid for MAX_REF_CNT == 2
		if (new_reflect.n_plane > 0)			
			new_reflect.plane[new_reflect.n_plane - 1] = estimator->planeList[i].GetReflectedPlane(new_reflect.plane[new_reflect.n_plane - 1]);

		new_reflect.plane[new_reflect.n_plane] = estimator->planeList[i];
			
		new_reflect.n_plane ++;
		setReflectedBeacons(new_beacon, new_reflect);	
	}
}
	


void TestRoom::setReflectedBeacons()
{
	for (int i = 0; i < estimator->beaconListSize; i++)
	{
		if (estimator->beaconList[i] == NULL) continue;

		Vector oriPoint = estimator->beaconList[i]->location;
		BeaconInfo beacon = BeaconInfo(i, oriPoint);
		reflectionInfo reflect;
		reflect.beacon = beacon;
		reflect.n_plane = 0;
		reflect.distance = 0;
		setReflectedBeacons(beacon, reflect);
	}
}




/* This function returns the approximated error based on measured data. 
varialbes are listener's angle, beacon's angle, distance.
It is modeled as linear system.
*/
double TestRoom::GetAppError(Vector vBeacon, Vector vListener)
{
	const double cL = 0.043;
	const double cB = 0.0289;
	const double cD = 0.0019;

	Vector vLToB = vBeacon - vListener;
	Vector vBToL = vListener - vBeacon;

	double aLToB = uvFace.getAngle(vLToB);
	double aBToL = Vector(0, 0, -1).getAngle(vBToL);
	double distance = vLToB.getSize();

	if (aLToB > angleRange || aBToL > angleRange /*limite angle*/) return -1;

	double approximatedDistance = cL*aLToB + cB*aBToL + cD*distance;

	return approximatedDistance;
}



bool TestRoom::checkValidReflection(reflectionInfo &reflect, Vector vListener)
{
		double prevDistance = 0;
		double curDistance;
		Vector vBeacon = reflect.beacon.location;
		std::vector<Vector> checkList;
		checkList.push_back(estimator->beaconList[reflect.beacon.beacon_id]->location);
		reflect.isValid = false;

		for (int i = 0; i < reflect.n_plane; i++)
		{
			Vector vCross = reflect.plane[i].GetCrossedPoint(vListener, vBeacon);
			reflect.vCross[i] = vCross;
			curDistance = vListener.getDistance(vCross);
			if (!vCross.isValid || curDistance < prevDistance)
				return false;
			prevDistance = curDistance;
			checkList.push_back(vCross);
		}
		checkList.push_back(vListener);


		for (size_t i = 0; i < estimator->planeList.size(); i++)
		{
			if (estimator->planeList[i].getDistanceToPoint(vBeacon) < MIN_SEP) continue;	
			bool itercheck = true;
			for (int j = 0; j < reflect.n_plane; j++)
			{
				if (reflect.plane[j].idx == estimator->planeList[i].idx)
				{
					itercheck = false;
					break;
				}
			}
			if (!itercheck) continue;


			for (size_t j = 0; j < checkList.size() - 1; j++)
			{
				Vector vCross = estimator->planeList[i].GetCrossedPoint(checkList[j+1], checkList[j]);
				if (vCross.isValid)
					return false;
			}

		}

				
		reflect.isValid = true;

		return true; 
}

void TestRoom::setCrossPoint(reflectionInfo *reflect)
{
	for (int i = 0; i < reflect->n_plane -1; i++)
	{
		reflect->plane[i] = reflect->plane[i + 1].GetReflectedPlane(reflect->plane[i]);
		reflect->vCross[i] = reflect->plane[i + 1].GetReflectedPoint(reflect->vCross[i]);
	}
}

void TestRoom::setHeardDistance(long timestamp, double errAvg, double errDev)
{	

	memset(minReflect, 0, sizeof(reflectionInfo*) * (MAX_BID + 1));

	for (size_t i = 0; i < reflectList.size(); i++)
	{	
		
		reflectList[i].vListener = vListener;
		Vector beacon = reflectList[i].beacon.location;
		int bid = reflectList[i].beacon.beacon_id;		
		double exactDistance = beacon.getDistance(vListener);		

		if (exactDistance > (double)args.SIM_MaxMeasurement) 
		{
			continue;
		}

		// check the validation of reflection scenario with given listener position
		if (!checkValidReflection(reflectList[i], vListener)) continue;


		// get measurement error and angle limitation check
		// this value depends on transmission and reception angles and measured distance
		double measurementError = GetAppError(beacon, vListener);
		if (measurementError < 0) continue;

// debug for plane detection
//		double distanceIncErr = exactDistance;
		double distanceIncErr = exactDistance + random->GetGaussDist(measurementError + errAvg, errDev);
		if (minReflect[bid] == NULL || minReflect[bid]->distance > distanceIncErr)	
		{
			minReflect[bid] = &reflectList[i];			
			minReflect[bid]->distance = distanceIncErr;
		}
	}

	for (int i = 0; i < estimator->beaconListSize; i++)
	{
		if (minReflect[i] == NULL) continue;

		estimator->InsertHeardData(1, timestamp, i, (int)minReflect[i]->distance);
		setCrossPoint(minReflect[i]);
	}
}


void TestRoom::SetRandomBeacon(int nBeacon, bool onCeiling)
{
	for (int i = 1; i <= nBeacon; i++)
	{

		double val1 = random->GetUniformDist();
		double val2 = random->GetUniformDist();
		int plane = (int)(random->GetUniformDist()*5);

		if (onCeiling)
			plane = 0;

		switch(plane)
		{
		case 0:
			SetBeacon(i, RAND_WIDTH(val1), RAND_LENGTH(val2), height); 
			break;
		case 1:
			SetBeacon(i, RAND_WIDTH(val1), length / 2, RAND_HEIGHT(val2));
			break;
		case 2:
			SetBeacon(i, width / 2, RAND_LENGTH(val1), RAND_HEIGHT(val2));
			break;
		case 3:
			SetBeacon(i, RAND_WIDTH(val1), -length / 2, RAND_HEIGHT(val2));
			break;
		case 4:
			SetBeacon(i, -width / 2, RAND_LENGTH(val1), RAND_HEIGHT(val2));
			break;
		}
	}
		
}


void TestRoom::SetRandomBeacon2(int nBeacon)
{
	int bid = 1;
	if (nBeacon > 3)
	{
		SetBeacon(bid++, 0, 0, height);
		nBeacon --;
	}
	for (int i = 0; i < nBeacon; i++)
	{
		double baseAngle = 2 * M_PI / (double) nBeacon;
		double rAngle = baseAngle * (i  + random->GetUniformDist());
		double rDistance = random->GetGaussDist(MIN(width/2.0, length/2.0)*(2.0/3.0), 1.0);
		SetBeacon(bid++, cos(rAngle) * rDistance, sin(rAngle) * rDistance, height);
	}
}

void TestRoom::SetBeaconLocationFixed(int nBeacon)
{
	int bid = 1;
	for (int i = 0; i < nBeacon; i++)
	{
		double baseAngle = 2 * M_PI / (double) nBeacon;
		double rAngle = baseAngle * i;
		double rDistance = width/2.0 * 1.0 / 2.0;
		SetBeacon(bid++, cos(rAngle) * rDistance, sin(rAngle) * rDistance, height);
	}

}

void TestRoom::setBeaconFromFile(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) 
	{
		printf("invalid beacon configuration file : %s\n", filename);
		getchar();
		exit(1);
	}
	char buf[1024];
	char* context;
	printf("%s\n", filename);
	int bid;
	double val[3];
	while(fgets(buf, sizeof(buf) - 1, fp) != NULL)
	{
		char* temp = strtok_s(buf, "#", &context);
		bid = atoi(temp);
		for (int i = 0; i < 3; i++)
		{
			temp = strtok_s(NULL, ",\r\n", &context);
			val[i] = atof(temp);
		}
		SetBeacon(bid, val[0], val[1], val[2]);
	}
	fclose(fp);
}


void TestRoom::SetBeaconByCoulomb(int nBeacon)
{
	std::vector<Vector> vBeacon;


	vBeacon.push_back(Vector(0, 0, args.RoomHeight));
	for (int i = 1; i < nBeacon; i++)
	{
		double baseAngle = 2 * M_PI / (double) (nBeacon - 1);
		double rAngle = baseAngle * i;
		double rDistance = args.RoomWidth/2.0 * 1.0 / 2.0 / 2;
		vBeacon.push_back(Vector(cos(rAngle) * rDistance, sin(rAngle) * rDistance, args.RoomHeight));
	}
	
	const int qPlane = 20;
	const int qBeacon = 20;
	const int k = 1000;
	bool stable = false;
	do
	{
		stable = true;
		for (int i = 0; i < nBeacon; i++)
		{
			Vector vForce = Vector(0, 0, 0);
			Vector vNorm;
			double distance;
			for (size_t j = 0; j < estimator->planeList.size(); j++)
			{
				vNorm = (vBeacon[i] - estimator->planeList[j].GetReflectedPoint(vBeacon[i])).getUnitVector();
				distance = estimator->planeList[j].getDistanceToPoint(vBeacon[i]);
				if (distance < 1) continue;
				vForce = vForce + (vNorm * (k * qPlane * qBeacon / (distance * distance)));
			}
			for (int j = 0; j < nBeacon; j++)
			{
				if (i == j) continue;
				vNorm = (vBeacon[i] - vBeacon[j]).getUnitVector();
				distance = vBeacon[i].getDistance(vBeacon[j]);
				if (distance < 1) continue;
				vForce = vForce + (vNorm * (k * qBeacon * qBeacon / (distance * distance)));
			}
			vForce.z = 0;
			if (vForce.getSize() > 0.5) stable = false;

			bool inside;
			do
			{
				inside = true;
				Vector vExpected = vBeacon[i] + vForce;
				for (size_t j =0; j < estimator->planeList.size(); j++)
				{
					if (estimator->planeList[j].GetSign(vExpected) > 0)
					{
						inside = false; 
						break;
					}
				}
				vForce = vForce * 0.5;
			}while(!inside);
			vBeacon[i] = vBeacon[i] + vForce;
//			printf("%.3f ", vForce.getSize());
		}
	}while(!stable);	

	int bid = 1;
	for (int i = 0; i < nBeacon; i++)
	{
		SetBeacon(bid++, vBeacon[i].x, vBeacon[i].y, vBeacon[i].z);
	}
}
