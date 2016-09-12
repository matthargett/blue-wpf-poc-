//////////////////////////////////////////////////////////////////////////
//
// device.cpp: Manages the Direct3D device
// 
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Device.h"
#include "BufferLock.h"

namespace media
{
	namespace presenter
	{
		const DWORD NUM_BACK_BUFFERS = 1;

		namespace _impl
		{
			HRESULT GetDefaultStride(IMFMediaType *pType, LONG *plStride);
		}

		//-------------------------------------------------------------------
		// Constructor
		//-------------------------------------------------------------------

		DrawDevice::DrawDevice() :
			m_hwnd(NULL),
			m_pD3D(NULL),
			m_pDevice(NULL),
			pSurface(NULL),
			m_format(D3DFMT_UNKNOWN),
			m_width(0),
			m_height(0),
			m_lDefaultStride(0),
			m_convertFn(NULL)
		{
			ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
		}


		//-------------------------------------------------------------------
		// Destructor
		//-------------------------------------------------------------------

		DrawDevice::~DrawDevice()
		{
			DestroyDevice();
		}

		//-------------------------------------------------------------------
		// CreateDevice
		//
		// Create the Direct3D device.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::CreateDevice(HWND hwnd)
		{
			if (m_pDevice)
			{
				return S_OK;
			}

			HRESULT hr = S_OK;
			// Create the Direct3D object.
			if (m_pD3D == NULL)
			{
#ifdef DX9EX
				hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D);
#else
				m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
#endif

				if (m_pD3D == NULL)
				{
					return E_FAIL;
				}
			}


			D3DPRESENT_PARAMETERS pp = { 0 };
			D3DDISPLAYMODE mode = { 0 };
			IDirect3DDevice9Ex *pDeviceEx = NULL;

			hr = m_pD3D->GetAdapterDisplayMode(
				D3DADAPTER_DEFAULT,
				&mode
			);

			if (FAILED(hr)) { goto done; }

			hr = m_pD3D->CheckDeviceType(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				mode.Format,
				D3DFMT_X8R8G8B8,
				TRUE    // windowed
			);

			if (FAILED(hr)) { goto done; }

			pp.BackBufferFormat = D3DFMT_X8R8G8B8;
			pp.SwapEffect = D3DSWAPEFFECT_FLIP;//pp.SwapEffect = D3DSWAPEFFECT_DISCARD; // D3DSWAPEFFECT_COPY;
			pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;   /*D3DPRESENT_INTERVAL_IMMIDIATE*/
			pp.Windowed = TRUE;
			pp.hDeviceWindow = m_hwnd;
			pp.Flags =
				D3DPRESENTFLAG_VIDEO /*| D3DPRESENTFLAG_DEVICECLIP |
				D3DPRESENTFLAG_LOCKABLE_BACKBUFFER*/;
			pp.BackBufferCount = NUM_BACK_BUFFERS;
#ifdef DX9EX
			hr = m_pD3D->CreateDeviceEx(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hwnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING |
				D3DCREATE_FPU_PRESERVE | /*D3DCREATE_NOWINDOWCHANGES*/
				D3DCREATE_MULTITHREADED,
				&pp,
				NULL,
				&pDeviceEx);

			if (FAILED(hr)) { goto done; }
			hr = pDeviceEx->QueryInterface(__uuidof(IDirect3DDevice9), reinterpret_cast<void**>(&m_pDevice));
#else
			hr = m_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				hwnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING |
				D3DCREATE_FPU_PRESERVE | /*D3DCREATE_NOWINDOWCHANGES*/
				D3DCREATE_MULTITHREADED,
				&pp,
				&m_pDevice);
#endif
			m_hwnd = hwnd;
			m_d3dpp = pp;

