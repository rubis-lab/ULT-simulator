#include <algorithm>
#include <iostream>
#include <fstream>
#include "Analyzer.h"

#ifndef ENV_LINUX
int gettimeofday(struct timeval* tp, int* tz)
{
	tz = NULL;
	LARGE_INTEGER tickNow;
	static LARGE_INTEGER tickFrequency;
	static BOOL tickFrequencySet = FALSE;
	if (tickFrequencySet == FALSE) {
		QueryPerformanceFrequency(&tickFrequency);
		tickFrequencySet = TRUE;
	}
	QueryPerformanceCounter(&tickNow);
	tp->tv_sec = (long) (tickNow.QuadPart / tickFrequency.QuadPart);
	tp->tv_usec = (long) (((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);
	
	return 0;
}
#endif


unsigned long long GetTimeUS()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)(tv.tv_sec) * 1000000L + tv.tv_usec);
}

/**********************************************
Analyzer Memebers
**********************************************/
Analyzer::Analyzer(void)
{
	EstimatorTotal = TimeRecorder("Estimator_Total_Time");
	EstimatorSetup = TimeRecorder("Estimator_Setup_Time");
	EstimatorSolving = TimeRecorder("Estimator_Solving_Time");
	EstimatorPostProc = TimeRecorder("Estimator_Filtering_Process_Time");
	EstimatorKFProc = TimeRecorder("Estimator_KF_Process_Time");
	SolverSolving = TimeRecorder("Solver_PMSs_Solving_Time");
	N_PMS = ValueRecorder("Number_of_PMSs");
	N_PMSFiltered = ValueRecorder("Number_of_Filtered_PMSs");
	N_SelectionFail = ValueRecorder("Number_of_Selection_Fail_Timesteps");
	N_Reflection = ValueRecorder("Number_of_Reflections");
	N_Selected = ValueRecorder("Number_of_Selected_Beacons");	// actual valid beacons. always equal or smaller than N_V	
	EstimationError = ValueRecorder("Estimation_Error");
	recorderList.push_back(&EstimatorTotal);
	recorderList.push_back(&EstimatorSetup);
	recorderList.push_back(&EstimatorSolving);
	recorderList.push_back(&EstimatorPostProc);
	recorderList.push_back(&EstimatorKFProc);
	recorderList.push_back(&SolverSolving);
	recorderList.push_back(&N_PMS);
	recorderList.push_back(&N_PMSFiltered);
	recorderList.push_back(&N_SelectionFail);
	recorderList.push_back(&N_Reflection);
	recorderList.push_back(&N_Selected);
	recorderList.push_back(&EstimationError);
	Reset();
}


Analyzer::~Analyzer(void)
{
}

void Analyzer::Reset()
{
	for (size_t i = 0; i < recorderList.size(); i ++)
	{
		recorderList[i]->Reset();
	}	
}

void Analyzer::TickTimeSlot()
{
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->Commit();
	}
}

void Analyzer::WriteCDF()
{
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->WriteCDF();
	}
}

void Analyzer::PrintAVG(FILE *fp)
{
	for (size_t i = 0; i < recorderList.size(); i ++)
	{
		recorderList[i]->PrintAVG(fp);
	}
}

/**********************************************
TimeRecorder Memebers
**********************************************/
TimeRecorder::TimeRecorder(const char *name):Recorder<unsigned long long>(name)
{
	Reset();	
}

TimeRecorder::~TimeRecorder()
{
}

void TimeRecorder::Reset()
{
	Recorder<unsigned long long>::Reset();
	start_time = 0;
	execution_time = 0;
	current_value = 0;
}

void TimeRecorder::StartTimer()
{
	start_time = GetTimeUS();
}

unsigned long long TimeRecorder::StopTimer()
{
	execution_time = GetTimeUS() - start_time;
	current_value += execution_time;
	return execution_time;
}

void TimeRecorder::Commit()
{
	Recorder<unsigned long long>::Commit();
	current_value = 0;
}

/**********************************************
ValueRecorder Memebers
**********************************************/
ValueRecorder::ValueRecorder(const char *name):Recorder<double>(name)
{
	Reset();
}

ValueRecorder::~ValueRecorder()
{
}

void ValueRecorder::Reset()
{
	Recorder<double>::Reset();
	current_value = 0.0;
}

void ValueRecorder::AddValue(double val)
{
	current_value += val;
}

void ValueRecorder::Commit()
{
	Recorder<double>::Commit();
	current_value = 0.0;	
}

/**********************************************
Recorder Memebers
**********************************************/
template <class T> 
void Recorder<T>::Reset()
{
	recorded_value.clear();	
}

template <class T>
double Recorder<T>::GetAvg()
{
	return GetSum()/(double)recorded_value.size();
}

template <class T>
void Recorder<T>::Commit()
{
	recorded_value.push_back(current_value);
}


template <class T>
void Recorder<T>::WriteCDF()
{
	char filename[255];
	sprintf(filename, "%s.txt", GetName());
	std::ofstream fout;
	fout.open(filename);
	if (fout == NULL)
	{
		printf("Cannot open file %s\n", filename);
		getchar();
		exit(100);		
	}
	std::sort(recorded_value.begin(),recorded_value.end());

	size_t size = recorded_value.size();
	for (size_t i = 0; i < size; i++)
	{
		fout << recorded_value[i] << "\t" << (double)i / (double)size << std::endl;
	}
	fout.close();	
}

template <class T> 
void Recorder<T>::PrintAVG(FILE *fp)
{
	fprintf(fp, "%s : %f\n", GetName(), GetAvg());
}


template <class T>
double Recorder<T>::GetSum()
{
	double sum = 0;
	for (size_t i = 0; i < recorded_value.size(); i++)
	{
		sum += recorded_value[i];
	}
	return sum;
}

template <class T>
int Recorder<T>::GetSize()
{
	return recorded_value.size();
}