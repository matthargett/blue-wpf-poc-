#include "stdafx.h"
#include "CamStreamStaticImpl.h"
#include "StaticDevice.h"
#include "StaticStreamAsyncHelper.h"

namespace media
{
	namespace reader
	{
		namespace impl_
		{

			CamStreamStaticImpl::CamStreamStaticImpl(HWND hWndVideo)
				: ICamStreamImpl(0),
				hVideo(hWndVideo),
				pRender(NULL), pReaderCallback(NULL)
			{
				width = 320;
				height = 240;
				pAsyncHelper = new StaticStreamAsyncHelper(this);
			}

			CamStreamStaticImpl::~CamStreamStaticImpl()
			{
				delete pAsyncHelper;
				Close();
			}

			IDirect3DSurface9 *CamStreamStaticImpl::GetDrawSurface()
			{
				return pRender->GetSurface();
			}

			HRESULT CamStreamStaticImpl::Next()
			{
				pAsyncHelper->Next();
				return S_OK;
			}

			HRESULT CamStreamStaticImpl::Close()
			{
				SafeRelease(&pReaderCallback);
				if (pRender)
				{
					delete pRender;
					pRender = NULL;
				}
				return S_OK;
			}

			HRESULT CamStreamStaticImpl::DrawSample(IMFMediaBuffer *pBuffer)
			{
				pRender->Draw(NULL);

				return S_OK;
			}


			HRESULT CamStreamStaticImpl::CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pCallback)
			{
				HRESULT hr = S_OK;

				pCallback->AddRef();
				pReaderCallback = pCallback;

				pRender = new (std::nothrow) presenter::StaticDevice();
				if (pRender == NULL)
					return E_OUTOFMEMORY;

				hr = pRender->CreateDevice(hVideo);
				return hr;
			}
		}
	}
}
