#include "stdafx.h"
#include "CamStreamSwImpl.h"
#include "ImageTransformers.h"

namespace media
{
	namespace reader
	{
		namespace impl_
		{
			CamStreamSwImpl::CamStreamSwImpl(HWND hWndVideo)
				: hVideo(hWndVideo),
				pReader(NULL)
			{
			}


			CamStreamSwImpl::~CamStreamSwImpl()
			{
				Close();
			}

			HRESULT CamStreamSwImpl::DrawSample(IMFMediaBuffer *pBuffer)
			{
				return Render.DrawFrame(pBuffer);
			}

			HRESULT CamStreamSwImpl::Next()
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

			HRESULT CamStreamSwImpl::Close()
			{
				SafeRelease(&pReader);
				return S_OK;
			}

			IDirect3DSurface9 *CamStreamSwImpl::GetDrawSurface()
			{
				assert(Render.pSurface != NULL);
				return Render.pSurface;
			}

			HRESULT CamStreamSwImpl::CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pReaderCallback)
			{
				HRESULT hr = S_OK;

				hr = Render.CreateDevice(hVideo);

				if (FAILED(hr))
					return hr;

				IMFAttributes   *pAttributes = NULL;
				IMFMediaType    *pType = NULL;

				// Create the source reader.
				if (SUCCEEDED(hr))
				{
					hr = MFCreateAttributes(&pAttributes, 2);
				}
				if (SUCCEEDED(hr))
				{
					hr = pAttributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
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

			HRESULT CamStreamSwImpl::TryMediaType(IMFMediaType *pType)
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
					hr = Render.SetVideoType(pType);
				}

				return hr;
			}

		}
	}
}
