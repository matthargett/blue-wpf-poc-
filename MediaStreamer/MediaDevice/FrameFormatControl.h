#pragma once
namespace media
{
	namespace reader
	{
		struct FrameFormat
		{
			FrameFormat(int w, int h, int f)
				: width(w),
				height(h),
				fps(f)
			{}
			
			int width;	//frame width
			int height;	//frame height
			int fps;	//frame fps

			int Serialize() const
			{
				return FrameFormatSerialize(width, height, fps);
			}
		};

		class FrameFormatControl
		{
		public:
			FrameFormatControl(IMFMediaSource  * pSource);
			~FrameFormatControl();

			HRESULT Enumerate();
			HRESULT Set(DWORD idx);

			static FrameFormat GetFormat(IMFMediaType* pType);

		protected:
			IMFMediaSource  *pSourceReader;
		};
	}
}

