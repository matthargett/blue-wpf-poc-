#include "stdafx.h"
#include "StaticStreamAsyncHelper.h"
#include "CamStreamStaticImpl.h"

namespace media
{
	namespace reader
	{
		namespace impl_
		{
			unsigned __stdcall StaticStreamAsyncHelper::ThreadProc(void* pArguments)
			{
				StaticStreamAsyncHelper *pImpl = static_cast<StaticStreamAsyncHelper*>(pArguments);

				if (pImpl == NULL)
					return -1;
				static const UINT nHandles = 2;
				HANDLE hEvents[nHandles];
				hEvents[0] = pImpl->hNextFrameEvent;
				hEvents[1] = pImpl->hExitEvent;

				while (true)
				{
					DWORD retCode = ::WaitForMultipleObjects(nHandles, hEvents, FALSE, INFINITE);

					if (retCode == WAIT_OBJECT_0 + 1)
						return 0;

					HRESULT hr = S_OK;
					IMFMediaBuffer *pBuffer = NULL;
					IMFSample *pSample = NULL;

					hr = MFCreateMemoryBuffer(1, &pBuffer);
					hr = MFCreateSample(&pSample);

					hr = pSample->AddBuffer(pBuffer);
					SafeRelease(&pBuffer);

					pImpl->pStaticStream->pReaderCallback->OnReadSample(S_OK, 0, 0, 0, pSample);
					SafeRelease(&pSample);

					::Sleep(33);
				}

			}

			StaticStreamAsyncHelper::StaticStreamAsyncHelper(CamStreamStaticImpl* pStream)
			{
				pStaticStream = pStream;
				hExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				hNextFrameEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, static_cast<void*>(this), NULL, NULL);

			}
			
			StaticStreamAsyncHelper::~StaticStreamAsyncHelper()
			{
				::SetEvent(hExitEvent);
				::WaitForSingleObject(hWorkerThread, msToWaitCompletition);
				
				::CloseHandle(hExitEvent);
				::CloseHandle(hNextFrameEvent);
			}

			void StaticStreamAsyncHelper::Next()
			{
				::SetEvent(hNextFrameEvent);
			}

		}
	}
}