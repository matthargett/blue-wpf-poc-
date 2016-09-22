#include "stdafx.h"
#include "VideoService.h"
#include "D3DManager.h"

namespace media
{
	namespace presenter
	{
		namespace _impl
		{
			const UINT EX_COLOR_INFO[][2] =
			{
				// SDTV ITU-R BT.601 YCbCr to driver's optimal RGB range
				{ DXVA2_VideoTransferMatrix_BT601, DXVA2_NominalRange_Unknown },
				// SDTV ITU-R BT.601 YCbCr to studio RGB [16...235]
				{ DXVA2_VideoTransferMatrix_BT601, DXVA2_NominalRange_16_235 },
				// SDTV ITU-R BT.601 YCbCr to computer RGB [0...255]
				{ DXVA2_VideoTransferMatrix_BT601, DXVA2_NominalRange_0_255 },
				// HDTV ITU-R BT.709 YCbCr to driver's optimal RGB range
				{ DXVA2_VideoTransferMatrix_BT709, DXVA2_NominalRange_Unknown },
				// HDTV ITU-R BT.709 YCbCr to studio RGB [16...235]
				{ DXVA2_VideoTransferMatrix_BT709, DXVA2_NominalRange_16_235 },
				// HDTV ITU-R BT.709 YCbCr to computer RGB [0...255]
				{ DXVA2_VideoTransferMatrix_BT709, DXVA2_NominalRange_0_255 }
			};

			DWORD RGBtoYUV(const D3DCOLOR rgb)
			{
				const INT A = HIBYTE(HIWORD(rgb));
				const INT R = LOBYTE(HIWORD(rgb)) - 16;
				const INT G = HIBYTE(LOWORD(rgb)) - 16;
				const INT B = LOBYTE(LOWORD(rgb)) - 16;

				//
				// studio RGB [16...235] to SDTV ITU-R BT.601 YCbCr
				//
				INT Y = (77 * R + 150 * G + 29 * B + 128) / 256 + 16;
				INT U = (-44 * R - 87 * G + 131 * B + 128) / 256 + 128;
				INT V = (131 * R - 110 * G - 21 * B + 128) / 256 + 128;

				return D3DCOLOR_AYUV(A, Y, U, V);
			}

		}

		VideoService::VideoService()
			: pVideoService(NULL),
			pProcessor(NULL)
		{
		}

		HRESULT VideoService::Create(UINT width, UINT height, D3DManager *pManager)
		{
			IDirect3DDevice9 *pDevice = pManager->GetDevice();
			if (pDevice == NULL)
				return D3DERR_INVALIDCALL;

			HRESULT hr = DXVA2CreateVideoService(
				pDevice,
				IID_PPV_ARGS(&pVideoService));

			assert(hr == S_OK);
			pManager->FreeDevice(&pDevice);

			if (FAILED(hr))
				return hr;

			memset(&g_VideoDesc, 0, sizeof(g_VideoDesc));

			g_VideoDesc.SampleWidth = width;
			g_VideoDesc.SampleHeight = height;
			g_VideoDesc.SampleFormat.VideoChromaSubsampling = DXVA2_VideoChromaSubsampling_MPEG2;
			g_VideoDesc.SampleFormat.NominalRange = DXVA2_NominalRange_16_235;
			g_VideoDesc.SampleFormat.VideoTransferMatrix = _impl::EX_COLOR_INFO[0][0];
			g_VideoDesc.SampleFormat.VideoLighting = DXVA2_VideoLighting_dim;
			g_VideoDesc.SampleFormat.VideoPrimaries = DXVA2_VideoPrimaries_BT709;
			g_VideoDesc.SampleFormat.VideoTransferFunction = DXVA2_VideoTransFunc_709;
			g_VideoDesc.SampleFormat.SampleFormat = DXVA2_SampleProgressiveFrame;
			g_VideoDesc.Format = D3DFMT_YUY2;
			g_VideoDesc.InputSampleFreq.Numerator = 30;
			g_VideoDesc.InputSampleFreq.Denominator = 1;
			g_VideoDesc.OutputFrameFreq.Numerator = 30;
			g_VideoDesc.OutputFrameFreq.Denominator = 1;

			UINT count;
			GUID* guids = NULL;

			hr = pVideoService->GetVideoProcessorDeviceGuids(&g_VideoDesc, &count, &guids);
			assert(hr == S_OK);

			for (UINT i = 0; i < count; i++)
			{
				if (CreateDevice(guids[i]))
				{
					break;
				}
			}

			::CoTaskMemFree(guids);

			if (pProcessor == NULL)
			{
				return D3DERR_NOTFOUND;
			}

			memset(&blt, 0, sizeof(blt));
			RECT target = { 0,0,(LONG)width, (LONG)height };
			blt.TargetFrame = 0;
			blt.TargetRect = target;

			// DXVA2_VideoProcess_Constriction
			blt.ConstrictionSize.cx = target.right - target.left;
			blt.ConstrictionSize.cy = target.bottom - target.top;

			blt.BackgroundColor = GetBackgroundColor();

			// DXVA2_VideoProcess_YUV2RGBExtended
			blt.DestFormat.VideoChromaSubsampling = DXVA2_VideoChromaSubsampling_Unknown;
			blt.DestFormat.NominalRange = _impl::EX_COLOR_INFO[0][1];
			blt.DestFormat.VideoTransferMatrix = DXVA2_VideoTransferMatrix_Unknown;
			blt.DestFormat.VideoLighting = DXVA2_VideoLighting_dim;
			blt.DestFormat.VideoPrimaries = DXVA2_VideoPrimaries_BT709;
			blt.DestFormat.VideoTransferFunction = DXVA2_VideoTransFunc_709;

			blt.DestFormat.SampleFormat = DXVA2_SampleProgressiveFrame;

			DXVA2_ValueRange range = { 0 };
			// DXVA2_ProcAmp_Brightness
			hr = pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Brightness, &range);
			assert(hr == S_OK);
			blt.ProcAmpValues.Brightness = range.DefaultValue;

