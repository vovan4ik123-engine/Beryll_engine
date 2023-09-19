/*

***************************************************************************************************
**
** profile.cpp
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

// Credits: The Clock class was inspired by the Timer classes in
// Ogre (www.ogre3d.org).

#include "btQuickprof.h"
#include "btThreads.h"

#ifdef __CELLOS_LV2__
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <stdio.h>
#endif

#if defined(SUNOS) || defined(__SUNOS__)
#include <stdio.h>
#endif
#ifdef __APPLE__
#include <mach/mach_time.h>
#include <TargetConditionals.h>
#endif

#if defined(WIN32) || defined(_WIN32)

#define BT_USE_WINDOWS_TIMERS
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOMCX
#define NOIME

#ifdef _XBOX
#include <Xtl.h>
#else  //_XBOX
#include <windows.h>

#if WINVER < 0x0602
#define GetTickCount64 GetTickCount
#endif

#endif  //_XBOX

#include <time.h>

#else  //_WIN32
#include <sys/time.h>

#ifdef BT_LINUX_REALTIME
//required linking against rt (librt)
#include <time.h>
#endif  //BT_LINUX_REALTIME

#endif  //_WIN32

#define mymin(a, b) (a > b ? a : b)

struct btClockData
{
#ifdef BT_USE_WINDOWS_TIMERS
	LARGE_INTEGER mClockFrequency;
	LONGLONG mStartTick;
	LARGE_INTEGER mStartTime;
#else
#ifdef __CELLOS_LV2__
	uint64_t mStartTime;
#else
#ifdef __APPLE__
	uint64_t mStartTimeNano;
#endif
	struct timeval mStartTime;
#endif
#endif  //__CELLOS_LV2__
};

///The btClock is a portable basic clock that measures accurate time in seconds, use for profiling.
btClock::btClock()
{
	m_data = new btClockData;
#ifdef BT_USE_WINDOWS_TIMERS
	QueryPerformanceFrequency(&m_data->mClockFrequency);
#endif
	reset();
}

btClock::~btClock()
{
	delete m_data;
}

btClock::btClock(const btClock& other)
{
	m_data = new btClockData;
	*m_data = *other.m_data;
}

btClock& btClock::operator=(const btClock& other)
{
	*m_data = *other.m_data;
	return *this;
}

/// Resets the initial reference time.
void btClock::reset()
{
#ifdef BT_USE_WINDOWS_TIMERS
	QueryPerformanceCounter(&m_data->mStartTime);
	m_data->mStartTick = GetTickCount64();
#else
#ifdef __CELLOS_LV2__

	typedef uint64_t ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);
	m_data->mStartTime = newTime;
#else
#ifdef __APPLE__
	m_data->mStartTimeNano = mach_absolute_time();
#endif
	gettimeofday(&m_data->mStartTime, 0);
#endif
#endif
}

/// Returns the time in ms since the last call to reset or since
/// the btClock was created.
unsigned long long int btClock::getTimeMilliseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	LONGLONG elapsedTime = currentTime.QuadPart -
						   m_data->mStartTime.QuadPart;
	// Compute the number of millisecond ticks elapsed.
	unsigned long msecTicks = (unsigned long)(1000 * elapsedTime /
											  m_data->mClockFrequency.QuadPart);

	return msecTicks;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1000.0;
	typedef uint64_t ClockSize;
	ClockSize newTime;
	SYS_TIMEBASE_GET(newTime);
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else

	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1000 +
		   (currentTime.tv_usec - m_data->mStartTime.tv_usec) / 1000;
#endif  //__CELLOS_LV2__
#endif
}

/// Returns the time in us since the last call to reset or since
/// the Clock was created.
unsigned long long int btClock::getTimeMicroseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	//see https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
	LARGE_INTEGER currentTime, elapsedTime;

	QueryPerformanceCounter(&currentTime);
	elapsedTime.QuadPart = currentTime.QuadPart -
						   m_data->mStartTime.QuadPart;
	elapsedTime.QuadPart *= 1000000;
	elapsedTime.QuadPart /= m_data->mClockFrequency.QuadPart;

	return (unsigned long long)elapsedTime.QuadPart;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1000000.0;
	typedef uint64_t ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else

	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1000000 +
		   (currentTime.tv_usec - m_data->mStartTime.tv_usec);
#endif  //__CELLOS_LV2__
#endif
}

unsigned long long int btClock::getTimeNanoseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	//see https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
	LARGE_INTEGER currentTime, elapsedTime;

	QueryPerformanceCounter(&currentTime);
	elapsedTime.QuadPart = currentTime.QuadPart -
						   m_data->mStartTime.QuadPart;
	elapsedTime.QuadPart *= 1000000000;
	elapsedTime.QuadPart /= m_data->mClockFrequency.QuadPart;

	return (unsigned long long)elapsedTime.QuadPart;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1e9;
	typedef uint64_t ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else
#ifdef __APPLE__
	uint64_t ticks = mach_absolute_time() - m_data->mStartTimeNano;
	static long double conversion = 0.0L;
	if (0.0L == conversion)
	{
		// attempt to get conversion to nanoseconds
		mach_timebase_info_data_t info;
		int err = mach_timebase_info(&info);
		if (err)
		{
			btAssert(0);
			conversion = 1.;
		}
		conversion = info.numer / info.denom;
	}
	return (ticks * conversion);

#else  //__APPLE__

#ifdef BT_LINUX_REALTIME
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return 1000000000 * ts.tv_sec + ts.tv_nsec;
#else
	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1e9 +
		   (currentTime.tv_usec - m_data->mStartTime.tv_usec) * 1000;
#endif  //BT_LINUX_REALTIME

#endif  //__APPLE__
#endif  //__CELLOS_LV2__
#endif
}

/// Returns the time in s since the last call to reset or since
/// the Clock was created.
btScalar btClock::getTimeSeconds()
{
	static const btScalar microseconds_to_seconds = btScalar(0.000001);
	return btScalar(getTimeMicroseconds()) * microseconds_to_seconds;
}

void btEnterProfileZoneDefault(const char* name)
{
}
void btLeaveProfileZoneDefault()
{
}

// clang-format off
#if defined(_WIN32) && (defined(__MINGW32__) || defined(__MINGW64__))
  #define BT_HAVE_TLS 1
#elif __APPLE__ && !TARGET_OS_IPHONE
  // TODO: Modern versions of iOS support TLS now with updated version checking.
  #define BT_HAVE_TLS 1
#elif __linux__
  #define BT_HAVE_TLS 1
#elif defined(__FreeBSD__) || defined(__NetBSD__)
  // TODO: At the moment disabling purposely OpenBSD, albeit tls support exists but not fully functioning
  #define BT_HAVE_TLS 1
#endif

// __thread is broken on Andorid clang until r12b. See
// https://github.com/android-ndk/ndk/issues/8
#if defined(__ANDROID__) && defined(__clang__)
  #if __has_include(<android/ndk-version.h>)
    #include <android/ndk-version.h>
  #endif  // __has_include(<android/ndk-version.h>)
  #if defined(__NDK_MAJOR__) && \
    ((__NDK_MAJOR__ < 12) || ((__NDK_MAJOR__ == 12) && (__NDK_MINOR__ < 1)))
    #undef BT_HAVE_TLS
  #endif
#endif  // defined(__ANDROID__) && defined(__clang__)
// clang-format on

static btEnterProfileZoneFunc* bts_enterFunc = btEnterProfileZoneDefault;
static btLeaveProfileZoneFunc* bts_leaveFunc = btLeaveProfileZoneDefault;

void btEnterProfileZone(const char* name)
{
	(bts_enterFunc)(name);
}
void btLeaveProfileZone()
{
	(bts_leaveFunc)();
}

btEnterProfileZoneFunc* btGetCurrentEnterProfileZoneFunc()
{
	return bts_enterFunc;
}
btLeaveProfileZoneFunc* btGetCurrentLeaveProfileZoneFunc()
{
	return bts_leaveFunc;
}

void btSetCustomEnterProfileZoneFunc(btEnterProfileZoneFunc* enterFunc)
{
	bts_enterFunc = enterFunc;
}
void btSetCustomLeaveProfileZoneFunc(btLeaveProfileZoneFunc* leaveFunc)
{
	bts_leaveFunc = leaveFunc;
}

CProfileSample::CProfileSample(const char* name)
{
	btEnterProfileZone(name);
}

CProfileSample::~CProfileSample(void)
{
	btLeaveProfileZone();
}
