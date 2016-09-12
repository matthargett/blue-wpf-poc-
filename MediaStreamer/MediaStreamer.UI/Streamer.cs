using System;
using System.Runtime.InteropServices;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Incapsulates all work with native MediaFoundation library
    /// </summary>
    class Streamer
    {
        private const string DLL_PATH = "Libs\\MediaDevice.dll";

        public delegate void RenderCallback(int x, int y);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        static extern void Init(IntPtr mainAppHandle);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        static extern int CreatePipeline([MarshalAs(UnmanagedType.LPWStr)]string address, out IntPtr pSurface, ref int width, ref int height, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        static extern int CreateCamStream(int id, out IntPtr pSurface, ref int width, ref int height, ref bool isHwEnabled, [MarshalAs(UnmanagedType.FunctionPtr)]RenderCallback cb);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        static extern void NextFrame(int index);

        [DllImport(DLL_PATH, CallingConvention = CallingConvention.Cdecl)]
        static extern void Shutdown(int index);


        public Streamer(IntPtr mainAppHandle)
        { 
            Init(mainAppHandle);
        }

        public int StartStream(string source, out int width, out int height, out IntPtr surface, RenderCallback callback)
        {
            width = 0; height = 0; surface = IntPtr.Zero;
            return CreatePipeline(source, out surface, ref width, ref height, callback);
        }

        public int StartStream(int index, out int width, out int height, out IntPtr surface, out bool isHwEnabled, RenderCallback callback)
        {
            width = 0; height = 0; surface = IntPtr.Zero; isHwEnabled = true;
            return CreateCamStream(index, out surface, ref width, ref height, ref isHwEnabled, callback);
        }

        public void StopStream(int index)
        {
            Shutdown(index);
        }

        public void Next(int index)
        {
            if (index > 0)
            {
                NextFrame(index);
            }
        }
    }
}