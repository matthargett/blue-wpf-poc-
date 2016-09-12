#include "stdafx.h"
#include <stdio.h>
#include "FpsCounter.h"

namespace media
{
	namespace reader
	{
		FpsCounter::FpsCounter()
			:frameStart(0), 
			lastFrameMs(0),
			frames(0), 
			fps(0)
		{
		}


		FpsCounter::~FpsCounter()
		{
		}

		void FpsCounter::Start()
		{
			frameStart = getMilliseconds();
			lastFrameMs = frameStart;
		}

		long FpsCounter::Tick()
		{
			++frames;
			long long passedTimeMs = getMilliseconds();
			if (isSecondGone(passedTimeMs))
			{
				outputFps();
				clear();
			}

			lastFrameMs += delayBetweenFramesMs;
			return lastFrameMs > passedTimeMs ? (long)(lastFrameMs - passedTimeMs) : 0;
		}

		long long FpsCounter::getMilliseconds() {
			static LARGE_INTEGER s_frequency;
			static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
			if (s_use_qpc) {
				LARGE_INTEGER now;
				QueryPerformanceCounter(&now);
				return (1000LL * now.QuadPart) / s_frequency.QuadPart;
			}
			else {
				return ::GetTickCount();
			}
		}

		bool FpsCounter::isSecondGone(long long time)
		{
			long long elapsed = time - frameStart;
			return elapsed > 1000;

		}

		void FpsCounter::clear()
		{
			frameStart = getMilliseconds();
			frames = 0;
		}

		void FpsCounter::outputFps()
		{
			//TCHAR buf[8];
			//swprintf_s(buf, 6, L"%i", frames);
			//::OutputDebugString(buf);

			fps = frames;
		}
	}
}