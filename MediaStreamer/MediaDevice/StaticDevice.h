#pragma once
namespace media
{
	namespace presenter
	{
		class StaticDevice
		{
		public:
			StaticDevice();
			~StaticDevice();
			IDirect3DSurface9 *GetSurface() const;
			HRESULT CreateDevice(HWND hWnd);
			void Draw(IDirect3DSurface9 *pSampleSurface);
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
			IDirect3DDevice9 *pDevice;
			IDirect3DSurface9 *pSurface;
			D3DPRESENT_PARAMETERS  d3dParams = { 0 };
			D3DFORMAT format;

			HRESULT TestCooperativeLevel();
			HRESULT CreateSurface();
		};
	}
}
