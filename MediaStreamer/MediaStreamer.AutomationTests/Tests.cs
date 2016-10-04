using MediaStreamer.AutomationTests.Helpers;
using MediaStreamer.AutomationTests.Logger;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using System.Linq;
using System.Threading;

namespace MediaStreamer.AutomationTests
{
    [TestClass]
    public class Tests
    {
        private string applicationName = "MediaStreamer.UI";
        private bool isFailed = false;
        private bool isFinalizing = false;

        [TestMethod]
        [Timeout(9000000)]
        public void MainTest()
        {
            PLogger.InitPLogger("MainTest");
            if (Settings.Default.LaunchApp) { AutHelper.LaunchApp(); }

            if (Settings.Default.ClickCallButton) { AutHelper.StarStopVideoButton.ClickButton(); }

            TimeSpan timeout = new TimeSpan(0, 0, 0, 0, 500);

            int iterationQuantity = Settings.Default.IterationQuantity;
            int iter = 0;
            Thread checkerThread = new Thread(() => checkAppRunner(this));
            try
            {
                checkerThread.Start();

                do
                {
                    AutHelper.MoveWindowUp();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowDown();
                    Thread.Sleep(timeout);

                    AutHelper.ResizeWindowSmall();
                    Thread.Sleep(timeout);

                    AutHelper.ResizeWindowBig();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowDown();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowUp();
                    Thread.Sleep(timeout);

                    AutHelper.ResizeWindowSmall();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowUp();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowDown();
                    Thread.Sleep(timeout);

                    AutHelper.MinimizeWindow();
                    Thread.Sleep(timeout);

                    AutHelper.RestoreFromTaskBar();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowUp();
                    Thread.Sleep(timeout);

                    AutHelper.ResizeWindowBig();
                    Thread.Sleep(timeout);
                    AutHelper.ResizeWindowSmall();
                    Thread.Sleep(timeout);

                    AutHelper.MoveWindowDown();
                    Thread.Sleep(timeout);

                    AutHelper.ResizeWindowBig();
                    Thread.Sleep(timeout);

                    AutHelper.MinimizeWindow();
                    Thread.Sleep(timeout);

                    AutHelper.RestoreFromTaskBar();
                    Thread.Sleep(timeout);
                    iter++;

                } while (iter < iterationQuantity);
            }
            catch (Exception e)
            {
                if (isFailed == true)
                {
                    Assert.Fail("Application is not responding");
                }
                else
                {
                    Assert.Fail(e.Message + "   " + e.StackTrace);
                }
            }
            finally
            {
                isFinalizing = true;
                checkerThread.Join();
                AutHelper.CloseApp();
            }
        }

        string instanceName = Settings.Default.ApplicationName;
        private static void checkAppRunner(Tests host)
        {
            while (!host.isFinalizing && Process.GetProcessesByName(host.applicationName).All(p => p.Responding))
            {
                Thread.Sleep(1000);
            }

            if (host.isFinalizing)
            {
                return;
            }

            host.isFailed = true;
            AutHelper.logLog("Application is not responding");
        }
    }
}
