#pragma once
namespace media
{
	namespace reader
	{

		class FpsCounter
		{
		public:
			FpsCounter();
			~FpsCounter();

			void Start();
			long Tick();
			int fps;

		private:
			long long frameStart;
			long long lastFrameMs;
			int frames;
			static const int delayBetweenFramesMs = 34; //30fps = 1000ms/30frames

			long long getMilliseconds();
			bool isSecondGone(long long time);
			void clear();
			void outputFps();

		};
	}
}
