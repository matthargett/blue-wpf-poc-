#include "stdafx.h"
#include "FileStreamDevice.h"
#include "FrameRateControl.h"
#include "RenderDevice.h"
#include "SeekControl.h"



namespace media
{

	FileStreamDevice::FileStreamDevice(int index, HWND hVideo, HWND hEvent, delegate callback)
		: IStreamDevice(index, hVideo, hEvent, callback),
		RefCount(1),
		pReader(NULL), pRender(NULL)
	{
		::InitializeCriticalSection(&csSync);
		drawPtr = &FileStreamDevice::DummyDrawFunc;
	}


	FileStreamDevice::~FileStreamDevice()
	{
		::DeleteCriticalSection(&csSync);
	}


	HRESULT FileStreamDevice::CreateInstance(int index, HWND hVideo, HWND hEvent, PCWSTR pszAddress, delegate callback, IStreamDevice **ppReader)
	{
		assert(hVideo != NULL);
		assert(hEvent != NULL);

		if (ppReader == NULL)
			return E_INVALIDARG;

		FileStreamDevice *pDevice = new (std::nothrow) FileStreamDevice(index, hVideo, hEvent, callback);

		if (pDevice == NULL)
			return E_OUTOFMEMORY;

		HRESULT hr = pDevice->Initialize(pszAddress);


		if (SUCCEEDED(hr))
		{
			*ppReader = pDevice;
			(*ppReader)->AddRef();
		}

		SafeRelease(&pDevice);

		return hr;
	}

	IDirect3DSurface9 *FileStreamDevice::GetDrawSurface()
	{
		return pRender->GetSurface();
	}

	HRESULT FileStreamDevice::Next()
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

	HRESULT FileStreamDevice::Close()
	{
		::EnterCriticalSection(&csSync);
		isClosed = true;
		drawPtr = &FileStreamDevice::DummyDrawFunc;
		//order is important!
		SafeRelease(&pReader);

		if (pRender)
		{
			delete pRender;
			pRender = NULL;
		}
		::LeaveCriticalSection(&csSync);

		return S_OK;
	}

