#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>

#include "..\..\..\sketch\libraries\StepperSystem\src\StepperSystem.h"
#include "..\..\..\sketch\libraries\StepperSystem\src\HelpParser.h"

#define _STORETIMEVALUES	1000000

class CMsvcStepper : public CStepper
{

public:

	CMsvcStepper();

	virtual void OnIdle(unsigned long idletime);
	virtual void OnStart();
	virtual void OnWait(EnumAsByte(EWaitType) wait);

	virtual void Init();

	virtual bool  IsReference(unsigned char referenceId);
	virtual bool  IsAnyReference() { return IsReference(0); };

protected:

	virtual void  StepBegin(const SStepBuffer* step);
//	virtual void  Step(unsigned char steps[NUM_AXIS],bool directionUp[NUM_AXIS]);
	virtual void  Step(const unsigned char steps[NUM_AXIS],unsigned char directionUp);
	virtual void  Step(bool isr);

	virtual void  SetEnable(axis_t axis, unsigned char level)	{ _level[axis] = level; };
	virtual unsigned char GetEnable(axis_t axis)				{ return _level[axis]; }

private:

	unsigned char _level[NUM_AXIS];

public:

	virtual void StartTimer(timer_t timerB);		// 0 => set idle timer (==Timer not running)
	virtual void SetIdleTimer();					// set idle Timer
	virtual void StopTimer();					// to cancel all timer

	virtual void OptimizeMovementQueue(bool force);
	virtual bool MoveReference(axis_t axis, unsigned char referenceid, bool toMin, steprate_t vMax, sdist_t maxdist, sdist_t distToRef, sdist_t distIfRefIsOn);

	// Test extensions

	void InitTest(const char* filename=NULL);
	void EndTest(const char* filename=NULL);

	bool DelayOptimization;
	bool SplitFile;
	bool UseSpeedSign;
	int  CacheSize;

private:

	void WriteTestResults(const char* filename);

	const char* _filename;
	int _flushcount;

	struct STimerEvent
	{
		timer_t TimerValues;
		int Steps;
		int Count;
		struct SAxis
		{
			int Multiplier;
			int MoveAxis;
			int Distance;
		} Axis[NUM_AXIS];
		char MSCInfo[MOVEMENTINFOSIZE];
	};

	int _TotalSteps;

	void InitCache()
	{
		_eventIdx = 0;
		memset(_TimerEvents, 0, sizeof(_TimerEvents));
	}

	int _exportIdx;
	int _eventIdx;
	STimerEvent* _TimerEvents;
	int _oldCacheSize;

	int	_sumtime[NUM_AXIS];
	int _count[NUM_AXIS];
	int _total[NUM_AXIS];
	char _speed[NUM_AXIS][20];
	long long _totaltime;
	int _lasttimer;

	int _refMovestart;

	bool _isReferenceMove;
	unsigned char _isReferenceId;
	int  _referenceMoveSteps;

public:

	void DoISR();
};
