#pragma once
namespace media
{
	namespace _impl
	{
		HRESULT LogMediaType(IMFMediaType *pType);
		void DBGMSG(PCWSTR format, ...);
	}
}