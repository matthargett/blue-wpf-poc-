using System;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Incapsulates all work with native MediaFoundation library
    /// </summary>
    public class Streamer
    {
        private readonly IMediaDeviceWrapper _mediaDeviceWrapper;
        public Streamer(IMediaDeviceWrapper mediaDeviceWrapper, IntPtr mainAppHandle)
        {
            _mediaDeviceWrapper = mediaDeviceWrapper;
            _mediaDeviceWrapper.Init(mainAppHandle);
        }

        public int StartStream(string source, out int width, out int height, out IntPtr surface, RenderCallback callback)
        {
            width = 0; height = 0; surface = IntPtr.Zero;
            return _mediaDeviceWrapper.CreatePipeline(source, out surface, ref width, ref height, callback);
        }

        public int StartStream(int index, out int width, out int height, out IntPtr surface, out bool isHwEnabled, RenderCallback callback)
        {
            width = 0; height = 0; surface = IntPtr.Zero; isHwEnabled = true;
            return _mediaDeviceWrapper.CreateCamStream(index, out surface, ref width, ref height, ref isHwEnabled, callback);
        }

        public void StopStream(int index)
        {
            _mediaDeviceWrapper.Shutdown(index);
        }

        public void SetHwRate(int fps)
        {
            _mediaDeviceWrapper.SetFrameRate(fps);
        }

        public void Next(int index)
        {
            if (index > 0)
            {
                _mediaDeviceWrapper.NextFrame(index);
            }
        }
    }
}