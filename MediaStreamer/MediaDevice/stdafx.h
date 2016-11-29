// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <new>
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#include <dxva2api.h>
#include <initguid.h>

#include <mfapi.h>
#include <evr.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

#include <strsafe.h>
#include <assert.h>

#include <Dbt.h>
#include <shlwapi.h>
#include <OleAuto.h>
#include <Propvarutil.h>
#include <process.h>

#define DX9EX

#define STREAM_DESTROY_PIPELINE WM_USER + 0x01 //sends to main thread to destroy stream

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		ULONG count = (*ppT)->Release();
		*ppT = NULL;
	}
}

inline UINT GetRefCounter(IUnknown *pUnk)
{
#ifdef _DEBUG
	pUnk->AddRef();
	return pUnk->Release();
#else
return 0;
#endif
}

#define STREAMER_SAMPLE_READY WM_APP + 0x02	//chained wit WPF part should be changed in both sides

#define PIPELINE_START		WM_APP + 0x10
#define PIPELINE_STOP		WM_APP + 0x11
#define STREAMER_NEXT		WM_APP + 0x12
#define CAMSTREAM_START		WM_APP + 0x13

typedef void(__stdcall *delegate)(int, int);

#define LIB_API __declspec(dllexport)

extern "C"
{
	LIB_API void Init(HWND hWnd);
	LIB_API int CreatePipeline(LPCWSTR szAddress, IDirect3DSurface9 **pSurface, int &width, int &heigth, delegate callback);
	LIB_API int CreateCamStream(int index, int prefferableMode,  IDirect3DSurface9 **pSurface, int &width, int &height, bool& isHwEnabled, delegate callback);
	LIB_API void NextFrame(int index);
	LIB_API void SetFrameRate(int fps);
	LIB_API void Shutdown(int index);
	LIB_API int FrameFormatSerialize(int width, int height, int fps);

}

extern int DefaultFps;
extern HMODULE g_hModule;