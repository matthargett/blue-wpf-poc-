using log4net;
using Microsoft.VisualStudio.TestTools.UITest.Extension;
using Microsoft.VisualStudio.TestTools.UITesting;
using Microsoft.VisualStudio.TestTools.UITesting.WinControls;
using Microsoft.VisualStudio.TestTools.UITesting.WpfControls;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Threading;
using static BJ.UIMediaStreamerUIMainWWindow;

namespace BJ
{
    class Helper
    {
        public static readonly ILog log = LogManager.GetLogger(typeof(Helper));
       
        ResourceCounter resourceCounter = new ResourceCounter();

        #region Configuration settings
        public static readonly string ApplicationPath = Settings1.Default.ApplicationPath;
        public static readonly bool isLaunchApp = Settings1.Default.LaunchApp;
        #endregion

        public string UIMainWindowWindowExePath = ApplicationPath;
        private UIRunningapplicationsWindow mUIRunningapplicationsWindow;
        private UIMainWindowWindow mUIMainWindowWindow1;
        private UIMediaStreamerUIMainWWindow mUIMediaStreamerUIMainWWindow;
        string instanceName = "MediaStreamer.UI";





        public UIRunningapplicationsWindow UIRunningapplicationsWindow
        {
            get
            {
                if ((this.mUIRunningapplicationsWindow == null))
                {
                    this.mUIRunningapplicationsWindow = new UIRunningapplicationsWindow();
                }
                return this.mUIRunningapplicationsWindow;
            }
        }


        public UIMainWindowWindow UIMainWindowWindow1
        {
            get
            {
                if ((this.mUIMainWindowWindow1 == null))
                {
                    this.mUIMainWindowWindow1 = new UIMainWindowWindow();
                }
                return this.mUIMainWindowWindow1;
            }
        }

        public UIMediaStreamerUIMainWWindow UIMediaStreamerUIMainWWindow
        {
            get
            {
                if ((this.mUIMediaStreamerUIMainWWindow == null))
                {
                    this.mUIMediaStreamerUIMainWWindow = new UIMediaStreamerUIMainWWindow();
                }
                return this.mUIMediaStreamerUIMainWWindow;
            }

        }

        public void LaunchApp()
        {
            if (isLaunchApp)
            {
                log.Info("Launch application" + resourceCounter.getCpuAndRamUsage());
                
                ApplicationUnderTest uIMainWindowWindow = ApplicationUnderTest.Launch(UIMainWindowWindowExePath);
                log.Info("Application started" + resourceCounter.getCpuAndRamUsage());
               
            }
            else
            {
                log.Info("Property 'isLaunchApp' set to False, application must be started manually");
            }
        }

