#pragma once
#include "ICamStreamImpl.h"

namespace media
{
	namespace presenter
	{
		class StaticDevice;
	}

	namespace reader
	{
		namespace impl_
		{
			class StaticStreamAsyncHelper;

			class CamStreamStaticImpl : public ICamStreamImpl
			{
			public:
				CamStreamStaticImpl(HWND hVideo);
				~CamStreamStaticImpl();

				virtual HRESULT CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pReaderCallback);
				virtual IDirect3DSurface9 *GetDrawSurface();
				virtual HRESULT DrawSample(IMFMediaBuffer *pBuffer);
				virtual HRESULT Next();
				virtual HRESULT Close();
				virtual bool IsAccelerated() const
				{
					return true;
				}
			private:
				friend StaticStreamAsyncHelper;
				HWND hVideo;
				presenter::StaticDevice *pRender;
				IMFSourceReaderCallback *pReaderCallback;
				StaticStreamAsyncHelper *pAsyncHelper;
			};
		}
	}
}

