// hrtimer.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "hrtimer.h"

#ifdef HIGHRES_TIMER_AVAILABLE

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <sys/time.h>
#elif defined(macintosh)
#include <Timer.h>
#endif

#include <assert.h>

NAMESPACE_BEGIN(CryptoPP)

word64 Timer::GetCurrentTimerValue()
{
#if defined(_WIN32)
	FILETIME now;
	GetSystemTimeAsFileTime(&now);
	return now.dwLowDateTime + ((word64)now.dwHighDateTime << 32);
#elif defined(__unix__)
	timeval now;
	gettimeofday(&now, NULL);
	return (word64)now.tv_sec * 1000000 + now.tv_usec;
#elif defined(macintosh)
	UnsignedWide now;
	Microseconds(&now);
	return now.lo + ((word64)now.hi << 32);
#endif
}

unsigned long Timer::ConvertTo(word64 t, Unit unit)
{
	switch (unit)
	{
	case SECONDS:
		return t / (TicksPerMillisecond() * 1000);
	case MILLISECONDS:
		return t / TicksPerMillisecond();
	case MICROSECONDS:
		assert(TicksPerMillisecond() % 1000 == 0);
		return t / (TicksPerMillisecond() / 1000);
	}
	assert(false);
	return 0;
}

void Timer::StartTimer()
{
	m_start = GetCurrentTimerValue();
	m_started = true;
}

unsigned long Timer::ElapsedTime()
{
	if (m_started)
		return ConvertTo(GetCurrentTimerValue() - m_start, m_unit);
	else
	{
		StartTimer();
		return 0;
	}
}

NAMESPACE_END

#endif
