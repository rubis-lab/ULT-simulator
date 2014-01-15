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


unsigned long long getTimeUS()
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
	estimatorTotal = TimeRecorder("Estimator_Total_Time");
	estimatorSetup = TimeRecorder("Estimator_Setup_Time");
	estimatorSolving = TimeRecorder("Estimator_Solving_Time");
	estimatorPostProc = TimeRecorder("Estimator_Filtering_Process_Time");
	estimatorKFProc = TimeRecorder("Estimator_KF_Process_Time");
	solverSolving = TimeRecorder("Solver_PMSs_Solving_Time");
	N_PMS = ValueRecorder("Number_of_PMSs");
	N_PMSFiltered = ValueRecorder("Number_of_Filtered_PMSs");
	N_selectionFail = ValueRecorder("Number_of_Selection_Fail_Timesteps");
	N_reflection = ValueRecorder("Number_of_Reflections");
	N_selected = ValueRecorder("Number_of_Selected_Beacons");	// actual valid beacons. always equal or smaller than N_V	
	estimationError = ValueRecorder("Estimation_Error");
	recorderList.push_back(&estimatorTotal);
	recorderList.push_back(&estimatorSetup);
	recorderList.push_back(&estimatorSolving);
	recorderList.push_back(&estimatorPostProc);
	recorderList.push_back(&estimatorKFProc);
	recorderList.push_back(&solverSolving);
	recorderList.push_back(&N_PMS);
	recorderList.push_back(&N_PMSFiltered);
	recorderList.push_back(&N_selectionFail);
	recorderList.push_back(&N_reflection);
	recorderList.push_back(&N_selected);
	recorderList.push_back(&estimationError);
	reset();
}


Analyzer::~Analyzer(void)
{
}

void Analyzer::reset()
{
	for (size_t i = 0; i < recorderList.size(); i ++)
	{
		recorderList[i]->reset();
	}	
}

void Analyzer::tickTimeSlot()
{
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->commit();
	}
}

void Analyzer::writeCDF()
{
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->writeCDF();
	}
}

void Analyzer::printAVG(FILE *fp)
{
	for (size_t i = 0; i < recorderList.size(); i ++)
	{
		recorderList[i]->printAVG(fp);
	}
}

/**********************************************
TimeRecorder Memebers
**********************************************/
TimeRecorder::TimeRecorder(const char *name):Recorder<unsigned long long>(name)
{
	reset();	
}

TimeRecorder::~TimeRecorder()
{
}

void TimeRecorder::reset()
{
	Recorder<unsigned long long>::reset();
	start_time = 0;
	execution_time = 0;
	current_value = 0;
}

void TimeRecorder::startTimer()
{
	start_time = getTimeUS();
}

unsigned long long TimeRecorder::stopTimer()
{
	execution_time = getTimeUS() - start_time;
	current_value += execution_time;
	return execution_time;
}

void TimeRecorder::commit()
{
	Recorder<unsigned long long>::commit();
	current_value = 0;
}

/**********************************************
ValueRecorder Memebers
**********************************************/
ValueRecorder::ValueRecorder(const char *name):Recorder<double>(name)
{
	reset();
}

ValueRecorder::~ValueRecorder()
{
}

void ValueRecorder::reset()
{
	Recorder<double>::reset();
	current_value = 0.0;
}

void ValueRecorder::addValue(double val)
{
	current_value += val;
}

void ValueRecorder::commit()
{
	Recorder<double>::commit();
	current_value = 0.0;	
}

/**********************************************
Recorder Memebers
**********************************************/
template <class T> 
void Recorder<T>::reset()
{
	recorded_value.clear();	
}

template <class T>
double Recorder<T>::getAvg()
{
	return getSum()/(double)recorded_value.size();
}

template <class T>
void Recorder<T>::commit()
{
	recorded_value.push_back(current_value);
}


template <class T>
void Recorder<T>::writeCDF()
{
	char filename[255];
	sprintf(filename, "%s.txt", getName());
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
void Recorder<T>::printAVG(FILE *fp)
{
	fprintf(fp, "%s : %f\n", getName(), getAvg());
}


template <class T>
double Recorder<T>::getSum()
{
	double sum = 0;
	for (size_t i = 0; i < recorded_value.size(); i++)
	{
		sum += recorded_value[i];
	}
	return sum;
}

template <class T>
int Recorder<T>::getSize()
{
	return recorded_value.size();
}
