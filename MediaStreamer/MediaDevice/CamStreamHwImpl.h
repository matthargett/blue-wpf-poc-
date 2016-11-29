#pragma once
#include "ICamStreamImpl.h"

namespace media
{
	namespace presenter
	{
		class RenderDevice;
	}
	
	namespace reader
	{
		namespace impl_
		{
			class CamStreamHwImpl : public ICamStreamImpl
			{
			public:
				CamStreamHwImpl(HWND hVideo, int prefferableMode);
				~CamStreamHwImpl();

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
				HWND hVideo;
				IMFSourceReader		*pReader;
				presenter::RenderDevice *pRender;

				HRESULT TryMediaType(IMFMediaType *pType);
			};
		}
	}
}

