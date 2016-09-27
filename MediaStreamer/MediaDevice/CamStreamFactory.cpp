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
			ICamStreamImpl *CreateCamStreamDevice(byte * buff, size_t length, HWND hVideo)
			{
				if (length < 32)
					return CreateCamStreamSwDevice(hVideo);

				GUID *g = (GUID *)malloc(sizeof(GUID));
				::memcpy(g, static_cast<void*>( &buff[16]), 16);

				ICamStreamImpl *pImpl = nullptr;

				if (MFVideoFormat_YUY2 == *g || MFVideoFormat_NV12 == *g)
					pImpl = CreateCamStreamHwDevice(hVideo);
				else
					pImpl = CreateCamStreamSwDevice(hVideo);

				free(g);

				return pImpl;
			}

			ICamStreamImpl *CreateCamStreamSwDevice(HWND hVideo) 
			{
				return new (std::nothrow) CamStreamSwImpl(hVideo);
			}
			
			ICamStreamImpl *CreateCamStreamHwDevice(HWND hVideo)
			{
				return new (std::nothrow) CamStreamHwImpl(hVideo);
			}

			ICamStreamImpl *CreateStaticDevice(HWND hVideo)
			{
				return new (std::nothrow) CamStreamStaticImpl(hVideo);
			}
		}
	}
}