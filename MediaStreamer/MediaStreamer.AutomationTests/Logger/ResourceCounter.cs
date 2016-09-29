using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MediaStreamer.AutomationTests.Logger
{
    public class ResourceCounter
    {

        static PerformanceCounter cpuCounter;
        static string instanceName = "MediaStreamer.UI";


        public static float logProcessInfo(string counterName)
        {
            cpuCounter = new PerformanceCounter();
            cpuCounter.CategoryName = "Process";
            cpuCounter.CounterName = counterName;

            try
            {
                cpuCounter.InstanceName = instanceName;
                return cpuCounter.NextValue();

            }
            catch (Exception)
            {
                cpuCounter.InstanceName = "_Total";
                return cpuCounter.NextValue();
            }

        }

        [DllImport("User32")]
        extern public static int GetGuiResources(IntPtr hProcess, int uiFlags);


        public static int GetGuiResourcesGDICount()
        {
            try
            {
                return GetGuiResources(Process.GetProcessesByName("MediaStreamer.UI")[0].Handle, 0);
            }
            catch (Exception)
            {
                return 0;
            }
        }
        
        public static int GetGuiResourcesUserCount()
        {
            return GetGuiResources(Process.GetCurrentProcess().Handle, 1);
        }


        public static string getCpuAndRamUsage()
        {
            return ". RAM: " + logProcessInfo("Working Set") / 1000000 + " MB"
                 + ", Handels: " + logProcessInfo("Handle Count")
                 + ", GDI: " + GetGuiResourcesGDICount()
                 + ", Threads: " + logProcessInfo("Thread Count");
        }

        public string getFinalUsage()
        {
            return ". RAM: " + logProcessInfo("Working Set Peak") / 1000000 + " MB"
                 + ", Handels: " + logProcessInfo("Handle Count")
                 + ", GDI: " + GetGuiResourcesGDICount()
                 + ", Threads: " + logProcessInfo("Thread Count");
        }
    }
}
