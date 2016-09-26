using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Encapsulates all work with native MediaFoundation library
    /// Should remain a simple wrapper without any logic
    /// </summary>
    internal class MediaDeviceWrapper : IMediaDeviceWrapper
    {
        private const string DLL_PATH = "Libs\\MediaDevice.dll";

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Init")]
        static extern void ExtInit(IntPtr mainAppHandle);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CreatePipeline")]
        static extern int ExtCreatePipeline([MarshalAs(UnmanagedType.LPWStr)]string address, out IntPtr pSurface, ref int width, ref int height, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CreateCamStream")]
        static extern int ExtCreateCamStream(int id, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "NextFrame")]
        static extern void ExtNextFrame(int index);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SetFrameRate")]
        static extern void ExtSetFrameRate(int fps);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Shutdown")]
        static extern void ExtShutdown(int index);

        public void Init(IntPtr mainAppHandle)
        {
            ExtInit(mainAppHandle);
        }

        public int CreatePipeline(string address, out IntPtr pSurface, ref int width, ref int height, RenderCallback cb)
        {
            return ExtCreatePipeline(address, out pSurface, ref width, ref height, cb);
        }

        public int CreateCamStream(int id, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, RenderCallback cb)
        {
            return ExtCreateCamStream(id, out pSurface, ref width, ref height, ref isHwEnabled, cb);
        }

        public void NextFrame(int index)
        {
            ExtNextFrame(index);
        }

        public void SetFrameRate(int fps)
        {
            ExtSetFrameRate(fps);
        }

        public void Shutdown(int index)
        {
            ExtShutdown(index);
        }
    }
}
