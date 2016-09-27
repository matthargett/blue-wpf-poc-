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
        public void Init(IntPtr mainAppHandle)
        {
            NativeMethods.Init(mainAppHandle);
        }

        public int CreatePipeline(string address, out IntPtr pSurface, ref int width, ref int height, RenderCallback cb)
        {
            return NativeMethods.CreatePipeline(address, out pSurface, ref width, ref height, cb);
        }

        public int CreateCamStream(int id, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, RenderCallback cb)
        {
            return NativeMethods.CreateCamStream(id, out pSurface, ref width, ref height, ref isHwEnabled, cb);
        }

        public void NextFrame(int index)
        {
            NativeMethods.NextFrame(index);
        }

        public void SetFrameRate(int fps)
        {
            NativeMethods.SetFrameRate(fps);
        }

        public void Shutdown(int index)
        {
            NativeMethods.Shutdown(index);
        }
    }

    internal static class NativeMethods
    {
        private const string DLL_PATH = "Libs\\MediaDevice.dll";

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Init(IntPtr mainAppHandle);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int CreatePipeline([MarshalAs(UnmanagedType.LPWStr)]string address, out IntPtr pSurface, ref int width, ref int height, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int CreateCamStream(int id, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NextFrame(int index);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void SetFrameRate(int fps);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Shutdown(int index);
    }
}
