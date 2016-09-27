#pragma once

namespace media
{
	namespace reader
	{
		namespace impl_
		{
			class CamStreamStaticImpl;

			class StaticStreamAsyncHelper
			{
			public:
				StaticStreamAsyncHelper(CamStreamStaticImpl* pStaticStream);
				~StaticStreamAsyncHelper();

				void Next();

			private:
				static unsigned __stdcall StaticStreamAsyncHelper::ThreadProc(void* pArguments);
				HANDLE hWorkerThread;
				HANDLE hExitEvent;
				HANDLE hNextFrameEvent;
				CamStreamStaticImpl* pStaticStream;
				static const DWORD msToWaitCompletition = INFINITE;
			};
		}
	}
}