#pragma once
namespace media
{
	namespace presenter
	{
		class D3DManager;

		class VideoService
		{
		public:
			VideoService();
			~VideoService();

			HRESULT Create(UINT width, UINT heigth, D3DManager *pManager);
			void Draw(IDirect3DSurface9* pDrawSurface, IDirect3DSurface9 *pSample);

		private:
			IDirectXVideoProcessorService *pVideoService;
			IDirectXVideoProcessor *pProcessor;
			DXVA2_VideoDesc          g_VideoDesc;
			DXVA2_VideoProcessBltParams blt;
			DXVA2_VideoSample sample;

			bool CreateDevice(REFGUID guid);
			DXVA2_AYUVSample16 GetBackgroundColor() const;
		};
	}
}

