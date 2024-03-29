#pragma once
#include "D3DManager.h"

namespace media
{
	namespace presenter
	{
		
		class VideoService;

		class RenderDevice
		{
		public:
			RenderDevice();
			~RenderDevice();
			D3DManager*		pD3DManager;
			IDirect3DSurface9 *GetSurface() const;
			HRESULT CreateDevice(HWND hWnd);
			HRESULT CreateVAService(UINT width, UINT height, GUID videoFormat);
			void Draw(IDirect3DSurface9 *pSampleSurface);
			HRESULT ResetDevice();
			void DestroyDevice();

		private:
			static const int NumberBackBuffers = 1;
			HWND hWnd;
			UINT width;
			UINT height;
#ifdef DX9EX
			IDirect3D9Ex *pDX;
#else
			IDirect3D9 *pDX;
#endif
			IDirect3DSurface9 *pSurface;
			D3DPRESENT_PARAMETERS  d3dParams = { 0 };
			D3DFORMAT format;
			VideoService *pVideoService;

			HRESULT TestCooperativeLevel();
			HRESULT CreateSurface();

			static D3DSURFACE_DESC GetSurfaceDescription(IDirect3DSurface9 *pSurface);

		};
	}
}
