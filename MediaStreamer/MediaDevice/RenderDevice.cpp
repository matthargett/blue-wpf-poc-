#include "StdAfx.h"
#include "RenderDevice.h"
#include "D3DManager.h"
#include "VideoService.h"

namespace media
{
	namespace presenter
	{
		RenderDevice::RenderDevice()
			: hWnd(NULL),
			pSurface(NULL),
			pDX(NULL),
			pD3DManager(NULL),
			format(D3DFMT_UNKNOWN),
			pVideoService(NULL)
		{

		}

		RenderDevice::~RenderDevice()
		{
			DestroyDevice();
		}

		HRESULT RenderDevice::CreateDevice(HWND hWnd)
		{
			if (pD3DManager)
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
			IDirect3DDevice9 *pDevice = NULL;

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
			pp.Flags = D3DPRESENTFLAG_VIDEO |
				D3DPRESENTFLAG_DEVICECLIP /*|
				D3DPRESENTFLAG_LOCKABLE_BACKBUFFER*/;
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
			hr = D3DManager::CreateInstance(pDevice, &pD3DManager);
			this->hWnd = hWnd;
			d3dParams = pp;

		done:
			SafeRelease(&pDeviceEx);
			SafeRelease(&pDevice);
			return hr;
		}

		IDirect3DSurface9 *RenderDevice::GetSurface() const
		{
			assert(pSurface != NULL);
			return pSurface;
		}
		HRESULT RenderDevice::CreateVAService(UINT width, UINT height, GUID videoFormat)
		{
			HRESULT hr = S_OK;
			d3dParams.BackBufferWidth = width;
			d3dParams.BackBufferHeight = height;

			//Create surface for drawing
			hr = CreateSurface();

			if (FAILED(hr))
				return hr;

			//Create DXVA
			//TODO: VideoFormat shoudl be initialized dynamically
			pVideoService = new VideoService();
			hr = pVideoService->Create(width, height, pD3DManager);
			return hr;
		}

		void RenderDevice::Draw(IDirect3DSurface9 *pSampleSurface)
		{
			assert(pSurface != NULL);
			assert(pSampleSurface != NULL);
			assert(pVideoService != NULL);

			pVideoService->Draw(pSurface, pSampleSurface);
		}

		void RenderDevice::DestroyDevice()
		{
			SafeRelease(&pSurface);

			delete pVideoService;
			pVideoService = NULL;

			delete pD3DManager;
			pD3DManager = NULL;

			SafeRelease(&pDX);
		}

		HRESULT RenderDevice::CreateSurface()
		{
			HRESULT hr = S_OK;

			SafeRelease(&pSurface);

			IDirect3DDevice9 *pDevice = NULL;
			pDevice = pD3DManager->GetDevice();

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
			pD3DManager->FreeDevice(&pDevice);

			return hr;

		}
		D3DSURFACE_DESC RenderDevice::GetSurfaceDescription(IDirect3DSurface9 *pSurface)
		{
			D3DSURFACE_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			assert(pSurface != NULL);
			HRESULT hr = pSurface->GetDesc(&desc);
			assert(hr == S_OK);
			return desc;
		}
	}
}