		done:
			SafeRelease(&pDeviceEx);
			return hr;
		}

		//-------------------------------------------------------------------
		// SetConversionFunction
		//
		// Set the conversion function for the specified video format.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::SetConversionFunction(REFGUID subtype)
		{
			m_convertFn = GetTransformFuntion(subtype);

			if (m_convertFn == NULL)
				return MF_E_INVALIDMEDIATYPE;
			else
				return S_OK;
		}


		//-------------------------------------------------------------------
		// SetVideoType
		//
		// Set the video format.  
		//-------------------------------------------------------------------

		HRESULT DrawDevice::SetVideoType(IMFMediaType *pType)
		{
			HRESULT hr = S_OK;
			GUID subtype = { 0 };
			MFRatio PAR = { 0 };

			// Find the video subtype.
			hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);

			if (FAILED(hr)) { goto done; }

			// Choose a conversion function.
			// (This also validates the format type.)
			hr = SetConversionFunction(subtype);

			if (FAILED(hr)) { goto done; }

			//--------------------------------------------------------------------------
			// Get some video attributes.

			// Get the frame size.
			hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &m_width, &m_height);
			if (FAILED(hr)) { goto done; }

			// Get the image stride.
			hr = _impl::GetDefaultStride(pType, &m_lDefaultStride);
			if (FAILED(hr)) { goto done; }

			m_format = (D3DFORMAT)subtype.Data1;

			// Create Direct3D swap chains.
			hr = CreateSurface();

		done:
			if (FAILED(hr))
			{
				m_format = D3DFMT_UNKNOWN;
				m_convertFn = NULL;
			}
			return hr;
		}


		//-------------------------------------------------------------------
		// CreateSurface
		//
		// Create Direct3D swap chains.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::CreateSurface()
		{
			HRESULT hr = S_OK;

			D3DPRESENT_PARAMETERS pp = { 0 };

			SafeRelease(&pSurface);

			hr = m_pDevice->CreateRenderTarget(
				m_width,
				m_height,
				D3DFMT_X8R8G8B8,
				D3DMULTISAMPLE_NONE, //static_cast<D3DMULTISAMPLE_TYPE>(0),
				0,
				TRUE,
				&pSurface,
				NULL);

			assert(hr == S_OK);

			hr = m_pDevice->SetRenderTarget(0, pSurface);


			return hr;
		}


		//-------------------------------------------------------------------
		// DrawFrame
		//
		// Draw the video frame.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::DrawFrame(IMFMediaBuffer *pBuffer)
		{
			if (m_convertFn == NULL)
			{
				return MF_E_INVALIDREQUEST;
			}

			HRESULT hr = S_OK;
			BYTE *pbScanline0 = NULL;
			LONG lStride = 0;
			D3DLOCKED_RECT lr;

			if (m_pDevice == NULL || pSurface == NULL)
			{
				return S_OK;
			}

			_impl::VideoBufferLock buffer(pBuffer);    // Helper object to lock the video buffer.

			hr = TestCooperativeLevel();

			if (FAILED(hr)) { goto done; }

			// Lock the video buffer. This method returns a pointer to the first scan
			// line in the image, and the stride in bytes.

			hr = buffer.LockBuffer(m_lDefaultStride, m_height, &pbScanline0, &lStride);
			if (FAILED(hr)) { goto done; }

			// Lock the swap-chain surface.
			hr = pSurface->LockRect(&lr, NULL, D3DLOCK_NOSYSLOCK);
			if (FAILED(hr)) { goto done; }


			// Convert the frame. This also copies it to the Direct3D surface.

			m_convertFn(
				(BYTE*)lr.pBits,
				lr.Pitch,
				pbScanline0,
				lStride,
				m_width,
				m_height
			);

			hr = pSurface->UnlockRect();


		done:
			return hr;
		}

		//-------------------------------------------------------------------
		// TestCooperativeLevel
		//
		// Test the cooperative-level status of the Direct3D device.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::TestCooperativeLevel()
		{
			if (m_pDevice == NULL)
			{
				return E_FAIL;
			}

			HRESULT hr = S_OK;

			// Check the current status of D3D9 device.
			hr = m_pDevice->TestCooperativeLevel();

			switch (hr)
			{
			case D3D_OK:
				break;

			case D3DERR_DEVICELOST:
				hr = S_OK;

			case D3DERR_DEVICENOTRESET:
				hr = ResetDevice();
				break;

			default:
				// Some other failure.
				break;
			}

			return hr;
		}


		//-------------------------------------------------------------------
		// ResetDevice
		//
		// Resets the Direct3D device.
		//-------------------------------------------------------------------

		HRESULT DrawDevice::ResetDevice()
		{
			HRESULT hr = S_OK;

			if (m_pDevice)
			{
				D3DPRESENT_PARAMETERS d3dpp = m_d3dpp;

				hr = m_pDevice->Reset(&d3dpp);

				if (FAILED(hr))
				{
					DestroyDevice();
				}
			}

			if (m_pDevice == NULL)
			{
				hr = CreateDevice(m_hwnd);

				if (FAILED(hr)) { goto done; }
			}

			if ((pSurface == NULL) && (m_format != D3DFMT_UNKNOWN))
			{
				hr = CreateSurface();

				if (FAILED(hr)) { goto done; }
			}

		done:

			return hr;
		}


		//-------------------------------------------------------------------
		// DestroyDevice 
		//
		// Release all Direct3D resources.
		//-------------------------------------------------------------------

		void DrawDevice::DestroyDevice()
		{
			SafeRelease(&pSurface);
			SafeRelease(&m_pDevice);
			SafeRelease(&m_pD3D);
		}


		namespace _impl
		{

			//-----------------------------------------------------------------------------
			// GetDefaultStride
			//
			// Gets the default stride for a video frame, assuming no extra padding bytes.
			//
			//-----------------------------------------------------------------------------

			HRESULT GetDefaultStride(IMFMediaType *pType, LONG *plStride)
			{
				LONG lStride = 0;

				// Try to get the default stride from the media type.
				HRESULT hr = pType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&lStride);
				if (FAILED(hr))
				{
					// Attribute not set. Try to calculate the default stride.
					GUID subtype = GUID_NULL;

					UINT32 width = 0;
					UINT32 height = 0;

					// Get the subtype and the image size.
					hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);
					if (SUCCEEDED(hr))
					{
						hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
					}
					if (SUCCEEDED(hr))
					{
						hr = MFGetStrideForBitmapInfoHeader(subtype.Data1, width, &lStride);
					}

					// Set the attribute for later reference.
					if (SUCCEEDED(hr))
					{
						(void)pType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(lStride));
					}
				}

				if (SUCCEEDED(hr))
				{
					*plStride = lStride;
				}
				return hr;
			}
		}
	}
}