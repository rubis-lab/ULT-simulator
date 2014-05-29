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
	estimatorTotal = TimeRecorder("A01_Estimator_Total_Time");
	estimatorSetup = TimeRecorder("A02_Estimator_Setup_Time");
	estimatorSolving = TimeRecorder("A03_Estimator_Solving_Time");
	estimatorPostProc = TimeRecorder("A04_Estimator_Filtering_Process_Time");
	estimatorKFProc = TimeRecorder("A05_Estimator_KF_Process_Time");
	solverSolving = TimeRecorder("A06_Solver_PMSs_Solving_Time");
	N_PMS = ValueRecorder("B01_Number_of_PMSs");
	N_PMSFiltered = ValueRecorder("B02_Number_of_Filtered_PMSs");
	N_PMS_KF = ValueRecorder("B03_Number_of_PMSs_to_KF");
	N_selectionFail = ValueRecorder("C01_Number_of_Selection_Fail_Timesteps");
	N_opt1Fail = ValueRecorder("C02_Number_of_opt1_Fail");
	N_optThresholdFail = ValueRecorder("C03_Number_of_Threshold_opt_Fail");
	N_kalmanFilterFail = ValueRecorder("C04_Number_of_Kalman_Filter_Fail_Timesteps");
	N_reflection = ValueRecorder("D01_Number_of_Reflections(sim)");
	N_reflection2 = ValueRecorder("D02_Number_of_Reflections_Twice(sim)");
	N_selected = ValueRecorder("E01_Number_of_Selected_Beacons");	// actual valid beacons. always equal or smaller than N_V	
	N_fail = ValueRecorder("F01_Number_of_Failed_Timesteps");
	N_tick = ValueRecorder("F02_Number_of_Timesteps");
	estimationError = ValueRecorder("G01_Estimation_Error(sim)");
	estimationError2 = ValueRecorder("G02_Estimation_Error_Reference(sim)");
	estimationErrorExcFail = ValueRecorder("G03_Estimation_Error_Except_Fail");
	estimationErrorExcFail2 = ValueRecorder("G04_Estimation_Error_Except_Fail_Ref");
	estimationErrorExcStrictFail = ValueRecorder("G05_Estimation_Error_Except_Strict_Fail");
	estimationErrorExcStrictFail2 = ValueRecorder("G06_Estimation_Error_Except_Strict_Fail_Ref");
	N_receptionFail = ValueRecorder("H01_Reception_Fails(<3)");
	N_receptionFail2 = ValueRecorder("H02_Reception_Fails_Ref.(<3)");
	N_receptionFailStrict = ValueRecorder("H03_Strict_Reception_Fails(<_NV)");
	N_receptionFailStrict2 = ValueRecorder("H04_Strict_Reception_Fails_Ref.(<_NV)");

	recorderList.push_back(&estimatorTotal);
	recorderList.push_back(&estimatorSetup);
	recorderList.push_back(&estimatorSolving);
	recorderList.push_back(&estimatorPostProc);
	recorderList.push_back(&estimatorKFProc);
	recorderList.push_back(&solverSolving);
	recorderList.push_back(&N_PMS);
	recorderList.push_back(&N_PMSFiltered);
	recorderList.push_back(&N_PMS_KF);
	recorderList.push_back(&N_selectionFail);
	recorderList.push_back(&N_opt1Fail);
	recorderList.push_back(&N_optThresholdFail);
	recorderList.push_back(&N_kalmanFilterFail);
	recorderList.push_back(&N_reflection);
	recorderList.push_back(&N_reflection2);
	recorderList.push_back(&N_selected);
	recorderList.push_back(&N_fail);
	recorderList.push_back(&N_tick);
	recorderList.push_back(&estimationError);
	recorderList.push_back(&estimationErrorExcFail);
	recorderList.push_back(&estimationErrorExcStrictFail);
	recorderList.push_back(&N_receptionFail);
	recorderList.push_back(&N_receptionFailStrict);
	recorderList.push_back(&estimationError2);
	recorderList.push_back(&N_receptionFail2);
	recorderList.push_back(&N_receptionFailStrict2);
	recorderList.push_back(&estimationErrorExcFail2);
	recorderList.push_back(&estimationErrorExcStrictFail2);
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
// this function is obsoleted
#if 0
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->writeCDF();
	}
#endif
}