        public void ClickCallButton()
        {
            WpfButton uIItemButton = UIMainWindowWindow1.UIItemButton;
            
            Mouse.Click(uIItemButton);
            log.Info("Click Call Button " + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
        }

        public void MaximizeWindow()
        {
            log.Info("Maximazing " + resourceCounter.getCpuAndRamUsage());
            UIMediaStreamerUIMainWWindow.Maximized = true;
            log.Info("Maximazed" + resourceCounter.getCpuAndRamUsage());
        }

        internal void RestoreFromMaxSize()
        {
            log.Info("Restore from full screen" + resourceCounter.getCpuAndRamUsage());
            UIMediaStreamerUIMainWWindow.Maximized = false;
            log.Info("Restored from full screen" + resourceCounter.getCpuAndRamUsage());
        }

        public void CloseApp()
        {
            WinButton uICloseButton = UIMediaStreamerUIMainWWindow.UICloseButton;
            log.Info("Close application" + resourceCounter.getFinalUsage());
            PLogger.WriteCurrentStatistics();
            Mouse.Click(uICloseButton, new Point(36, 11));
        }

        public void MinimizeWindow()
        {
            WinButton uIMinimizeButton = UIMediaStreamerUIMainWWindow.UIMinimizeButton;
            log.Info("Minimize to task bar" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
            Mouse.Click(uIMinimizeButton);
            Thread.Sleep(2000);
            //Assert.IsTrue(UIMediaStreamerUIMainWWindow.State == ControlStates.Minimized);
        }


        public void RestoreFromTaskBar()
        {

            WinButton uIMediaStreamerUIMainWButton = UIRunningapplicationsWindow.UIRunningapplicationsToolBar.UIMediaStreamerUIMainWButton;
            Mouse.Click(uIMediaStreamerUIMainWButton);
            log.Info("Restore application from task bar" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
            try
            {
                Assert.IsTrue(UIMediaStreamerUIMainWWindow.State == ControlStates.Restored);
            } 
            catch (AssertFailedException)
            {
                Mouse.Click(uIMediaStreamerUIMainWButton);
            }
        }

        public void ClickOnTitleBar()
        {
            WinTitleBar uIMainWindowTitleBar = UIMediaStreamerUIMainWWindow.UIMediaStreamerUIMainWTitleBar;

            Mouse.Click(uIMainWindowTitleBar, new Point(906, 11));
        }

        public void MoveWindowUp()
        {

            Point midPointOfTitle = new Point(getTitleBar().BoundingRectangle.X + getTitleBar().BoundingRectangle.Width / 2, getTitleBar().BoundingRectangle.Y + getTitleBar().BoundingRectangle.Height / 2);

            Mouse.Click(getTitleBar());
            Mouse.StartDragging(getTitleBar(), midPointOfTitle);
            log.Info("Start moving up" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();

            Mouse.StopDragging(-500, -100);// (mUIMainWindowWindow, 500, 100);
            log.Info("Stop moving up" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();

        }


        public void MoveWindowDown()
        {

            Point midPointOfTitle = new Point(getTitleBar().BoundingRectangle.X + getTitleBar().BoundingRectangle.Width / 2, getTitleBar().BoundingRectangle.Y + getTitleBar().BoundingRectangle.Height / 2);

            Mouse.Click(getTitleBar());
            Mouse.StartDragging(getTitleBar(), midPointOfTitle);
            log.Info("Start moving down" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
            Mouse.StopDragging(500, 100);
            log.Info("Stop moving down" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
        }

        public void ResizeWindowSmall()
        {
            Mouse.Click(getTitleBar());
            log.Info("Start make it smaller" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
            CodedUITest.SetWindowPos(mainWindow().WindowHandle, (IntPtr)(-1), 0, 0, 500, 300, 0x0002 | 0x0040);

            log.Info("Stop resize" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();

        }

        public void CheckIfWindowExist()
        {
            Assert.IsTrue(UIMediaStreamerUIMainWWindow.Exists, "MediaStreamer main window is absent on screen");
        }

        public void ResizeWindowBig()
        {
            Mouse.Click(getTitleBar());
            log.Info("Start make it bigger" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();
            CodedUITest.SetWindowPos(mainWindow().WindowHandle, (IntPtr)(-1), 0, 0, 1000, 600, 0x0002 | 0x0040);

            log.Info("Stop resize" + resourceCounter.getCpuAndRamUsage());
            PLogger.WriteCurrentStatistics();

        }

        public WinWindow mainWindow()
        {
            return UIMediaStreamerUIMainWWindow;
        }

        public WinTitleBar getTitleBar()
        {
            return UIMediaStreamerUIMainWWindow.UIMediaStreamerUIMainWTitleBar;
        }

        public void failTest(string message)
        {
            log.Info(message);
            foreach (var process in Process.GetProcessesByName(instanceName))
            {
                process.Kill();
            }          

        }

        public bool isResponding()
        {
            return Process.GetProcessesByName(instanceName).All(p => p.Responding);
                        
        }

        public Thread checkApp(CodedUITest host)
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

        public void logLog(string message)
        {
            log.Info(message);
        }

    }
}