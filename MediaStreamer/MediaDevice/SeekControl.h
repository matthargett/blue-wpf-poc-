#pragma once
namespace media
{
	namespace reader
	{
		class SeekControl
		{
		public:
			SeekControl(IMFSourceReader * pReader);
			~SeekControl();

			bool CanSeek();
			HRESULT SetPosition(LONGLONG position = 0);	//position in 100-nanosec units

		private:
			IMFSourceReader *pSourceReader;

			HRESULT GetSourceFlags(IMFSourceReader *pReader, ULONG *pulFlags);
		};
	}
}

