#pragma once
namespace media
{
	namespace reader
	{
		class FrameFormatControl
		{
		public:
			FrameFormatControl(IMFMediaSource  * pSource);
			~FrameFormatControl();

			HRESULT Enumerate();
			HRESULT Set(DWORD idx);

		protected:
			IMFMediaSource  *pSourceReader;
		};
	}
}

