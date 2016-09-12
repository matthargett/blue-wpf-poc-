#include "stdafx.h"
#include "Pipeline.h"
#include "FileStreamDevice.h"
#include "CamStreamDevice.h"

namespace media
{
	Pipeline::Pipeline()
		: pCapture(NULL)
	{

	}

	Pipeline::~Pipeline()
	{
		if (pCapture)
		{
			pCapture->Close();
			SafeRelease(&pCapture);
		}
	}

	HRESULT Pipeline::CreateCamInstance(int index, HWND hVideo, int camIndex, delegate callback, Pipeline **ppPipe)
	{
		assert(hVideo != NULL);
		if (ppPipe == NULL)
			return E_INVALIDARG;

		*ppPipe = NULL;

		Pipeline *pPipe = new (std::nothrow)Pipeline();

		if (pPipe == NULL)
			return E_OUTOFMEMORY;

		HRESULT hr = CamStreamDevice::CreateInstance(index, hVideo, hVideo, camIndex, callback, &(pPipe->pCapture));

		if (FAILED(hr))
		{
			delete pPipe;
			return hr;
		}

		*ppPipe = pPipe;
		return S_OK;
	}


	HRESULT Pipeline::CreateFileInstance(int index, HWND hVideo, PCWSTR pszAddress, delegate callback, Pipeline **ppPipe)
	{
		assert(hVideo != NULL);
		if (ppPipe == NULL)
			return E_INVALIDARG;
		
		*ppPipe = NULL;
		
		Pipeline *pPipe = new (std::nothrow)Pipeline();

		if (pPipe == NULL)
			return E_OUTOFMEMORY;

		HRESULT hr = FileStreamDevice::CreateInstance(index, hVideo, hVideo, pszAddress, callback, &(pPipe->pCapture));

		if (FAILED(hr))
		{
			delete pPipe;
			return hr;
		}

		*ppPipe = pPipe;
		return S_OK;
	}

	bool Pipeline::IsHwAccelerated() const
	{
		return pCapture->IsHwAccelerated();
	}

	void Pipeline::Next()
	{
		pCapture->Next();
	}

	IDirect3DSurface9 *Pipeline::GetSurface()
	{
		return pCapture->GetDrawSurface();
	}

	void Pipeline::GetFrameSize(int &width, int &height) const
	{
		width = pCapture->GetFrameWidth();
		height = pCapture->GetFrameHeight();
	}
}