#pragma once
#include "ICamStreamImpl.h"
namespace media
{
	namespace reader
	{
		namespace impl_
		{
			ICamStreamImpl *CreateStaticDevice(HWND hVideo);
			ICamStreamImpl *CreateCamStreamSwDevice(HWND hVideo);
			ICamStreamImpl *CreateCamStreamHwDevice(HWND hVideo);
			ICamStreamImpl *CreateCamStreamDevice(byte * buff, size_t length, HWND hVideo);
		}
	}
}