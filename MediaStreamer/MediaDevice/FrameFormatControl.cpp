#include "stdafx.h"
#include "FrameFormatControl.h"
#include "DebugMedia.h"

using namespace media::_impl;
namespace media
{
	namespace reader
	{

		FrameFormatControl::FrameFormatControl(IMFMediaSource  * pSource)
			: pSourceReader(pSource)
		{
			pSourceReader->AddRef();
		}


		FrameFormatControl::~FrameFormatControl()
		{
			SafeRelease(&pSourceReader);
		}

		HRESULT FrameFormatControl::Enumerate()
		{
			IMFPresentationDescriptor *pPD = NULL;
			IMFStreamDescriptor *pSD = NULL;
			IMFMediaTypeHandler *pHandler = NULL;
			IMFMediaType *pType = NULL;

			HRESULT hr = pSourceReader->CreatePresentationDescriptor(&pPD);
			if (FAILED(hr))
			{
				goto done;
			}

			BOOL fSelected;
			hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
			if (FAILED(hr))
			{
				goto done;
			}

			hr = pSD->GetMediaTypeHandler(&pHandler);
			if (FAILED(hr))
			{
				goto done;
			}

			DWORD cTypes = 0;
			hr = pHandler->GetMediaTypeCount(&cTypes);
			if (FAILED(hr))
			{
				goto done;
			}

			for (DWORD i = 0; i < cTypes; i++)
			{
				hr = pHandler->GetMediaTypeByIndex(i, &pType);
				if (FAILED(hr))
				{
					goto done;
				}
				
				DBGMSG(L"Idx: %d\n", i);
				LogMediaType(pType);
				OutputDebugString(L"\n");

				SafeRelease(&pType);
			}

		done:
			SafeRelease(&pPD);
			SafeRelease(&pSD);
			SafeRelease(&pHandler);
			SafeRelease(&pType);
			return hr;
		}

		HRESULT FrameFormatControl::Set(DWORD idx)
		{
			IMFPresentationDescriptor *pPD = NULL;
			IMFStreamDescriptor *pSD = NULL;
			IMFMediaTypeHandler *pHandler = NULL;
			IMFMediaType *pType = NULL;

			HRESULT hr = pSourceReader->CreatePresentationDescriptor(&pPD);
			if (FAILED(hr))
			{
				goto done;
			}

			BOOL fSelected;
			hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
			if (FAILED(hr))
			{
				goto done;
			}

			hr = pSD->GetMediaTypeHandler(&pHandler);
			if (FAILED(hr))
			{
				goto done;
			}

			hr = pHandler->GetMediaTypeByIndex(idx, &pType);
			if (FAILED(hr))
			{
				goto done;
			}

			hr = pHandler->SetCurrentMediaType(pType);

		done:
			SafeRelease(&pPD);
			SafeRelease(&pSD);
			SafeRelease(&pHandler);
			SafeRelease(&pType);
			return hr;
		}
	}
}
