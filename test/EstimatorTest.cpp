#include <string.h>
#include "Estimator.h"
#include "BeaconInfo.h"
#define MAX_BID 100

class LogInfo
{
public :
	Vector vPosition;
	Vector vFace;
	BeaconInfo* beaconList;
	long timestamp;
};

std::vector <LogInfo> logList;
int l_width, l_length, l_height;
int nBeacon;	
PlaneList planes;

void clear_log_list(std::vector <LogInfo> &logList)
{
	for (unsigned int i = 0; i < logList.size(); i++)
	{
		free(logList[i].beaconList);
	}
	logList.clear();
}

double read_value(char* str)
{
	return atof(strtok(str, ";"));
}
long read_value_long(char* str)
{
	return atol(strtok(str, ";"));
}
char* read_header(char* str)
{
	return strtok(str, ";");
}

bool read_vector(char* str, Vector &v)
{
	char *buf;
	buf = strtok(str, ";\r\n");
	if (buf == NULL) 
	{
		return false;
	}
	v.x = atof(buf);
	v.y = atof(strtok(NULL, ";"));
	v.z = atof(strtok(NULL, ";"));

	return true;
}
void read_beacon_info(FILE* fp, BeaconInfo * beaconList, int nBeacon)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];
	for (int i = 0; i < nBeacon; i++)
	{
		fgets(buf, buf_size, fp);
		int bid = (int) read_value(buf);
		beaconList[bid].beacon_id = bid;
		read_vector(NULL, beaconList[bid].location);
	}
}
long read_listener_info(FILE* fp, Vector &vPosition, Vector &vFace)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];
	char *buf_header;

	if (fgets(buf, buf_size, fp) == NULL)
	{
		printf("incorrect file contents (exit codee :15)\n");
		getchar();
		exit(15);
	}
	buf_header = read_header(buf);
	
//	if (fgets(buf, buf_size, fp) == NULL) return -1;
	if (strcmp(buf_header, "endlog") == 0) return -1;
	if (strcmp(buf_header, "time") != 0)
	{
		printf("incorrect file contents (exit code : 10)\n");
		getchar();
		exit(10);
	}
	
	long timestamp;
	bool ret1, ret2;

	timestamp = read_value_long(NULL);
	
	ret1 = read_vector(NULL, vPosition);
	ret2 = read_vector(NULL, vFace);

	if (ret1 && ret2 == false)
	{
		printf("incorrect file contents (exit code : 11\n");
		getchar();
		exit(11);
	}

	return timestamp;
}
int read_beacon_distance(FILE* fp, BeaconInfo *beaconList)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];
	
	for (int i = 0; i < MAX_BID; i++)
	{
		beaconList[i].distance = 0;
	}
	while(fgets(buf, buf_size, fp), strcmp(read_header(buf), "dist") == 0)
	{
		int bid = (int) read_value(NULL);				
		beaconList[bid].distance = read_value(NULL);
		read_vector(NULL, beaconList[bid].vTemp);
		read_vector(NULL, beaconList[bid].vTemp2);
	}	

	return true;
}

int read_reflection_cnt(FILE *fp, int* ref_cnt1, int *ref_cnt2)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];

	fgets(buf, buf_size, fp);
	if (strcmp(read_header(buf), "ref1") != 0)
	{
		printf("incorrect file contents : no reflection1 info\n");
		getchar();
		exit(12);
	}
	*ref_cnt1 = (int)read_value(NULL);
	fgets(buf, buf_size, fp);
	if (strcmp(read_header(buf), "ref2") != 0)
	{
		printf("incorrect file contents : no reflection2 info\n");
		getchar();
		exit(13);
	}
	*ref_cnt2 = (int)read_value(NULL);

	return true;

	
}

LogInfo get_log_info(Vector vPosition, Vector vFace, BeaconInfo* beaconList, 
					 long timestamp, int nBeacon)
{
	int n = 0;
	LogInfo ret;
	ret.vPosition = vPosition;
	ret.vFace = vFace;
	ret.beaconList = (BeaconInfo*) malloc(sizeof(BeaconInfo) * nBeacon);	
	ret.timestamp = timestamp;
	
	for (int i = 0; i < MAX_BID; i++)
	{
		if (beaconList[i].beacon_id < 0) continue;

		ret.beaconList[n] = beaconList[i];
		if (n ++ == nBeacon) break;
	}
	return ret;
}

