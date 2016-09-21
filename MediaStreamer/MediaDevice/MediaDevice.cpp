#include "stdafx.h"
#include <process.h>
#include "PipelineManager.h"

int DefaultFps = 34; //FPS for rendering video streams

LPCWSTR szStreamerWindowName = L"StreamerWnd";
HANDLE hThread = NULL;
HWND hWndUI = NULL;
HWND hWndStreamer = NULL;
unsigned threadID = 0;

unsigned __stdcall ThreadProc(void* pArguments);
BOOL InitializeApplication();
void UnininitializeApplication();
BOOL RegisterStreamerWindowClass();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------------------
// Public API
void Init(HWND hWnd)
{
	hWndUI = hWnd;
	hWndStreamer = NULL;
	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, NULL, NULL);
}

int CreateCamStream(int index, IDirect3DSurface9 **pSurface, int &width, int &height, bool& isHwEnabled, delegate callback)
{
	if (!callback || hWndStreamer == NULL)
		return 0;

	media::PipelineInit pli = { 0 };
	pli.index = index;
	pli.renderCallback = callback;

	::SendMessage(hWndStreamer, CAMSTREAM_START, (WPARAM)&pli, NULL);

	width = pli.width;
	height = pli.height;
	isHwEnabled = pli.isHwAcceleration;
	*pSurface = pli.pISurface;
	return pli.index;
}

int CreatePipeline(LPCWSTR szAddress, IDirect3DSurface9 **pSurface, int &width, int &height, delegate callback)
{
	if (!callback || hWndStreamer == NULL)
		return 0;

	media::PipelineInit pli = { 0 };
	pli.szAddress = szAddress;
	pli.renderCallback = callback;
	
	::SendMessage(hWndStreamer, PIPELINE_START, (WPARAM)&pli, NULL);


	width = pli.width;
	height = pli.height;
	*pSurface = pli.pISurface;
	return pli.index;
}

void NextFrame(int index)
{
	::SendMessage(hWndStreamer, STREAMER_NEXT, index, NULL);
}

void SetFrameRate(int fps)
{
	if (fps > 1 && fps < 100)
		DefaultFps = fps;
}

void Shutdown(int index)
{
	::SendMessage(hWndStreamer, PIPELINE_STOP, index, NULL);
	return;
}

//---------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case PIPELINE_START:
	{
		//TODO: the window should be used for handling hEvent messages
		//received from stream devices
		media::PipelineInit *pPLI = reinterpret_cast<media::PipelineInit*>(wParam);
		media::CreatePipeline(hWndStreamer, *pPLI);
		return S_OK;
	}
	case CAMSTREAM_START:
	{
		//TODO: the window should be used for handling hEvent messages
		//received from stream devices
		media::PipelineInit *pPLI = reinterpret_cast<media::PipelineInit*>(wParam);
		media::CreateCamStream(hWndStreamer, *pPLI);
		return S_OK;
	}
	case PIPELINE_STOP:
	{
		media::DestroyPipeline((int)wParam);
		return S_OK;
	}
	case STREAMER_NEXT:
	{
		media::Next((int)wParam);
		return S_OK;
	}
	case WM_DESTROY:
		media::DestroyAll();
		PostQuitMessage(0);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//---------------------------------------------------------------------------

// The new thread
unsigned __stdcall ThreadProc(void* /*pArguments*/)
{
	MSG messages;

	if (!InitializeApplication())
		return -1;

	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	UnininitializeApplication();
	return 0;
}

BOOL InitializeApplication()
{
	HRESULT hr = S_OK;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		hr = MFStartup(MF_VERSION);
	}

	if (FAILED(hr))
		return FALSE;

	//create window for message pumping
	if (RegisterStreamerWindowClass())
	{
		hWndStreamer = ::CreateWindow(szStreamerWindowName, szStreamerWindowName,
			WS_CHILD, /*STYLE*/
			0, 0, 1, 1,
			hWndUI,
			NULL, g_hModule, NULL);

		return hWndStreamer != NULL;
	}

	return FALSE;
}

void UnininitializeApplication()
{
	MFShutdown();
	CoUninitialize();
}

BOOL RegisterStreamerWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.lpszClassName = szStreamerWindowName;
	wc.hInstance = g_hModule;
	wc.lpfnWndProc = WindowProc;

	if (!RegisterClass(&wc))
		return FALSE;

	return TRUE;
}
