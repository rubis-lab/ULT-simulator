#include <string.h>
#include "LogRecorder.h"


void clear_log_list(std::vector <LogInfo> &logList)
{
	for (unsigned int i = 0; i < logList.size(); i++)
	{
		free(logList[i].beaconList);
	}
	logList.clear();
}





LogRecorder::LogRecorder(unsigned long randomseed)
{
	blLoadFlag = false;
	nBeacon = 0;
	this->randomRoom = new Random(randomseed);
	this->randomListener = new Random(randomseed);
}

LogRecorder::~LogRecorder(void)
{
	clear_log_list(logList);
	delete randomRoom;
	delete randomListener;
}

LogRecorder::LogRecorder()
{
	blLoadFlag = false;
	nBeacon = 0;
	randomRoom = new Random(0);
	randomListener = new Random(0);
}



bool LogRecorder::CreateLog(char* filename, Argument& args, unsigned long seed)
{
//	if (seed == 0) seed = (unsigned long)time(NULL);

	delete randomRoom;
	delete randomListener;
	randomRoom = new Random(seed);
	randomListener = new Random(seed);

	return CreateLog(filename, args);
}

Vector LogRecorder::getPoint(Vector vRange, Vector vOffset)
{
	Vector vPos;
	vPos.x = randomListener->GetUniformDist() * vRange.x + vOffset.x;
	vPos.y = randomListener->GetUniformDist() * vRange.y + vOffset.y;
	vPos.z = randomListener->GetUniformDist() * vRange.z + vOffset.z;

	return vPos;
}


