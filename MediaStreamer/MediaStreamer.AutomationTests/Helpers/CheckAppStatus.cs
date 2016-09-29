using System.Diagnostics;
using System.Linq;
using System.Threading;

namespace MediaStreamer.AutomationTests.Helpers
{
    public class CheckAppStatus
    {
        string instanceName = "MediaStreamer.UI";
        public Thread checkApp(Tests host)
        {
            return new Thread(() =>
            {
                while (isResponding())
                {
                    Thread.Sleep(1000);

                }
                host.isFailed = true;
                failTest("Application is not responding");

            });
        }

        public bool isResponding()
        {
            return Process.GetProcessesByName(instanceName).All(p => p.Responding);
        }
        public void failTest(string message)
        {
            AutHelper.logLog(message);
            foreach (var process in Process.GetProcessesByName(instanceName))
            {
                process.Kill();
            }

        }
    }
}
