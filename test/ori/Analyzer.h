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

unsigned long long GetTimeUS();

class Recorder_
{
public:
	Recorder_(const char* _name) {strncpy(name, _name, sizeof(name)-1);}
	virtual void Reset() = 0;
	virtual double GetAvg() = 0;
	virtual void Commit() = 0;
	virtual void WriteCDF() = 0;
	virtual void PrintAVG(FILE* fp) = 0;
	virtual double GetSum() = 0;
	virtual int GetSize() = 0;
	char* GetName() {return name;}

private:
	char name[100];
};

template <class T> class Recorder : public Recorder_
{
public:
	Recorder(const char* name):Recorder_(name)	{Reset();}
	~Recorder()	{}
	virtual void Reset();
	virtual double GetAvg();
	virtual void Commit();
	virtual void WriteCDF();
	virtual void PrintAVG(FILE* fp);
	virtual double GetSum();
	virtual int GetSize();

	

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
	void StartTimer();
	unsigned long long StopTimer();
	void Reset();
	void Commit();


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
	void AddValue(double val);
	void Reset();
	void Commit();
	

private:
	std::vector<double> recorded_value;
};

class Analyzer
{
public:
	Analyzer(void);
	~Analyzer(void);

	void Reset();
	void TickTimeSlot();
	void WriteCDF();
	void PrintAVG(FILE* fp);

	std::vector<Recorder_ *> recorderList;
	TimeRecorder EstimatorTotal;
	TimeRecorder EstimatorSetup;
	TimeRecorder EstimatorSolving;
	TimeRecorder EstimatorPostProc;
	TimeRecorder EstimatorKFProc;
	TimeRecorder SolverSolving;
	ValueRecorder N_PMS;
	ValueRecorder N_PMSFiltered;
	ValueRecorder N_SelectionFail;
	ValueRecorder N_Reflection;
	ValueRecorder N_Selected;
	ValueRecorder EstimationError;
	

private:

};

