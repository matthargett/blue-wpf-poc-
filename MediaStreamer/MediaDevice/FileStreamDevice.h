#pragma once
#include "IStreamDevice.h"

namespace media
{
	namespace presenter
	{
		class RenderDevice;
	}

	class FileStreamDevice : public IStreamDevice
	{
	public:
		static HRESULT CreateInstance(int index, HWND hVideo, HWND hEvent, PCWSTR pszAddress, delegate callback, IStreamDevice **ppReader);

		// IStreamDevice methods
		virtual HRESULT Close();
		virtual HRESULT	Next();
		virtual IDirect3DSurface9 *GetDrawSurface();
		virtual bool IsHwAccelerated() const
		{
			return true; //it always HW accelerated
		}

		// IMFSourceReaderCallback methods
		STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample);
		STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
		{
			return S_OK;	//TODO: check
		}
		STDMETHODIMP OnFlush(DWORD)
		{
			return S_OK;	//TODO: check
		}

		// IUnknown methods
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();


	protected:
		IMFSourceReader		*pReader;
		presenter::RenderDevice *pRender;

		FileStreamDevice(int index, HWND hVideo, HWND hEvent, delegate callback);
		virtual ~FileStreamDevice();

	private:
		HRESULT Initialize(PCWSTR pszAddress);
		HRESULT CreateMediaSource(LPCWSTR szAddress, IPropertyStore *pConfig, IMFMediaSource **ppMediaSource);
		HRESULT TryMediaType(IMFMediaType *pType);

		long RefCount;
		CRITICAL_SECTION csSync;

		//an effective way to get read of multithreading issue during shutdown
		//when SourceReader has stoped already but callbacka are in queue for drowng
		void (FileStreamDevice::*drawPtr)(IDirect3DSurface9 *);
		inline void RealDrawFunc(IDirect3DSurface9 *pSurface);
		inline void DummyDrawFunc(IDirect3DSurface9 *pSurface);
	};
}

