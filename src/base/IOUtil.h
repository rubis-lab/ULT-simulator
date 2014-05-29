#pragma once
#include "Vector.h"

#ifdef ENV_LINUX
#define _strdup strdup
#endif

double read_value(char* str);
long read_value_long(char* str);
char* read_header(char* str);
bool read_vector(char* str, Vector &v);
bool read_listener_info(FILE* fp, Vector &vPosition, Vector &vFace, unsigned long *timestamp);
int read_reflection_cnt(FILE *fp, int* ref_cnt1, int *ref_cnt2);
