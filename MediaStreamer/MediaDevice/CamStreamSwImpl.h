SPECIAL ERROR TO TEST BUILD FAILURE
#pragma once
#include "ICamStreamImpl.h"
#include "device.h"
namespace media
{
	namespace reader
	{
		namespace impl_
		{
			//Software implementation of CamStream device
			class CamStreamSwImpl : public ICamStreamImpl
			{
			public:
				CamStreamSwImpl(HWND hVideo);
				~CamStreamSwImpl();

				virtual HRESULT CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pReaderCallback);
				virtual IDirect3DSurface9 *GetDrawSurface();
				virtual HRESULT DrawSample(IMFMediaBuffer *pBuffer);
				virtual HRESULT Next();
				virtual HRESULT Close();
				virtual bool IsAccelerated() const
				{
					return false;
				}

			private:
				IMFSourceReader		*pReader;
				presenter::DrawDevice Render;
				HWND hVideo;


				HRESULT TryMediaType(IMFMediaType *pType);
			};
		}
	}
}

