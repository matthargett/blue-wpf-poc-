#pragma once
#include "FpsCounter.h"

namespace media
{
	class IStreamDevice : public IMFSourceReaderCallback
	{
	public:


		virtual HRESULT Close() = 0;
		virtual HRESULT Next() = 0;
		virtual IDirect3DSurface9 *GetDrawSurface() = 0;

		inline UINT32 GetFrameWidth() const
		{
			return width;
		}

		inline UINT32 GetFrameHeight() const
		{
			return height;
		}

		virtual bool IsHwAccelerated() const = 0;

	protected:
		IStreamDevice(int idx, HWND hVideo, HWND hEvent, delegate callback)
			: frameRate(0), width(0), height(0),
			hWndVideo(hVideo), hWndEvent(hEvent),
			index(idx),
			isClosed(false),
			renderCallback(callback)
		{
		}
		virtual ~IStreamDevice() {}

		inline void DoRender()
		{
			if (!isClosed)
				renderCallback(index, counter.fps);
		}

		UINT32 frameRate;
		UINT32 height;
		UINT32 width;
		int index;
		HWND hWndVideo;
		HWND hWndEvent;

		delegate renderCallback;
		bool isClosed;

		reader::FpsCounter counter;
	};
}
