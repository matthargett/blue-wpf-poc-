#pragma once
#include "IStreamDevice.h"
#include "ICamStreamImpl.h"

namespace media
{
	class CamStreamDevice : public IStreamDevice
	{
	public:
		static HRESULT CreateInstance(int index, HWND hVideo, HWND hEvent, int camIndex, int prefferableMode, delegate callback, IStreamDevice **ppReader);

		// IStreamDevice methods
		virtual HRESULT Close();
		virtual HRESULT	Next();
		virtual IDirect3DSurface9 *GetDrawSurface();
		virtual bool IsHwAccelerated() const
		{
			if (pDevice)
				return pDevice->IsAccelerated();
			return false;
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
		CamStreamDevice(int index, HWND hVideo, HWND hEvent, int prefferabelMode, delegate callback);
		virtual ~CamStreamDevice();
		reader::impl_::ICamStreamImpl *pDevice;


	private:
		HRESULT Initialize(int camIndex);
		HRESULT ChooseDevice(int camIndex);
		HRESULT SetDevice(IMFActivate *pActivate);
		HRESULT SetStaticImage();

		long RefCount;
		CRITICAL_SECTION csSync;

		WCHAR *pwszSymbolicLink;
		UINT32 SymbolicLinkCount;

		int prefferableMode; // FrameSize that should be seleected in webcam (height x width @ fps)
	};
}

