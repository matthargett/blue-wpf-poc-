#pragma once

namespace media
{
	struct PipelineInit
	{
		LPCWSTR szAddress;	//[in] address of mediasource (NULL for WebCam)
		int index;			//[in, out] input: index of WebCam, not use for mediafile,
							//output: index of pipeline for further control
		LPDIRECT3DSURFACE9 pISurface; //[out] DirectX Surface for drawing
		int width;			//[out] width of frame
		int height;			//[out] height of frame
		bool isHwAcceleration; //[out] got WebCam indicates if HW acceleration is enabled

		delegate renderCallback;//[in] callback that is called once frame is ready for dispaly
	};

	/**
		Creates pipeline

		@param: hWndVideo - handle to window that displays video,
		@return: index of the pipeline, it is duplicate pli.index value
	*/
	int CreatePipeline(HWND hWndVideo, PipelineInit &pli);
	int CreateCamStream(HWND hWndVideo, PipelineInit &pli);
	void DestroyPipeline(int indexPipeline);
	void Next(int indexPipeline);
	void DestroyAll();
}
