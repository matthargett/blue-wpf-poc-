#include "stdafx.h"
#include "SeekControl.h"

namespace media
{
	namespace reader
	{
		SeekControl::SeekControl(IMFSourceReader *pReader)
		{
			pReader->AddRef();
			pSourceReader = pReader;
		}

		SeekControl::~SeekControl()
		{
			SafeRelease(&pSourceReader);
		}


		bool SeekControl::CanSeek()
		{
			BOOL bCanSeek = FALSE;
			ULONG flags;
			if (SUCCEEDED(GetSourceFlags(pSourceReader, &flags)))
			{
				bCanSeek = ((flags & MFMEDIASOURCE_CAN_SEEK) == MFMEDIASOURCE_CAN_SEEK);
			}
			return bCanSeek == TRUE;
		}

		HRESULT SeekControl::SetPosition(LONGLONG position)
		{
			PROPVARIANT var;
			HRESULT hr = InitPropVariantFromInt64(position, &var);
			if (SUCCEEDED(hr))
			{
				hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
				PropVariantClear(&var);
			}
			return hr;
		}

		HRESULT SeekControl::GetSourceFlags(IMFSourceReader *pReader, ULONG *pulFlags)
		{
			ULONG flags = 0;

			PROPVARIANT var;
			PropVariantInit(&var);

			HRESULT hr = pReader->GetPresentationAttribute(
				MF_SOURCE_READER_MEDIASOURCE,
				MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS,
				&var);

			if (SUCCEEDED(hr))
			{
				hr = PropVariantToUInt32(var, &flags);
			}
			if (SUCCEEDED(hr))
			{
				*pulFlags = flags;
			}

			PropVariantClear(&var);
			return hr;
		}
	}
}
