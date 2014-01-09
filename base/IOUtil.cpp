#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IOUtil.h"



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
	buf = strtok(str, ";#\r\n");
	if (buf == NULL) 
	{
		v = Vector(false);
		return false;
	}
	double x, y, z;

	x = atof(buf);
	y = atof(strtok(NULL, ";"));
	z = atof(strtok(NULL, ";"));
	
	v = Vector(x, y, z);

	return true;
}


bool read_listener_info(FILE* fp, Vector &vPosition, Vector &vFace, unsigned long *timestamp)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];
	char *buf_header;

	if (fgets(buf, buf_size, fp) == NULL)
	{
		printf("incorrect file contents (exit codee :15)\n");
		getchar();
		exit(10);
	}
	buf_header = read_header(buf);
	
//	if (fgets(buf, buf_size, fp) == NULL) return -1;
	if (strcmp(buf_header, "endlog") == 0) 
		return false;
	if (strcmp(buf_header, "time") != 0)
	{
		printf("incorrect file contents. \"time\" is expected but read %s (exit code : 10)\n", buf_header);
		getchar();
		exit(11);
	}
	
	bool ret1, ret2;

	*timestamp = read_value_long(NULL);
	
	ret1 = read_vector(NULL, vPosition);
	ret2 = read_vector(NULL, vFace);

	if (ret1 && ret2 == false)
	{
		printf("incorrect file contents (exit code : 11\n");
		getchar();
		exit(12);
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
		exit(13);
	}
	*ref_cnt1 = (int)read_value(NULL);
	fgets(buf, buf_size, fp);
	if (strcmp(read_header(buf), "ref2") != 0)
	{
		printf("incorrect file contents : no reflection2 info\n");
		getchar();
		exit(14);
	}
	*ref_cnt2 = (int)read_value(NULL);

	return true;

	
}
