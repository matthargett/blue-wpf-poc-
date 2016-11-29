#include "stdafx.h"
#include "CamStreamFactory.h"
#include "CamStreamHwImpl.h"
#include "CamStreamSwImpl.h"
#include "CamStreamStaticImpl.h"

namespace media
{
	namespace reader
	{
		namespace impl_
		{
			ICamStreamImpl *CreateCamStreamDevice(byte * buff, size_t length, HWND hVideo, int prefferableMode)
			{
				if (length < 32)
					return CreateCamStreamSwDevice(hVideo, prefferableMode);

				GUID *g = (GUID *)malloc(sizeof(GUID));
				::memcpy(g, static_cast<void*>( &buff[16]), 16);

				ICamStreamImpl *pImpl = nullptr;

				if (MFVideoFormat_YUY2 == *g || MFVideoFormat_NV12 == *g)
					pImpl = CreateCamStreamHwDevice(hVideo, prefferableMode);
				else
					pImpl = CreateCamStreamSwDevice(hVideo, prefferableMode);

				free(g);

				return pImpl;
			}

			ICamStreamImpl *CreateCamStreamSwDevice(HWND hVideo, int prefferableMode) 
			{
				return new (std::nothrow) CamStreamSwImpl(hVideo, prefferableMode);
			}
			
			ICamStreamImpl *CreateCamStreamHwDevice(HWND hVideo, int prefferableMode)
			{
				return new (std::nothrow) CamStreamHwImpl(hVideo, prefferableMode);
			}

			ICamStreamImpl *CreateStaticDevice(HWND hVideo)
			{
				return new (std::nothrow) CamStreamStaticImpl(hVideo);
			}
		}
	}
}