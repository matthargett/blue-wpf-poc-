//////////////////////////////////////////////////////////////////////////
//
// device.h: Manages the Direct3D device
// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "ImageTransformers.h"

namespace media
{
	namespace presenter
	{


	// DrawDevice class

	class DrawDevice
	{
	private:

		HWND                    m_hwnd;
#ifdef DX9EX
		IDirect3D9Ex              *m_pD3D;
#else
		IDirect3D9	*m_pD3D;
#endif
		D3DPRESENT_PARAMETERS   m_d3dpp;

		// Format information
		D3DFORMAT               m_format;
		LONG                    m_lDefaultStride;

		// Drawing
		transformers::IMAGE_TRANSFORM_FN      m_convertFn;    // Function to convert the video to RGB32

	private:

		HRESULT TestCooperativeLevel();
		HRESULT SetConversionFunction(REFGUID subtype);
		HRESULT CreateSurface();
		IDirect3DDevice9        *m_pDevice;

	public:

		DrawDevice();
		virtual ~DrawDevice();

		HRESULT CreateDevice(HWND hwnd);
		HRESULT ResetDevice();
		void    DestroyDevice();

		HRESULT SetVideoType(IMFMediaType *pType);
		HRESULT DrawFrame(IMFMediaBuffer *pBuffer);

		IDirect3DSurface9	*pSurface;
		UINT                    m_width;
		UINT                    m_height;
	};
}
	}
