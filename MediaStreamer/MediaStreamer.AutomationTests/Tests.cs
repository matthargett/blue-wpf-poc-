using MediaStreamer.AutomationTests.Helpers;
using MediaStreamer.AutomationTests.Logger;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Threading;

namespace MediaStreamer.AutomationTests
{
    [TestClass]
    public class Tests
    {
        [TestMethod]
        [Timeout(9000000)]
        public void MainTest()
        {
            PLogger.InitPLogger("MainTest");
            AutHelper.LaunchApp();

            if (Settings.Default.ClickCallButton) AutHelper.StarStopVideoButton.ClickButton();

            DateTime time = DateTime.Now;
            //DateTime finishTime = time.AddHours(2);

            TimeSpan timeout = new TimeSpan(0, 0, 0, 0, 500);

            Thread checkerThread = new CheckAppStatus().checkApp(this);
            try
            {
                checkerThread.Start();

                //do
                //{
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

                    time = DateTime.Now;
                //} while (time < finishTime);
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
                AutHelper.CloseApp();
            }
        }

        public bool isFailed = false;
    }
}
