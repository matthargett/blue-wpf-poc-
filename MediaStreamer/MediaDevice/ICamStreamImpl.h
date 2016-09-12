#pragma once
namespace media
{
	namespace reader
	{
		namespace impl_
		{

			//Interface that generalizes creating and handling WebCamera
			//implementation will realize either software or hardware handling
			class ICamStreamImpl
			{
			public:
				ICamStreamImpl()
					: height(0), width(0)
				{
				}

				virtual ~ICamStreamImpl()
				{
				}

				virtual HRESULT CreateSourceReader(IMFMediaSource *pSource, IMFSourceReaderCallback *pReaderCallback) = 0;
				virtual IDirect3DSurface9 *GetDrawSurface() = 0;
				virtual HRESULT DrawSample(IMFMediaBuffer *pBuffer) = 0;
				virtual HRESULT Next() = 0;
				virtual HRESULT Close() = 0;
				virtual bool IsAccelerated() const = 0;

				inline UINT32 GetFrameWidth() const
				{
					return width;
				}

				inline UINT32 GetFrameHeight() const
				{
					return height;
				}

			protected:
				UINT32 height;
				UINT32 width;
			};
		}
	}
}

