#include "stdafx.h"
#include "CamStreamDevice.h"
#include "CamStreamFactory.h"

namespace media
{

	CamStreamDevice::CamStreamDevice(int index, HWND hVideo, HWND hEvent, delegate callback)
		: IStreamDevice(index, hVideo, hEvent, callback),
		RefCount(1),
		pDevice(NULL),
		pwszSymbolicLink(NULL),
		SymbolicLinkCount(0)
	{
		::InitializeCriticalSection(&csSync);
	}


	CamStreamDevice::~CamStreamDevice()
	{
		//Render.DestroyDevice(); Called from destructir of Render
		::DeleteCriticalSection(&csSync);
	}

	HRESULT CamStreamDevice::CreateInstance(int index, HWND hVideo, HWND hEvent, int camIndex, delegate callback, IStreamDevice **ppReader)
	{
		assert(hVideo != NULL);
		assert(hEvent != NULL);

		if (ppReader == NULL)
			return E_INVALIDARG;

		CamStreamDevice *pDevice = new (std::nothrow) CamStreamDevice(index, hVideo, hEvent, callback);

		if (pDevice == NULL)
			return E_OUTOFMEMORY;

		HRESULT hr = pDevice->Initialize(camIndex);

		if (SUCCEEDED(hr))
		{
			*ppReader = pDevice;
			(*ppReader)->AddRef();
		}

		SafeRelease(&pDevice);

		return hr;
	}

	IDirect3DSurface9 *CamStreamDevice::GetDrawSurface()
	{
		return pDevice->GetDrawSurface();
	}

	HRESULT CamStreamDevice::Next()
	{
		return pDevice->Next();
	}

	HRESULT CamStreamDevice::Close()
	{
		::EnterCriticalSection(&csSync);
		isClosed = true;
		if (pDevice)
			delete pDevice;

		pDevice = NULL;
		CoTaskMemFree(pwszSymbolicLink);
		pwszSymbolicLink = NULL;
		SymbolicLinkCount = 0;

		::LeaveCriticalSection(&csSync);

		return S_OK;
	}



