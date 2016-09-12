#include "stdafx.h"
#include "CamStreamHwImpl.h"
#include "RenderDevice.h"
#include "ImageTransformers.h"

namespace media
{
	namespace reader
	{
		namespace impl_
		{

			CamStreamHwImpl::CamStreamHwImpl(HWND hWndVideo)
				: hVideo(hWndVideo),
				pReader(NULL), pRender(NULL)
			{
			}

			CamStreamHwImpl::~CamStreamHwImpl()
			{
				Close();
			}

			IDirect3DSurface9 *CamStreamHwImpl::GetDrawSurface()
			{
				assert(pRender->pSurface != NULL);
				return pRender->pSurface;
			}

			HRESULT CamStreamHwImpl::Next()
			{
				return pReader->ReadSample(
					(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
					0,
					NULL,
					NULL,
					NULL,
					NULL
				);
			}

			HRESULT CamStreamHwImpl::Close()
			{
				SafeRelease(&pReader);
				if (pRender)
				{
					delete pRender;
					pRender = NULL;
				}
				return S_OK;
			}

			HRESULT CamStreamHwImpl::DrawSample(IMFMediaBuffer *pBuffer)
			{
				IDirect3DSurface9 *pSurface = NULL;
				HRESULT hr = MFGetService(pBuffer, MR_BUFFER_SERVICE, IID_PPV_ARGS(&pSurface));
				if (FAILED(hr))
					return hr;

				pRender->Draw(pSurface);
				SafeRelease(&pSurface);

				return S_OK;
			}


			HRESULT CamStreamHwImpl::CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pReaderCallback)
			{
				HRESULT hr = S_OK;

				pRender = new (std::nothrow) presenter::RenderDevice();
				if (pRender == NULL)
					return E_OUTOFMEMORY;

				hr = pRender->CreateDevice(hVideo);
				if (FAILED(hr))
					return hr;

				IMFAttributes   *pAttributes = NULL;
				IMFMediaType    *pType = NULL;

				if (SUCCEEDED(hr))
				{
					hr = MFCreateAttributes(&pAttributes, 2);
				}
				if (SUCCEEDED(hr))
				{
					hr = pAttributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
				}

				if (SUCCEEDED(hr))
				{
					hr = pAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, pRender->pD3DManager->pManager);
				}

				// Set the callback pointer.
				if (SUCCEEDED(hr))
				{
					hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, pReaderCallback);
				}

				if (SUCCEEDED(hr))
				{
					hr = MFCreateSourceReaderFromMediaSource(
						pSource,
						pAttributes,
						&pReader
					);
				}

				// Try to find a suitable output type.
				if (SUCCEEDED(hr))
				{
					for (DWORD i = 0; ; i++)
					{
						hr = pReader->GetNativeMediaType(
							(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
							i,
							&pType);

						if (FAILED(hr)) { break; }


						hr = TryMediaType(pType);
						SafeRelease(&pType);

						if (SUCCEEDED(hr))
						{
							// Found an output type.
							break;
						}
					}
				}

				if (FAILED(hr))
				{
					if (pSource)
					{
						pSource->Shutdown();


						// NOTE: The source reader shuts down the media source
						// by default, but we might not have gotten that far.
					}

					Close();
				}


				SafeRelease(&pAttributes);
				SafeRelease(&pType);

				return hr;

			}

			HRESULT CamStreamHwImpl::TryMediaType(IMFMediaType *pType)
			{
				HRESULT hr = S_OK;

				BOOL bFound = FALSE;
				GUID subtype = { 0 };

				hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);

				if (FAILED(hr))
				{
					return hr;
				}

				// Do we support this type directly?
				if (presenter::IsFormatSupported(subtype))
				{
					bFound = TRUE;
				}
				else
				{
					// Can we decode this media type to one of our supported
					// output formats?

					for (DWORD i = 0; ; i++)
					{
						// Get the i'th format.
						presenter::GetFormat(i, &subtype);

						hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);

						if (FAILED(hr)) { break; }

						// Try to set this type on the source reader.
						hr = pReader->SetCurrentMediaType(
							(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
							NULL,
							pType
						);

						if (SUCCEEDED(hr))
						{
							bFound = TRUE;
							break;
						}
					}
				}

				if (bFound)
				{
					hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
					hr = pRender->CreateVAService(width, height, MFVideoFormat_YUY2);
				}

				return hr;
			}

		}
	}
}
