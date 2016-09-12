#include "stdafx.h"
#include "FrameRateControl.h"

namespace media
{
	namespace reader
	{

		FrameRateControl::FrameRateControl(IMFSourceReader * pSource)
			: pSourceReader(pSource)
		{
			pSourceReader->AddRef();
		}

		FrameRateControl::~FrameRateControl()
		{
			SafeRelease(&pSourceReader);
		}

		float FrameRateControl::GetRate() const
		{
			IMFRateControl *pControl = NULL;

			HRESULT hr = pSourceReader->GetServiceForStream(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				MF_RATE_CONTROL_SERVICE,	//list of GUIDs https://msdn.microsoft.com/en-us/library/windows/desktop/ms695350(v=vs.85).aspx
				IID_IMFRateControl,
				(LPVOID*)&pControl);
				
			float rate = 0;
			if (SUCCEEDED(hr))
			{
				BOOL isThinSupported = FALSE;
				hr = pControl->GetRate(&isThinSupported, &rate);
				assert(hr == S_OK);
			}

			SafeRelease(&pControl);
			return rate;
		}

		void FrameRateControl::SetRate(float rate)
		{
			IMFRateControl *pControl = NULL;

			HRESULT hr = pSourceReader->GetServiceForStream(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				MF_RATE_CONTROL_SERVICE,	//list of GUIDs https://msdn.microsoft.com/en-us/library/windows/desktop/ms695350(v=vs.85).aspx
				IID_IMFRateControl,
				(LPVOID*)&pControl);

			if (SUCCEEDED(hr))
			{
				hr = pControl->SetRate(FALSE, rate);
				assert(hr == S_OK);
			}

			SafeRelease(&pControl);
		}

		void FrameRateControl::GetRateRange(float &min, float &max, MFRATE_DIRECTION direction) const
		{
			IMFRateSupport *pSupport = NULL;

			HRESULT hr = pSourceReader->GetServiceForStream(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				MF_RATE_CONTROL_SERVICE,
				IID_IMFRateSupport,
				(LPVOID*) &pSupport);

			if (SUCCEEDED(hr))
			{
				hr = pSupport->GetSlowestRate(direction, FALSE, &min);
				assert(hr == S_OK);
				hr = pSupport->GetFastestRate(direction, FALSE, &max);
				//assert(hr == S_OK);
			}

			SafeRelease(&pSupport);
		}

		float FrameRateControl::IsRateSupported(float rate) const
		{
			IMFRateSupport *pSupport = NULL;

			HRESULT hr = pSourceReader->GetServiceForStream(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				MF_RATE_CONTROL_SERVICE,
				IID_IMFRateSupport,
				(LPVOID*)&pSupport);

			float nearestRate = 0;
			if (SUCCEEDED(hr))
			{
				hr = pSupport->IsRateSupported(FALSE, rate, &nearestRate);
			}
			
			SafeRelease(&pSupport);
			return nearestRate;
		}
	}
}
