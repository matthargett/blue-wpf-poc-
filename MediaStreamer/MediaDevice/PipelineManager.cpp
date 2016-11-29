#include "stdafx.h"
#include <map>
#include "PipelineManager.h"
#include "Pipeline.h"


#include "RenderDevice.h"

namespace media
{
	namespace _impl
	{
		static int streamsCounter = 0;
		static  std::map<int, Pipeline*> streamsMap;

		void addStream(int id, Pipeline *pipe)
		{
			streamsMap[id] = pipe;
		}

		Pipeline *getStream(int id)
		{
			auto it = streamsMap.find(id);

			if (it == streamsMap.end())
				return nullptr;
			return
				it->second;
		}

		void removeStream(int id)
		{
			auto it = streamsMap.find(id);

			if (it == streamsMap.end())
				return;

			delete it->second;
			streamsMap.erase(it);
		}

		void removeAllStreams()
		{
			for (auto it = streamsMap.begin(); it != streamsMap.end(); ++it)
			{
				delete it->second;
			}

			streamsMap.clear();
			streamsCounter = 0;
		}
	}
//	#define DEBUG_TEST

#ifdef DEBUG_TEST
	presenter::RenderDevice *pRD = nullptr;
	Pipeline *p = nullptr;

	int CreateCamStream(HWND hWndVideo, PipelineInit &pli)
	{
		HRESULT hr = Pipeline::CreateCamInstance(10, hWndVideo, 0, &p);
		pli.pISurface = p->GetSurface();
		return 42;
	}

	int CreatePipeline(HWND hWndVideo, PipelineInit &pli)
	{
		pRD = new presenter::RenderDevice();
		HRESULT hr;
		hr = pRD->CreateDevice(hWndVideo);
		assert(hr == S_OK);
		hr = pRD->CreateVAService(1024, 768, GUID_NULL);
		assert(hr == S_OK);
		return 1;
	}

	void DestroyPipeline(int indexPipeline)
	{
		delete p;
		p = nullptr;
		delete pRD;
		pRD = nullptr;
	}

	void Next(int indexPipeline)
	{
	}
#else
	int CreateCamStream(HWND hWndVideo, PipelineInit &pli)
	{
		Pipeline *p = nullptr;
		int id = ++_impl::streamsCounter;
		HRESULT hr = Pipeline::CreateCamInstance(id, hWndVideo, pli.index, pli.prefferableMode, pli.renderCallback, &p);

		if (FAILED(hr))
		{
			pli.index = 0;
			return 0;
		}

		pli.pISurface = p->GetSurface();
		p->GetFrameSize(pli.width, pli.height);
		pli.isHwAcceleration = p->IsHwAccelerated();

		pli.index = id;
		_impl::addStream(id, p);

		return id;
	}

	int CreatePipeline(HWND hWndVideo, PipelineInit &pli)
	{
		Pipeline *p = nullptr;
		int id = ++_impl::streamsCounter;
		HRESULT hr = Pipeline::CreateFileInstance(id, hWndVideo, pli.szAddress, pli.renderCallback, &p);

		if (FAILED(hr))
		{
			pli.index = 0;
			return 0;
		}

		pli.pISurface = p->GetSurface();
		p->GetFrameSize(pli.width, pli.height);

		pli.index = id;
		_impl::addStream(id, p);

		return id;
	}

	void DestroyPipeline(int indexPipeline)
	{
		_impl::removeStream(indexPipeline);
	}

	void Next(int indexPipeline)
	{
		auto pStream = _impl::getStream(indexPipeline);
		if (pStream)
			pStream->Next();
	}
#endif

	void DestroyAll()
	{
		_impl::removeAllStreams();
	}
}