using System;
using Microsoft.VisualStudio.TestTools.UITesting;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Runtime.InteropServices;
using System.Threading;

namespace BJ
{
    [CodedUITest]
    public class CodedUITest
    {
        public CodedUITest()
        {
        }

        Helper helper;

        [TestMethod]
        [Timeout(9000000)]
        public void CrazyWindowTest()
        {

            helper = new Helper();
            PLogger.InitPLogger("MediaStreamer");
            helper.LaunchApp();

            if (Settings1.Default.ClickCallButton) helper.ClickCallButton();

            DateTime time = DateTime.Now;
            DateTime finishTime = time.AddHours(2);

            Thread checkerThread = helper.checkApp(this);

            try
            {
                checkerThread.Start();

                do
                {
                    helper.MoveWindowDown();
                    Thread.Sleep(1000);

                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);
                    //helper.MinimizeWindow();
                    //Thread.Sleep(1000);
                    //helper.RestoreFromTaskBar();
                    //Thread.Sleep(1000);

                    helper.MoveWindowUp();
                    Thread.Sleep(1000);

                    helper.ResizeWindowSmall();
                    Thread.Sleep(1000);

                    helper.ResizeWindowBig();
                    Thread.Sleep(1000);
                    helper.MoveWindowDown();
                    Thread.Sleep(1000);
                    helper.MoveWindowUp();
                    Thread.Sleep(1000);

                    helper.ResizeWindowSmall();
                    Thread.Sleep(1000);
                    helper.MoveWindowDown();
                    Thread.Sleep(1000);
                    helper.MoveWindowUp();
                    Thread.Sleep(1000);

                    helper.MinimizeWindow();
                    Thread.Sleep(1000);
                    helper.RestoreFromTaskBar();
                    Thread.Sleep(1000);
                    helper.ResizeWindowBig();
                    Thread.Sleep(1000);
                    helper.ResizeWindowSmall();
                    Thread.Sleep(1000);
                    helper.MoveWindowDown();
                    Thread.Sleep(1000);

                    helper.ResizeWindowBig();
                    Thread.Sleep(1000);
                    helper.MinimizeWindow();
                    Thread.Sleep(1000);
                    helper.RestoreFromTaskBar();
                    Thread.Sleep(1000);
                    time = DateTime.Now;



                } while (time < finishTime);
                helper.CloseApp();
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
        }

        [TestMethod]
        [Timeout(9000000)]
        public void startStopVideo()
        {
            helper = new Helper();
            PLogger.InitPLogger("ClickCallButton");
            helper.LaunchApp();

            DateTime time = DateTime.Now;
            DateTime finishTime = time.AddHours(2);

            Thread checkerThread = helper.checkApp(this);

            try
            {
                checkerThread.Start();

                do
                {
                    helper.ClickCallButton();
                    Thread.Sleep(20000);
                    helper.ClickCallButton();
                    Thread.Sleep(20000);
                    time = DateTime.Now;

                } while (time < finishTime);
                helper.CloseApp();
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
        }

        [TestMethod]
        [Timeout(9000000)]
        public void launchDropApp()
        {
            helper = new Helper();
            PLogger.InitPLogger("LaunchDropApp");
            DateTime time = DateTime.Now;
            DateTime finishTime = time.AddHours(1);

            Thread checkerThread = helper.checkApp(this);

            try
            {
                checkerThread.Start();
                do
                {
                    helper.LaunchApp();

                    helper.ClickCallButton();
                    Thread.Sleep(10000);

                    helper.MinimizeWindow();
                    Thread.Sleep(20000);

                    helper.RestoreFromTaskBar();
                    Thread.Sleep(20000);

                    helper.CloseApp();
                    Thread.Sleep(2000);
                    time = DateTime.Now;


                } while (time < finishTime);

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
        }

        public bool isFailed = false;

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool SetWindowPos(IntPtr hWnd, IntPtr hwndInsertAfter, int x, int y, int cx, int cy, uint uFlags);

        public UIMap UIMap
        {
            get
            {
                if ((this.map == null))
                {
                    this.map = new UIMap();
                }

                return this.map;
            }
        }

        private UIMap map;

    }
}