	//--- IMFSourceReaderCallback Impl ------------------------------------
	HRESULT CamStreamDevice::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample)
	{
		HRESULT hr = S_OK;
		IMFMediaBuffer *pBuffer = NULL;

		BOOL isVacant = TryEnterCriticalSection(&csSync);

		if (!isVacant) //if we could not acquire CS it should be about closing
			return S_OK;

		if (FAILED(hrStatus))
		{
			hr = hrStatus;
		}

		if (isClosed)
			hr = E_ABORT;

		if (SUCCEEDED(hr))
		{
			counter.Tick();
			if (pSample)
			{
				// Get the video frame buffer from the sample.
				hr = pSample->GetBufferByIndex(0, &pBuffer);

				// Draw the frame.
				if (SUCCEEDED(hr))
				{
					hr = pDevice->DrawSample(pBuffer);
				}
				SafeRelease(&pBuffer);
			}
		}

		// Request the next frame.
		if (SUCCEEDED(hr))
		{
			DoRender();
		}
		LeaveCriticalSection(&csSync);

		//TODO: error handling
		//if (FAILED(hr))
		//{
		//	NotifyError(hr);
		//}

		return hr;

	}


	//--- IUnknown Impl----------------------------------------------------
	ULONG CamStreamDevice::AddRef()
	{
		return InterlockedIncrement(&RefCount);
	}

	ULONG CamStreamDevice::Release()
	{
		// For thread safety, return a temporary variable.
		long count = InterlockedDecrement(&RefCount);
		if (count == 0)
		{
			delete this;
		}
		return count;
	}

	HRESULT CamStreamDevice::QueryInterface(REFIID riid, void** ppv)
	{
#pragma warning (disable: 4838)
		static const QITAB qit[] =
		{
			QITABENT(CamStreamDevice, IMFSourceReaderCallback),
			{ 0 },
		};
#pragma warning (default: 4838)
		return QISearch(this, qit, riid, ppv);
	}


	//--- private members -------------------------------------------------
	HRESULT CamStreamDevice::Initialize(int camIndex)
	{
		HRESULT hr = S_OK;
		if (SUCCEEDED(hr))
			hr = ChooseDevice(camIndex);
		return hr;
	}

	HRESULT CamStreamDevice::ChooseDevice(int camIndex)
	{
		HRESULT hr = S_OK;
		IMFActivate **ppDevices = NULL;
		UINT32 count = 0;

		IMFAttributes *pAttributes = NULL;
		hr = MFCreateAttributes(&pAttributes, 1);
		if (FAILED(hr)) { goto done; }

		hr = pAttributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
		if (FAILED(hr)) { goto done; }

		hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
		if (FAILED(hr)) { goto done; }

		if ((int)count > camIndex)
		{
			hr = SetDevice(ppDevices[camIndex]);
		}
		else
		{
			hr = SetStaticImage(); // can be used for test purposes, (mocking)
		}
		//TODO: error handling no device
	done:

		SafeRelease(&pAttributes);

		if (&ppDevices)
		{
			for (DWORD i = 0; i < count; i++)
			{
				SafeRelease(&ppDevices[i]);
			}
			CoTaskMemFree(ppDevices);
		}

		//TODO: error handling
		//if (FAILED(hr))
		//{
			//ShowErrorMessage(L"Cannot create a video capture device", hr);
			//::MessageBox(NULL, L"Cannot create a video capture device", L"MediaLib", MB_OK | MB_ICONEXCLAMATION);
		//}

		return hr;
	}

	HRESULT CamStreamDevice::SetStaticImage()
	{
		pDevice = reader::impl_::CreateStaticDevice(hWndVideo);
		if (pDevice == NULL)
			return E_UNEXPECTED;

		HRESULT hr = pDevice->CreateSourceReader(NULL, this);
		width = pDevice->GetFrameWidth();
		height = pDevice->GetFrameHeight();
		counter.Start();
		
		return hr;
	}

	HRESULT CamStreamDevice::SetDevice(IMFActivate *pActivate)
	{
		HRESULT hr = S_OK;
		IMFMediaSource  *pSource = NULL;

		EnterCriticalSection(&csSync);

		// Release the current device, if any.
		hr = Close();
		isClosed = false;

		// Create the media source for the device.
		if (SUCCEEDED(hr))
		{
			//!!! Leak, handle still not closed
			// https://social.msdn.microsoft.com/Forums/en-US/c4ac1950-b8de-4622-bf46-19f897706188/imfactivateactivateobject-memory-leak?forum=mediafoundationdevelopment
			// /HKLM\SYSTEM\ControlSet001\Control\DeviceClasses\{65E8773D-8F56-11D0-A3B9-00A0C9223196}\##?#USB#VID_046D&PID_0819&MI_00#7&35650338&1&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\#GLOBAL\Device Parameters
			hr = pActivate->ActivateObject(__uuidof(IMFMediaSource), (void**)&pSource);
		}

		// Get the symbolic link.
		if (SUCCEEDED(hr))
		{
			hr = pActivate->GetAllocatedString(
				MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
				&pwszSymbolicLink,
				&SymbolicLinkCount
			);
		}


		BYTE MediaType[64] = { 0 };
		UINT32 mediaSize = 0;
		hr = pActivate->GetBlob(MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE, &MediaType[0], 64, &mediaSize);

		if (FAILED(hr))
		{
			pDevice = reader::impl_::CreateCamStreamSwDevice(hWndVideo);
		}
		else
		{
			pDevice = reader::impl_::CreateCamStreamDevice(&MediaType[0], mediaSize, hWndVideo);
		}

		hr = pDevice->CreateSourceReader(pSource, this);


		if (FAILED(hr))
		{
			delete pDevice;
			pDevice = NULL;
		}


		if (pDevice != NULL)
		{
			width = pDevice->GetFrameWidth();
			height = pDevice->GetFrameHeight();
			counter.Start();
		}

		SafeRelease(&pSource);
		LeaveCriticalSection(&csSync);
		return hr;
	}

}
