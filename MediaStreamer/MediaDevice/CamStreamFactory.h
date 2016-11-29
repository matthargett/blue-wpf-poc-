#pragma once
#include "ICamStreamImpl.h"
namespace media
{
	namespace reader
	{
		namespace impl_
		{
			ICamStreamImpl *CreateStaticDevice(HWND hVideo);
			ICamStreamImpl *CreateCamStreamSwDevice(HWND hVideo, int prefferableMode);
			ICamStreamImpl *CreateCamStreamHwDevice(HWND hVideo, int prefferableMode);
			ICamStreamImpl *CreateCamStreamDevice(byte * buff, size_t length, HWND hVideo, int prefferableMode);
		}
	}
}