#pragma once
namespace media
{
	namespace reader
	{
		class FrameRateControl
		{
		public:
			FrameRateControl(IMFSourceReader * pSource);
			~FrameRateControl();

			float GetRate() const;
			void SetRate(float rate);
			void GetRateRange(float &min, float &max, MFRATE_DIRECTION direction = MFRATE_FORWARD) const;
			
			//returns nearest supported rate
			float IsRateSupported(float rate) const;
		protected:
			IMFSourceReader *pSourceReader;

		private:
			//---------------------------------------------------------------
			// Frame rate support provided by 2 services:
			//	- IMFRateSupport - https://msdn.microsoft.com/en-us/library/windows/desktop/ms701858(v=vs.85).aspx
			//	- IMFRateControl - https://msdn.microsoft.com/en-us/library/windows/desktop/ms697193(v=vs.85).aspx
		};
	}
}

