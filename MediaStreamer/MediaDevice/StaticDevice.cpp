#include "StdAfx.h"
#include "StaticDevice.h"

namespace media
{
	namespace presenter
	{
		StaticDevice::StaticDevice()
			: hWnd(NULL),
			pSurface(NULL),
			pDX(NULL), pDevice(NULL),
			format(D3DFMT_UNKNOWN)
		{

		}

		StaticDevice::~StaticDevice()
		{
			DestroyDevice();
		}

		HRESULT StaticDevice::CreateDevice(HWND hWnd)
		{
			if (pDevice)
				return S_OK;

			HRESULT hr = D3D_OK;
			if (NULL == pDX)
			{
#ifdef DX9EX
				hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pDX);
#else
				pDX = Direct3DCreate9(D3D_SDK_VERSION);
#endif

				assert(pDX != NULL);
				if (pDX == NULL)
					return D3DERR_DRIVERINTERNALERROR;
			}

			D3DPRESENT_PARAMETERS pp = { 0 };
			D3DDISPLAYMODE mode = { 0 };
			IDirect3DDevice9Ex *pDeviceEx = NULL;

			hr = pDX->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

			if (FAILED(hr))
				goto done;

			hr = pDX->CheckDeviceType(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				mode.Format,
				D3DFMT_X8R8G8B8,
				TRUE);    // windowed

			if (FAILED(hr))
				goto done;

			pp.BackBufferFormat = D3DFMT_X8R8G8B8;
			pp.SwapEffect = D3DSWAPEFFECT_FLIP;
			pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			pp.Windowed = TRUE;
			pp.hDeviceWindow = hWnd;
			pp.Flags = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_DEVICECLIP;
			pp.BackBufferCount = NumberBackBuffers;
#ifdef DX9EX
			hr = pDX->CreateDeviceEx(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING |
				D3DCREATE_FPU_PRESERVE |
				D3DCREATE_MULTITHREADED,
				&pp,
				NULL,
				&pDeviceEx);

			if (FAILED(hr))
				goto done;
			pDeviceEx->QueryInterface(__uuidof(IDirect3DDevice9), reinterpret_cast<void**>(&pDevice));
#else
			hr = pDX->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING |
				D3DCREATE_FPU_PRESERVE |
				D3DCREATE_MULTITHREADED,
				&pp,
				&pDevice);
#endif
			this->hWnd = hWnd;
			d3dParams = pp;
			d3dParams.BackBufferWidth = 320;
			d3dParams.BackBufferHeight = 240;

			hr = CreateSurface();

		done:
			SafeRelease(&pDeviceEx);
			return hr;
		}

		IDirect3DSurface9 *StaticDevice::GetSurface() const
		{
			assert(pSurface != NULL);
			return pSurface;
		}

		void StaticDevice::Draw(IDirect3DSurface9 *pSampleSurface)
		{
			assert(pSurface != NULL);
			HRESULT hr = S_OK;
		
			static BYTE b = 0xff;
			static BYTE g = 0x10;
			static BYTE r = 0x00;
			hr = pDevice->ColorFill(pSurface, NULL, D3DCOLOR_XRGB(--r, ++g, ++b));
			assert(hr == S_OK);
		}

		void StaticDevice::DestroyDevice()
		{
			SafeRelease(&pSurface);
			SafeRelease(&pDevice);
			SafeRelease(&pDX);
		}

		HRESULT StaticDevice::CreateSurface()
		{
			HRESULT hr = S_OK;

			SafeRelease(&pSurface);

			hr = pDevice->CreateRenderTarget(
				d3dParams.BackBufferWidth,
				d3dParams.BackBufferHeight,
				d3dParams.BackBufferFormat,
				D3DMULTISAMPLE_NONE,
				0,
				FALSE,	//Lockable buffer!
				&pSurface,
				NULL);

			assert(hr == S_OK);
			if (FAILED(hr))
				goto done;

			hr = pDevice->SetRenderTarget(0, pSurface);

		done:
			return hr;

		}
	}
}