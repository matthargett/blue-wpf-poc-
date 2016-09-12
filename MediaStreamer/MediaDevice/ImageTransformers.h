#pragma once
namespace media
{
	namespace presenter
	{
		namespace transformers
		{
			typedef void(*IMAGE_TRANSFORM_FN)(
				BYTE*       pDest,
				LONG        lDestStride,
				const BYTE* pSrc,
				LONG        lSrcStride,
				DWORD       dwWidthInPixels,
				DWORD       dwHeightInPixels
				);

			void TransformImage_RGB24(
				BYTE*       pDest,
				LONG        lDestStride,
				const BYTE* pSrc,
				LONG        lSrcStride,
				DWORD       dwWidthInPixels,
				DWORD       dwHeightInPixels
			);

			void TransformImage_RGB32(
				BYTE*       pDest,
				LONG        lDestStride,
				const BYTE* pSrc,
				LONG        lSrcStride,
				DWORD       dwWidthInPixels,
				DWORD       dwHeightInPixels
			);

			void TransformImage_YUY2(
				BYTE*       pDest,
				LONG        lDestStride,
				const BYTE* pSrc,
				LONG        lSrcStride,
				DWORD       dwWidthInPixels,
				DWORD       dwHeightInPixels
			);

			void TransformImage_NV12(
				BYTE* pDst,
				LONG dstStride,
				const BYTE* pSrc,
				LONG srcStride,
				DWORD dwWidthInPixels,
				DWORD dwHeightInPixels
			);
		}

		bool IsFormatSupported(REFGUID subtype);
		HRESULT GetFormat(DWORD index, GUID *pSubtype);
		transformers::IMAGE_TRANSFORM_FN GetTransformFuntion(REFGUID subtype);

	}
}
