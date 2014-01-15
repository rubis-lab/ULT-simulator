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
	virtual double getAvg() = 0;
	virtual void commit() = 0;
	virtual void writeCDF() = 0;
	virtual void printAVG(FILE* fp) = 0;
	virtual double getSum() = 0;
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
	virtual double getAvg();
	virtual void commit();
	virtual void writeCDF();
	virtual void printAVG(FILE* fp);
	virtual double getSum();
	virtual int getSize();

	

protected:
	std::vector <T> recorded_value;
	T current_value;
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
	

private:
	std::vector<double> recorded_value;
};

class Analyzer
{
public:
	Analyzer(void);
	~Analyzer(void);

	void reset();
	void tickTimeSlot();
	void writeCDF();
	void printAVG(FILE* fp);

	std::vector<Recorder_ *> recorderList;
	TimeRecorder estimatorTotal;
	TimeRecorder estimatorSetup;
	TimeRecorder estimatorSolving;
	TimeRecorder estimatorPostProc;
	TimeRecorder estimatorKFProc;
	TimeRecorder solverSolving;
	ValueRecorder N_PMS;
	ValueRecorder N_PMSFiltered;
	ValueRecorder N_selectionFail;
	ValueRecorder N_reflection;
	ValueRecorder N_selected;
	ValueRecorder estimationError;
	

private:

};

