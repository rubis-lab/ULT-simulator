#pragma once
#include <vector>
#ifndef ENV_LINUX
#include <windows.h>
int gettimeofday(struct timeval* tp, int* tz);
#else
#include <sys/time.h>
#endif

#include <string.h>
#include <stdio.h>

unsigned long long getTimeUS();

class Recorder_
{
public:
	Recorder_(const char* _name) {strncpy(name, _name, sizeof(name)-1);}
	virtual void reset() = 0;
//	virtual double getAvg() = 0;			// obsoleted
	virtual void commit() = 0;
	virtual void discard() = 0;
//	virtual void writeCDF() = 0;			// obsoleted
//	virtual void printAVG(FILE* fp) = 0;	// obsoleted
	virtual void printAVG(FILE* fp, int count) = 0;
//	virtual double getSum() = 0;			// obsoleted
	virtual int getSize() = 0;
	char* getName() {return name;}

private:
	char name[100];
};

template <class T> class Recorder : public Recorder_
{
public:
	Recorder(const char* name):Recorder_(name)	{reset();}
	~Recorder()	{}
	virtual void reset();
//	virtual double getAvg();			// obsoleted
	virtual void commit();
	virtual void discard();
//	virtual void writeCDF();			// obsoleted
//	virtual void printAVG(FILE* fp);	// obsoleted
	virtual void printAVG(FILE* fp, int count);
//	virtual double getSum();			// obsoleted
	virtual int getSize();
	virtual double getTotal() = 0;

	

protected:
	std::vector <T> recorded_value;
	T current_value;
	T total_value;
};


class TimeRecorder : public Recorder <unsigned long long>
{
public:
	TimeRecorder():Recorder<unsigned long long>(""){}
	TimeRecorder(const char* name);
	~TimeRecorder();
	void startTimer();
	unsigned long long stopTimer();
	void reset();
	void commit();
	void discard();
	double getTotal();


private:
	unsigned long long start_time;
	unsigned long long execution_time;
	
};

class ValueRecorder : public Recorder <double>
{
public:
	ValueRecorder():Recorder<double>(""){}
	ValueRecorder(const char* name);
	~ValueRecorder();
	void addValue(double val);
	void reset();
	void commit();
	void discard();
	double getTotal();
	

private:
	std::vector<double> recorded_value;
};

class Analyzer
{
public:
	Analyzer(void);
	~Analyzer(void);

	void reset();
	void tickTimeSlot();		// obsoleted
	void writeCDF();
	void printAVG(FILE* fp);
	double getTotal();

	std::vector<Recorder_ *> recorderList;
	TimeRecorder estimatorTotal;
	TimeRecorder estimatorSetup;
	TimeRecorder estimatorSolving;
	TimeRecorder estimatorPostProc;
	TimeRecorder estimatorKFProc;
	TimeRecorder solverSolving;
	ValueRecorder N_PMS;
	ValueRecorder N_PMSFiltered;
	ValueRecorder N_PMS_KF;
	ValueRecorder N_selectionFail;
	ValueRecorder N_opt1Fail;
	ValueRecorder N_optThresholdFail;
	ValueRecorder N_kalmanFilterFail;
	ValueRecorder N_reflection;
	ValueRecorder N_reflection2;
	ValueRecorder N_selected;
	ValueRecorder N_fail;
	ValueRecorder N_tick;
	ValueRecorder estimationError;
	ValueRecorder N_receptionFail;
	ValueRecorder N_receptionFailStrict;
	ValueRecorder estimationErrorExcFail;
	ValueRecorder estimationErrorExcStrictFail;
	ValueRecorder estimationError2;
	ValueRecorder N_receptionFail2;
	ValueRecorder N_receptionFailStrict2;
	ValueRecorder estimationErrorExcFail2;
	ValueRecorder estimationErrorExcStrictFail2;
	

private:

};