bool LoadLog(const char* filename, int *ref_cnt1, int *ref_cnt2)
{
	FILE *fp;
	int width, length, height;

	fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("file %s open error\n", filename);
		return false;
	}

/*
	errno_t ret = fopen_s(&fp, filename, "r");
	if (ret != 0)
	{		
//		printf("file %s open error\n");
		return false;		
	}
*/
	clear_log_list(logList);
	
	

	const size_t buf_size = 4096;
	char buf[buf_size + 1];
	fgets(buf, buf_size, fp);
	l_width = width = (int) read_value(buf);
	fgets(buf, buf_size, fp);
	l_length = length = (int) read_value(buf);
	fgets(buf, buf_size, fp);
	l_height = height = (int) read_value(buf);
	fgets(buf, buf_size, fp);
	nBeacon = (int) read_value(buf);

	BeaconInfo beaconList[MAX_BID];
	Vector vPosition;
	Vector vFace;
	long timestamp;

	read_beacon_info(fp, beaconList, nBeacon);

	while((timestamp = read_listener_info(fp, vPosition, vFace)) > 0)
	{
		read_beacon_distance(fp, beaconList);
		logList.push_back(get_log_info(vPosition, vFace, beaconList, timestamp, nBeacon));
	}

	read_reflection_cnt(fp, ref_cnt1, ref_cnt2);

	fclose(fp);
	
	return true;
}

void SetPlane(Vector p1, Vector p2, Vector p3, Vector p4)
{
	Plane plane = Plane(p1, p2, p3, p4);
	plane.setBoundary(true);
	planes.addPlane(plane);
}

void SetPlanes(int width, int length, int height)
{
		
	double cx = width / 2.0;
	double cy = length / 2.0;
	double cz = height;

	Vector v1 = Vector(-cx,  cy, cz);
	Vector v2 = Vector( cx,  cy, cz);
	Vector v3 = Vector(-cx, -cy, cz);
	Vector v4 = Vector( cx, -cy, cz);
	Vector v5 = Vector(-cx,  cy, 0);
	Vector v6 = Vector( cx,  cy, 0);
	Vector v7 = Vector(-cx, -cy, 0);
	Vector v8 = Vector( cx, -cy, 0);

	SetPlane(v1, v2, v6, v5);		// front plane
	SetPlane(v2, v4, v8, v6);		// right side plane
	SetPlane(v4, v3, v7, v8);		// back plane
	SetPlane(v3, v1, v5, v7);		// left plane
	SetPlane(v3, v4, v2, v1);		// ceiling
	SetPlane(v5, v6, v8, v7);		// floor
	
}



int main()
{
	int ref_cnt1, ref_cnt2;
	LoadLog("test_log.dat", &ref_cnt1, &ref_cnt2);

	BeaconList beacons;
	Estimator estimator;

	SetPlanes(1000, 1000, 300);	
	
	double err1 = 0;
	
	for (int i = 0; i < nBeacon; i++)
	{
		beacons.addBeacon(logList[0].beaconList[i].beacon_id,
				logList[0].beaconList[i].location);
	}
	beacons.applyPlanes(&planes);



	EstimatorArgument args;
	args.timeSlot = 50;
	args.validSize = 4;
	args.maxMeasError = 50;
	args.minValidDistance = 10;
	args.maxValidDistance = 700;
	args.minBeaconSize = 3;
	args.strictValidSize = false;
	args.timeWindow = 1000;
	args.optimization = OPT::SELECTION | OPT::THRESHOLD | OPT::BRANCHCUT;
	args.kfMode = KF::PV;
	args.estimatorMode = EST::PROPOSED1;
	args.gatherData = false;
	args.beacons = &beacons;
	args.planes = &planes;





	estimator.setEstimator(args);

	for (size_t i = 0; i < logList.size(); i++)
	{
		long timestamp = logList[i].timestamp;

		for (int j = 0; j < nBeacon; j++)
		{
			estimator.measure(timestamp, 
					logList[i].beaconList[j].beacon_id,
					logList[i].beaconList[j].distance);
		}
		EstimatorResult result;
		result = estimator.solve(timestamp);

		Vector ori_point = logList[i].vPosition;
		Vector slv_point1 = result.location;
		
		err1 += slv_point1.getDistance(ori_point);

		fprintf(stdout, "%d\t%f\t%f\n", (int)i, err1/(double)(i + 1), 0.0);

		slv_point1.println();
//		getchar();
	}

	return 0;
}


