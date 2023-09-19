
/***************************************************************************************************
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

// Credits: The Clock class was inspired by the Timer classes in
// Ogre (www.ogre3d.org).

#ifndef BT_QUICK_PROF_H
#define BT_QUICK_PROF_H

#include "btScalar.h"
#define USE_BT_CLOCK 1

#ifdef USE_BT_CLOCK

///The btClock is a portable basic clock that measures accurate time in seconds, use for profiling.
class btClock
{
public:
	btClock();

	btClock(const btClock& other);
	btClock& operator=(const btClock& other);

	~btClock();

	/// Resets the initial reference time.
	void reset();

	/// Returns the time in ms since the last call to reset or since
	/// the btClock was created.
	unsigned long long int getTimeMilliseconds();

	/// Returns the time in us since the last call to reset or since
	/// the Clock was created.
	unsigned long long int getTimeMicroseconds();

	unsigned long long int getTimeNanoseconds();

	/// Returns the time in s since the last call to reset or since
	/// the Clock was created.
	btScalar getTimeSeconds();

private:
	struct btClockData* m_data;
};

#endif  //USE_BT_CLOCK

typedef void(btEnterProfileZoneFunc)(const char* msg);
typedef void(btLeaveProfileZoneFunc)();

btEnterProfileZoneFunc* btGetCurrentEnterProfileZoneFunc();
btLeaveProfileZoneFunc* btGetCurrentLeaveProfileZoneFunc();

void btSetCustomEnterProfileZoneFunc(btEnterProfileZoneFunc* enterFunc);
void btSetCustomLeaveProfileZoneFunc(btLeaveProfileZoneFunc* leaveFunc);

///ProfileSampleClass is a simple way to profile a function's scope
///Use the BT_PROFILE macro at the start of scope to time
class CProfileSample
{
public:
	CProfileSample(const char* name);

	~CProfileSample(void);
};

#define BT_PROFILE(name) CProfileSample __profile(name)

#endif  //BT_QUICK_PROF_H
