#pragma once
#include "Dxva2api.h"
namespace media
{
	namespace presenter
	{
		class D3DManager
		{
		public:
			static HRESULT CreateInstance(IDirect3DDevice9 *pDevice, D3DManager **ppD3DManager);
			~D3DManager();

			IDirect3DDevice9 *GetDevice(); //Lock device for usage, should be freed!
			void FreeDevice(IDirect3DDevice9 **pDevice);	//frees device after usage
			IDirect3DDeviceManager9* pManager;
		protected:
			D3DManager();
		private:
			UINT token;

			HANDLE hDevice;
		};
	}
}