			// DXVA2_ProcAmp_Contrast
			hr = pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Contrast, &range);
			assert(hr == S_OK);
			blt.ProcAmpValues.Contrast = range.DefaultValue;

			// DXVA2_ProcAmp_Hue
			hr = pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Hue, &range);
			assert(hr == S_OK);
			blt.ProcAmpValues.Hue = range.DefaultValue;

			// DXVA2_ProcAmp_Saturation
			hr = pProcessor->GetProcAmpRange(DXVA2_ProcAmp_Saturation, &range);
			assert(hr == S_OK);
			blt.ProcAmpValues.Saturation = range.DefaultValue;

			// DXVA2_VideoProcess_AlphaBlend
			blt.Alpha = DXVA2_Fixed32OpaqueAlpha();

			// DXVA2_VideoProcess_NoiseFilter //DOESNT work for RADEON HD4500
			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterLumaLevel, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterLuma.Level = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterLumaThreshold, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterLuma.Threshold = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterLumaRadius, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterLuma.Radius = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterChromaLevel, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterChroma.Level = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterChromaThreshold, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterChroma.Threshold = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_NoiseFilterChromaRadius, &range);
			//assert(hr == S_OK);
			//blt.NoiseFilterChroma.Radius = range.DefaultValue;

			
			// DXVA2_VideoProcess_DetailFilter
			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterLumaLevel, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterLuma.Level = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterLumaThreshold, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterLuma.Threshold = range.DefaultValue;
			//
			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterLumaRadius, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterLuma.Radius = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterChromaLevel, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterChroma.Level = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterChromaThreshold, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterChroma.Threshold = range.DefaultValue;

			//hr = pProcessor->GetFilterPropertyRange(DXVA2_DetailFilterChromaRadius, &range);
			//assert(hr == S_OK);
			//blt.DetailFilterChroma.Radius = range.DefaultValue;

			memset(&sample, 0, sizeof(sample));
			sample.SampleFormat.VideoChromaSubsampling = DXVA2_VideoChromaSubsampling_MPEG2;
			sample.SampleFormat.NominalRange = DXVA2_NominalRange_16_235;
			sample.SampleFormat.VideoTransferMatrix = _impl::EX_COLOR_INFO[0][0];
			sample.SampleFormat.VideoLighting = DXVA2_VideoLighting_dim;
			sample.SampleFormat.VideoPrimaries = DXVA2_VideoPrimaries_BT709;
			sample.SampleFormat.VideoTransferFunction = DXVA2_VideoTransFunc_709;

			sample.SampleFormat.SampleFormat = DXVA2_SampleProgressiveFrame;

			sample.DstRect = target;
			sample.SrcRect = target;
			sample.PlanarAlpha = DXVA2FloatToFixed(float(1));

			return S_OK;
		}


		VideoService::~VideoService()
		{
			SafeRelease(&pProcessor);
			SafeRelease(&pVideoService);
		}

		void VideoService::Draw(IDirect3DSurface9* pDrawSurface, IDirect3DSurface9 *pSample)
		{
			assert(pProcessor != NULL);
			assert(pDrawSurface != NULL);
			assert(pSample != NULL);

			sample.SrcSurface = pSample;
			HRESULT hr = pProcessor->VideoProcessBlt(
				pDrawSurface,
				&blt,
				&sample,
				1,
				NULL);
			assert(hr == S_OK);
		}

		bool VideoService::CreateDevice(REFGUID guid)
		{
			DXVA2_VideoProcessorCaps g_VPCaps = { 0 };
			HRESULT hr = pVideoService->GetVideoProcessorCaps(
				guid,
				&g_VideoDesc,
				D3DFMT_X8R8G8B8,
				&g_VPCaps);

			if (FAILED(hr))
				return false;

			hr = pVideoService->CreateVideoProcessor(
				guid,
				&g_VideoDesc,
				D3DFMT_X8R8G8B8,
				0,
				&pProcessor);
			
			return hr == S_OK;
		}

		DXVA2_AYUVSample16 VideoService::GetBackgroundColor() const
		{
			const D3DCOLOR yuv = _impl::RGBtoYUV(D3DCOLOR_XRGB(0xEB, 0xEB, 0x10));

			const BYTE Y = LOBYTE(HIWORD(yuv));
			const BYTE U = HIBYTE(LOWORD(yuv));
			const BYTE V = LOBYTE(LOWORD(yuv));

			DXVA2_AYUVSample16 color;

			color.Cr = V * 0x100;
			color.Cb = U * 0x100;
			color.Y = Y * 0x100;
			color.Alpha = 0xFFFF;

			return color;
		}

	}
}