	//--- IMFSourceReaderCallback Impl ------------------------------------
	STDMETHODIMP FileStreamDevice::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample)
	{
		HRESULT hr = S_OK;
		IMFMediaBuffer *pBuffer = NULL;

		EnterCriticalSection(&csSync);

		if (FAILED(hrStatus))
		{
			hr = hrStatus;
		}

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			reader::SeekControl seekControl(pReader);
			if (seekControl.CanSeek())
			{
				seekControl.SetPosition();
			}
		}

		if (SUCCEEDED(hr))
		{
			long sleep = counter.Tick();
			if (sleep > 0)
				::Sleep(sleep);

			if (pSample)
			{
				// Get the video frame buffer from the sample.
				hr = pSample->GetBufferByIndex(0, &pBuffer);

				// Draw the frame.
				if (SUCCEEDED(hr))
				{
					IDirect3DSurface9 *pSurface = NULL;
					hr = MFGetService(pBuffer, MR_BUFFER_SERVICE, IID_PPV_ARGS(&pSurface));
					(this->*drawPtr)(pSurface);

					SafeRelease(&pSurface);
				}
				SafeRelease(&pBuffer);
			}
		}

		// Request the next frame.
		if (SUCCEEDED(hr))
		{
			//hr = (HRESULT)::PostMessage(hWndEvent, STREAMER_SAMPLE_READY, counter.fps, index);
			DoRender();
		}
		LeaveCriticalSection(&csSync);

		return hr;
	}

	//--- IUnknown Impl----------------------------------------------------
	ULONG FileStreamDevice::AddRef()
	{
		return InterlockedIncrement(&RefCount);
	}

	ULONG FileStreamDevice::Release()
	{
		// For thread safety, return a temporary variable.
		long count = InterlockedDecrement(&RefCount);
		if (count == 0)
		{
			delete this;
		}
		return count;
	}

	HRESULT FileStreamDevice::QueryInterface(REFIID riid, void** ppv)
	{
#pragma warning (disable: 4838)
		static const QITAB qit[] =
		{
			QITABENT(FileStreamDevice, IMFSourceReaderCallback),
			{ 0 },
		};
#pragma warning (default: 4838)
		return QISearch(this, qit, riid, ppv);
	}


	//--- private members -------------------------------------------------
	HRESULT FileStreamDevice::Initialize(PCWSTR pszAddress)
	{
		//create stream
		IMFMediaSource *pMediaSource = NULL;

		HRESULT hr = S_OK;

		pRender = new (std::nothrow) presenter::RenderDevice();
		if (pRender == NULL)
			return E_OUTOFMEMORY;

		hr = pRender->CreateDevice(hWndVideo);
		if (FAILED(hr))
			return hr;

		hr = CreateMediaSource(pszAddress, NULL, &pMediaSource);

		if (SUCCEEDED(hr))
		{
			IMFAttributes   *pAttributes = NULL;

			if (SUCCEEDED(hr))
			{
				hr = MFCreateAttributes(&pAttributes, 2);
			}
			if (SUCCEEDED(hr))
			{
				hr = pAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, pRender->pD3DManager->pManager);
			}

			// Set the callback pointer.
			if (SUCCEEDED(hr))
			{
				hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
			}

			if (SUCCEEDED(hr))
			{
				hr = MFCreateSourceReaderFromMediaSource(
					pMediaSource,
					pAttributes,
					&pReader
				);

				SafeRelease(&pAttributes);
			}

			// Try to find a suitable output type.
			if (SUCCEEDED(hr))
			{
				IMFMediaType    *pType = NULL;

				for (DWORD i = 0; ; i++)
				{
					hr = pReader->GetNativeMediaType(
						(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
						i,
						&pType
					);

					if (FAILED(hr))
						break;

					hr = TryMediaType(pType);
					SafeRelease(&pType);

					if (SUCCEEDED(hr))
					{
						//Play with FrmaRate
						//reader::FrameRateControl frControl(pReader);
						//float f = frControl.GetRate();
						//float rMin = 0, rMax = 0;
						//frControl.GetRateRange(rMin, rMax);
						//float nearestR = frControl.IsRateSupported(rMax);
						//frControl.SetRate(nearestR);

						// Found an output type.
						//hr = Next();	//ask for the first sample
						drawPtr = &FileStreamDevice::RealDrawFunc;
						assert(hr == S_OK);
						counter.Start();
						break;
					}
				}
			}


			if (FAILED(hr))
			{
				if (pMediaSource)
				{
					pMediaSource->Shutdown();
					// NOTE: The source reader shuts down the media source
					// by default, but we might not have gotten that far.
				}

				Close();
			}

			SafeRelease(&pMediaSource);
		}

		return hr;
	}

	HRESULT FileStreamDevice::CreateMediaSource(LPCWSTR szAddress, IPropertyStore *pConfig, IMFMediaSource **ppMediaSource)
	{
		IMFSourceResolver* pSourceResolver = NULL;
		IUnknown* pSource = NULL;

		// Create the source resolver.
		HRESULT hr = MFCreateSourceResolver(&pSourceResolver);

		// Use the source resolver to create the media source.
		if (SUCCEEDED(hr))
		{
			MF_OBJECT_TYPE ObjectType;

			hr = pSourceResolver->CreateObjectFromURL(
				szAddress,
				MF_RESOLUTION_MEDIASOURCE,  // Create a media source.
				pConfig,                    // Configuration properties.
				&ObjectType,                // Receives the object type. 
				&pSource
			);

			assert(ObjectType == MF_OBJECT_MEDIASOURCE);

		}

		if (SUCCEEDED(hr))
		{
			hr = pSource->QueryInterface(IID_PPV_ARGS(ppMediaSource));
		}

		SafeRelease(&pSourceResolver);
		SafeRelease(&pSource);
		return hr;
	}

	HRESULT FileStreamDevice::TryMediaType(IMFMediaType *pType)
	{
		HRESULT hr = S_OK;
		hr = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);

		assert(hr == S_OK);
		if (FAILED(hr))
			return hr;

		hr = pReader->SetCurrentMediaType(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			NULL,
			pType);

		if (FAILED(hr))
			return hr;

		//frame size
		hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
		if (FAILED(hr))
			return hr;

		//frame rate
		UINT32 numerator = 0;
		UINT32 denominator = 0;
		hr = ::MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &numerator, &denominator);
		if (SUCCEEDED(hr))
			frameRate = numerator * denominator;

		hr = pRender->CreateVAService(width, height, MFVideoFormat_YUY2);
		return hr;
	}

	inline void FileStreamDevice::RealDrawFunc(IDirect3DSurface9 *pSurface)
	{
		pRender->Draw(pSurface);
	}
	inline void FileStreamDevice::DummyDrawFunc(IDirect3DSurface9 *pSurface)
	{ //for performance optimization, see comment in the header
	};

}
