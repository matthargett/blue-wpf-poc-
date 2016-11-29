using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MediaStreamer.UI
{
    public interface IMediaDeviceWrapper
    {
        void Init(IntPtr mainAppHandle);
        int CreatePipeline(string address, out IntPtr pSurface, ref int width, ref int height, RenderCallback cb);
        int CreateCamStream(int id, int prefferableMode, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, RenderCallback cb);
        void NextFrame(int index);
        void SetFrameRate(int fps);
        void Shutdown(int index);
        int FrameFormatSerialize(int width, int height, int fps);
    }

    public delegate void RenderCallback(int x, int y);
}