void Analyzer::printAVG(FILE *fp)
{
	double nTick = N_tick.getTotal();
	double nFail = N_fail.getTotal();
	double nSuccess = nTick - nFail;
	double nReceptionFail = N_receptionFail.getTotal();
	double nReceptionFail2 = N_receptionFail2.getTotal();
	double nReceptionFailStrict = N_receptionFailStrict.getTotal();
	double nReceptionFailStrict2 = N_receptionFailStrict2.getTotal();
	double nReceptionSuccess = nTick - nReceptionFail;
	double nReceptionSuccess2 = nTick - nReceptionFail2;
	double nReceptionSuccessStrict = nTick - nReceptionFailStrict;
	double nReceptionSuccessStrict2 = nTick - nReceptionFailStrict2;

	estimatorTotal.printAVG(fp, nSuccess);
	estimatorSetup.printAVG(fp, nSuccess);
	estimatorSolving.printAVG(fp, nSuccess);
	estimatorPostProc.printAVG(fp, nSuccess);
	estimatorKFProc.printAVG(fp, nSuccess);
	solverSolving.printAVG(fp, nSuccess);

	N_PMS.printAVG(fp, nSuccess);
	N_PMSFiltered.printAVG(fp, nSuccess);
	N_PMS_KF.printAVG(fp, nSuccess);
	N_selectionFail.printAVG(fp, nSuccess);
	N_opt1Fail.printAVG(fp, nSuccess);
	N_optThresholdFail.printAVG(fp, nSuccess);
	N_kalmanFilterFail.printAVG(fp, nSuccess);
	N_reflection.printAVG(fp, nSuccess);
	N_reflection2.printAVG(fp, nSuccess);
	N_selected.printAVG(fp, nSuccess);
	N_fail.printAVG(fp, nTick);
	N_tick.printAVG(fp, 1.0);

	estimationError.printAVG(fp, nTick);
	estimationError2.printAVG(fp, nTick);
	estimationErrorExcFail.printAVG(fp, nReceptionSuccess);
	estimationErrorExcFail2.printAVG(fp, nReceptionSuccess2);
	estimationErrorExcStrictFail.printAVG(fp, nReceptionSuccessStrict);
	estimationErrorExcStrictFail2.printAVG(fp, nReceptionSuccessStrict2);
	N_receptionFail.printAVG(fp, nTick);
	N_receptionFail2.printAVG(fp, nTick);
	N_receptionFailStrict.printAVG(fp, nTick);
	N_receptionFailStrict2.printAVG(fp, nTick);
}

void Analyzer::addAnalyzer(const Analyzer &analyzer)
{
	for (size_t i = 0; i < recorderList.size(); i++)
	{
		recorderList[i]->setTotal(recorderList[i]->getTotal() + analyzer.recorderList[i]->getTotal());
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
	total_value = 0;
}

void TimeRecorder::startTimer()
{
	start_time = getTimeUS();
}

unsigned long long TimeRecorder::stopTimer()
{
	execution_time = getTimeUS() - start_time;
	current_value += execution_time;
	total_value += execution_time;
	return execution_time;
}

void TimeRecorder::commit()
{
	Recorder<unsigned long long>::commit();
	current_value = 0;
}

void TimeRecorder::discard()
{
	current_value = 0;
}

double TimeRecorder::getTotal()
{
	return (double)total_value;
}

void TimeRecorder::setTotal(double total)
{
	total_value = (unsigned long long) total;

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
	total_value = 0.0;
}

void ValueRecorder::addValue(double val)
{
	current_value += val;
	total_value += val;
}

void ValueRecorder::commit()
{
	Recorder<double>::commit();
	current_value = 0.0;	
}

void ValueRecorder::discard()
{
	current_value = 0.0;
}

double ValueRecorder::getTotal()
{
	return total_value;
}

void ValueRecorder::setTotal(double total)
{
	total_value = total;
}


/**********************************************
Recorder Memebers
**********************************************/
template <class T> 
void Recorder<T>::reset()
{
	recorded_value.clear();	
}

// this function is obsoleted
#if 0
template <class T>
double Recorder<T>::getAvg()
{
	return getSum()/(double)recorded_value.size();
}
#endif

template <class T>
void Recorder<T>::commit()
{
	recorded_value.push_back(current_value);
}

template <class T>
void Recorder<T>::discard()
{
}


//this function is obsoleted
#if 0
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
#endif

// this function is obsoleted
#if 0
template <class T> 
void Recorder<T>::printAVG(FILE *fp)
{
	fprintf(fp, "%s : %f\n", getName(), getAvg());
}
#endif

template <class T>
void Recorder<T>::printAVG(FILE *fp, double count)
{
	fprintf(fp, "%-50s : %f\n", getName(), getTotal() / count);
}


// this function is obsoleted
#if 0
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
#endif

template <class T>
int Recorder<T>::getSize()
{
	return recorded_value.size();
}

