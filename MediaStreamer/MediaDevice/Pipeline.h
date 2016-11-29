#pragma once

namespace media
{
	class IStreamDevice;

	class Pipeline
	{
	public:
		static HRESULT CreateFileInstance(int index, HWND hVideo, PCWSTR pszAddress, delegate callback, Pipeline **pPipe);
		static HRESULT CreateCamInstance(int index, HWND hVideo, int camIndex, int prefferableMode, delegate callback, Pipeline **pPipe);

		~Pipeline();
		IDirect3DSurface9 *GetSurface();
		void GetFrameSize(int &width, int &height) const;
		bool IsHwAccelerated()const;
		void Next();
	protected:
		Pipeline();
	private:
		IStreamDevice *pCapture;
	};
}
