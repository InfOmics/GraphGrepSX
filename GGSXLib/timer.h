#ifndef TIMER_H_
#define TIMER_H_

#ifdef WIN32 // definita nelle direttive del preprocessore in Visual Studio 7.0

#include <Windows.h>
typedef LARGE_INTEGER TIMEHANDLE;

extern LARGE_INTEGER _freq;

inline TIMEHANDLE start_time()
{
    static int first = 1;

    if(first) {
        if (!QueryPerformanceFrequency(&_freq))
			cout << "No high-resolution performance counter installed" << endl;

		if (_freq.QuadPart == 0)
			cout << "High-resolution performance counter available but _freq = 0" << endl;
        first = 0;
    }
    TIMEHANDLE tstart;
	QueryPerformanceCounter(&tstart);
	return tstart;
}
inline double end_time(TIMEHANDLE th)
{
    TIMEHANDLE tend;
    QueryPerformanceCounter(&tend);

	double res = (double)(tend.QuadPart - th.QuadPart)/(_freq.QuadPart); 
	return res;
}

#pragma warning( disable : 4267 4018)

#else // sotto linux

#include <sys/time.h>

typedef struct timeval TIMEHANDLE;

//struct rusage ru;

extern struct timezone _tz;
inline TIMEHANDLE start_time()
{
    TIMEHANDLE tstart;
    gettimeofday(&tstart, &_tz);
    //getrusage(RUSAGE_SELF, &ru);
    //tstart = ru.ru_utime;
    return tstart;
}
inline double end_time(TIMEHANDLE th)
{
    TIMEHANDLE tend;
    double t1, t2;

	gettimeofday(&tend,&_tz);
	//getrusage(RUSAGE_SELF, &ru);
    //tend = ru.ru_utime;
    //printf("%f-%f", (double)th.tv_usec, (double)tend.tv_usec);

    t1 =  (double)th.tv_sec + (double)th.tv_usec/(1000*1000);
    t2 =  (double)tend.tv_sec + (double)tend.tv_usec/(1000*1000);
    return t2-t1;
}
#endif //WIN32

#ifdef WIN32
LARGE_INTEGER _freq;
#else
struct timezone _tz;
#endif

#endif /* TIMER_H*/