Vector LogRecorder::getPointRND()
{
	Vector vRange, vOffset;
	vRange = Vector(args.RoomWidth - margin * 2, args.RoomLength - margin * 2,args.RoomHeight / 2);
	vOffset = Vector(margin - args.RoomWidth / 2, margin - args.RoomLength / 2, args.RoomHeight / 4);

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

int LogRecorder::getArea(Vector vPos)
{
	int x_border = (args.RoomWidth / 2) - (args.RoomWidth / 8) -  margin;
	int y_border = (args.RoomLength / 2) - (args.RoomLength / 8) -  margin;

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

Vector LogRecorder::getPointCW(Vector vPos)
{
	Vector vRange, vOffset;

	vRange.z = args.RoomHeight / 2;
	vOffset.z = args.RoomHeight / 4;
	int area = getArea(vPos);
	switch (area)
	{
	case 0:
	case 1:
		vRange.x = hallway_x;
		vRange.y = vPos.y + args.RoomLength / 2 - margin;
		vOffset.x = -args.RoomWidth/2 + margin;
		vOffset.y = -args.RoomLength / 2 + margin;
		break;
	case 2:
	case 3:
		vRange.x = -vPos.x + args.RoomWidth / 2 - margin;
		vRange.y = hallway_y;
		vOffset.x = vPos.x;
		vOffset.y = - args.RoomLength / 2 + margin;
		break;	
	case 4:
	case 5:
		vRange.x = hallway_x;
		vRange.y = - vPos.y + args.RoomLength / 2 -margin ;
		vOffset.x = args.RoomWidth / 2 - margin - hallway_x;
		vOffset.y = vPos.y;			
		break;
	case 6:
	case 7:
		vRange.x = vPos.x + args.RoomWidth/2 - margin;
		vRange.y = hallway_y;
		vOffset.x = - args.RoomWidth / 2 + margin;
		vOffset.y = args.RoomLength /2 - margin - hallway_y;
		break;
	}
	return getPoint(vRange, vOffset);
}

Vector LogRecorder::getPointACW(Vector vPos)
{
	Vector vRange, vOffset;

	vRange.z = args.RoomHeight / 2;
	vOffset.z = args.RoomHeight / 4;
	int area = getArea(vPos);
	switch (area)
	{
	case 0:
	case 7:
		vRange.x = -vPos.x + args.RoomWidth / 2 - margin;
		vRange.y = hallway_y;
		vOffset.x = vPos.x;
		vOffset.y = args.RoomLength /2 - margin - hallway_y;
		break;
	case 5:
	case 6:
		vRange.x = hallway_x;
		vRange.y = vPos.y + args.RoomLength / 2 - margin;
		vOffset.x = args.RoomWidth / 2 - margin - hallway_x;
		vOffset.y = -args.RoomLength / 2 + margin;
		break;	
	case 3:
	case 4:
		vRange.x = vPos.x + args.RoomWidth/2 - margin;
		vRange.y = hallway_y;
		vOffset.x = - args.RoomWidth / 2 + margin;
		vOffset.y = - args.RoomLength / 2 + margin;
		break;
	case 1:
	case 2:
		vRange.x = hallway_x;
		vRange.y = - vPos.y + args.RoomLength / 2 -margin ;
		vOffset.x = -args.RoomWidth/2 + margin;
		vOffset.y = vPos.y;			
		break;
	}
	return getPoint(vRange, vOffset);
}



bool LogRecorder::CreateLog(char* filename, Argument& _args)
{
	args = _args;
	FILE* fp;

	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("file open error\n");
		return false;
	}
/*	
	errno_t ret = fopen_s(&fp, filename, "w");

	if (ret != 0) 
	{
		printf("file open error \n");
		return false;
	}
*/
	fprintf(fp, "%d; # width of test room\n", args.RoomWidth);
	fprintf(fp, "%d; # length of test room\n", args.RoomLength);
	fprintf(fp, "%d; # height of test room\n", args.RoomHeight);
	fprintf(fp, "%d; # number of beacons\n", args.SIM_NBeacon);


//	Random random;
//	Random random(1);
//	Random random(1276108085);
	
	LocationEstimator estimator(args);
	TestRoom testRoom(&estimator, randomRoom, args);
	ListenerMover listenerMover(randomListener, args);
	
//	testRoom.SetRandomBeacon(args.cNBeacon);
//	testRoom.CalculateBeaconReflection();
//	testRoom.SetError(args.cEstimationErr, args.cEstimationDev);

	for (int i = 0; i < estimator.beaconListSize; i++)
	{
		if (estimator.beaconList[i] == NULL) continue;
		Vector vBeacon = estimator.beaconList[i]->location;
		fprintf(fp, "%4d;%4d; %4d; %4d; # location of Beacon %d\n", 
			i, (int)vBeacon.x, (int)vBeacon.y, (int)vBeacon.z, i);

	}

	int ref_cnt1 = 0;
	int ref_cnt2 = 0;

	
//	margin = 150;		// for complex room
	margin = 50;
	hallway_x = args.RoomWidth / 16;
	hallway_y = args.RoomWidth / 16;

	Vector vPos = Vector(-args.RoomWidth / 2 + margin, args.RoomLength / 2 - margin,0);
	for (int i = 0; i<5000 ; i++)
	{		
		double r_speed;
		while ((r_speed = randomListener->GetGaussDist(args.SIM_SpeedAvg, args.SIM_SpeedDev)) <= 0);

		
		
		switch (args.SIM_MoveType)
		{
		default:
		case SIM::RND: vPos = getPointRND();	break;
		case SIM::CW: vPos = getPointCW(vPos); break;
		case SIM::PD_CCW: 	// ignore in this case
		case SIM::CCW: vPos = getPointACW(vPos); break;
		}
		listenerMover.SetPath(vPos, r_speed);

	
	}

	PDListener pdL(args);	// this is special listener for plane detection
	long timestamp;
	do
	{
		timestamp = listenerMover.MoveNext();		
		printf("%ld/%ld (%3d%%)\r", timestamp, args.SIM_TimeLimit, (int)((double)timestamp/(double)args.SIM_TimeLimit*100));

		if (timestamp < 0) break;

		Vector vLPosition = listenerMover.GetListenerPosition();
		Vector vLFace = listenerMover.GetListenerFace();

		/* overide plane detection listener mover when PD_CCW*/
		if (args.SIM_MoveType == SIM::PD_CCW)
		{
			vLPosition = pdL.GetListenerPosition();
			vLFace = pdL.GetListenrFace();
		}

		fprintf(fp, "time;%10ld;%4d;%4d;%4d;", timestamp, 
													(int) vLPosition.x, 
													(int) vLPosition.y, 
													(int) vLPosition.z);
		fprintf(fp, "%.4f;%.4f;%.4f;\n", vLFace.x, vLFace.y, vLFace.z);
		testRoom.SetListener(vLPosition, vLFace, timestamp);
		for (int i = 0; i < testRoom.estimator->beaconListSize; i++)
		{
			if (testRoom.minReflect[i] == NULL) continue;
			fprintf(fp, "dist;%4d;%4.2f;", i, testRoom.minReflect[i]->distance);

			for (int j = 0; j < testRoom.minReflect[i]->n_plane; j ++)
			{
				if (j == 0) ref_cnt1 ++;
				if (j == 1) ref_cnt2 ++;
				fprintf(fp, "%4d;%4d;%4d;", 
					(int) testRoom.minReflect[i]->vCross[j].x,
					(int) testRoom.minReflect[i]->vCross[j].y,
					(int) testRoom.minReflect[i]->vCross[j].z);
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "end;\n");
		if (args.SIM_MoveType == SIM::PD_CCW && !pdL.MoveNext()) break;
	}while (timestamp <= args.SIM_TimeLimit);
	printf("\n");
	fprintf(fp, "endlog;\n");
	fprintf(fp, "ref1;%d;\n", ref_cnt1);
	fprintf(fp, "ref2;%d;\n", ref_cnt2);
	fclose(fp);

	return true;


	/* log file format
	"time"; timestamp ; Lx;Ly;Lz; Fx;Fy;Fz;
	"dist"; beaconid  ; distance; reflected point x;y;z;
	"dist"; beaconid  ; distance;
		      ....
	"end" ;
	"time"; .....
	*/
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
		v.isValid = false;
		return false;
	}
	v.isValid = true;
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



bool LogRecorder::LoadLog(char* filename, int *ref_cnt1, int *ref_cnt2)
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
	blLoadFlag = true;
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



//	int main()
//	//int main(int argc, char** argv)
//	{	
//	//	create_simulation_log("test.txt", 1000, 1000 ,1000);
//		load_simulation_log("test.txt");
//		return 0;
//	/*	
//		if (argc < 3)
//		{
//			printf("Usage : SimulationTracer [create | load] file_name\n");
//			exit(0);
//		}
//
//		if (strcmp(argv[1], "create") == 0)
//		{
//			create_simulation_log(argv[2], 1000, 1000 ,1000);
//			return 0;
//		}
//	*/
//
//
//
//		Random random;
//		LocationEstimator estimator;	
//		TestRoom testRoom(&estimator, &random, width, length, height);
//		ListenerMover listenerMover(&random, width, length, height);
//
//	/*  Add Beacon Position Module 
//	do
//		testRoom.SetBeacon()
//	loop
//	*/
//		testRoom.CalculateBeaconReflection();
//		
//	/* Add Listener Path Module
//	do
//		listenerMover.SetPath()
//	loop
//	*/
//
//	/* Apply Listener Path to TestRoom
//	do
//		load listenerMover path 
//		listenerMover.MoveNext()
//		testRoom.setListner(listenerMover.location, listenerMover.direction, timestamp)
//		testRoom.SetHeardDistance(double err_mean, double err_dev);
//		estimator.solve();
//		LogRecoder(estimator.positionList, listenerMover.actual Position)
//	while()
//	*/
	
	
	
