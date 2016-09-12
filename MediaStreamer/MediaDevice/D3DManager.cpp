#include "stdafx.h"
#include "D3DManager.h"

namespace media
{
	namespace presenter
	{

		D3DManager::D3DManager()
			: token(0), hDevice(NULL),
			pManager(NULL)
		{
		}


		D3DManager::~D3DManager()
		{
			SafeRelease(&pManager);
		}

		HRESULT D3DManager::CreateInstance(IDirect3DDevice9 *pDevice, D3DManager **ppD3DManager)
		{
			*ppD3DManager = new (std::nothrow) D3DManager();

			if (ppD3DManager == NULL)
				return E_OUTOFMEMORY;

			UINT resetToken = 0;
			IDirect3DDeviceManager9 *pTmpManager = NULL;

			HRESULT hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &pTmpManager);

			if (FAILED(hr))
				goto done;

			hr = pTmpManager->ResetDevice(pDevice, resetToken);

			if (FAILED(hr))
				goto done;


			(*ppD3DManager)->pManager = pTmpManager;
			(*ppD3DManager)->pManager->AddRef();
			(*ppD3DManager)->token = resetToken;


		done:
			SafeRelease(&pTmpManager);
			return hr;
		}

		IDirect3DDevice9 *D3DManager::GetDevice()
		{
			IDirect3DDevice9 *pDevice = NULL;

			HRESULT hr = pManager->OpenDeviceHandle(&hDevice);

			if (SUCCEEDED(hr))
			{
				hr = pManager->LockDevice(hDevice, &pDevice, TRUE); //!!! Reference incremented
			}

			if (hr == DXVA2_E_NEW_VIDEO_DEVICE)
			{
				// Invalid device handle. Try to open a new device handle.
				hr = pManager->CloseDeviceHandle(hDevice);

				if (SUCCEEDED(hr))
				{
					hr = pManager->OpenDeviceHandle(&hDevice);
				}

				// Try to lock the device again.
				if (SUCCEEDED(hr))
				{
					hr = pManager->LockDevice(hDevice, &pDevice, TRUE);
				}
			}

			if (FAILED(hr))
			{
				hDevice = NULL;
				return NULL;
			}
			return pDevice;
		}

		void D3DManager::FreeDevice(IDirect3DDevice9 **ppDevice)
		{
			SafeRelease(ppDevice);

			if (!hDevice)
				return;

			HRESULT hr = pManager->UnlockDevice(hDevice, TRUE);
			hr = pManager->CloseDeviceHandle(hDevice);
			hDevice = NULL;
		}
	}
}