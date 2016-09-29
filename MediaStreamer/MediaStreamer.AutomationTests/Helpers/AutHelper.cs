using MediaStreamer.AutomationTests.Logger;
using log4net;
using System;
using System.Diagnostics;
using System.Threading;
using System.Windows.Automation;

namespace MediaStreamer.AutomationTests.Helpers
{
    public static class AutHelper
    {
        private static readonly ILog Log = LogManager.GetLogger(typeof(AutHelper).Name);

        private static AutomationElement mainWindow = null;
        private static AutomationElement runningApplications = null;

        public static void LaunchApp()
        {
            if (Process.GetProcessesByName("MediaStreamer.UI").Length <= 0)
            {
                Process.Start(Settings.Default.ApplicationPath);
            }
            //killProcess("MediaStreamer.UI");

            CommonHelper.WaitForTrue(() => MainWindow != null, 10, 1000);
        }

        public static void CloseApp()
        {
            CloseButton.ClickButton();
            killProcess("MediaStreamer.UI");
        }

        private static void killProcess(string name)
        {
            var appProcesses = Process.GetProcessesByName(name);
            if (appProcesses.Length != 0)
            {
                foreach (var proc in appProcesses)
                {
                    proc.Kill();
                }
            }

        }

        public static AutomationElement MainWindow
        {
            get
            {
                if (mainWindow == null)
                    mainWindow = AutomationElement.RootElement.FindFirst(TreeScope.Children, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Window),
                    new PropertyCondition(AutomationElement.AutomationIdProperty, "MainWindow")));
                return mainWindow;
            }
        }

        public static AutomationElement RunningApplications
        {
            get
            {
                if (runningApplications == null)
                    runningApplications = AutomationElement.RootElement.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.ToolBar),
                    new PropertyCondition(AutomationElement.NameProperty, "Running applications")));
                return runningApplications;
            }
        }

        private static AutomationElement AppOnTaskBar
        {
            get
            {
                return RunningApplications.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Button),
                    new PropertyCondition(AutomationElement.NameProperty, "MediaStreamer.UI - MainWindow")));
            }
        }

        public static void RestoreFromTaskBar()
        {
            AppOnTaskBar.ClickButton();
            var windowPattern = MainWindow.GetCurrentPattern(WindowPattern.Pattern) as WindowPattern;
            if (windowPattern.Current.WindowVisualState.Equals(WindowVisualState.Minimized))
            {
                AppOnTaskBar.ClickButton();
            }
            PLogger.WriteCurrentStatistics("After restore from task bar: ");
        }



        public static AutomationElement MainWindowToolbar
        {
            get
            {
                return AutomationElement.RootElement.FindFirst(TreeScope.Children, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Window),
                    new PropertyCondition(AutomationElement.NameProperty, "MediaStreamer.UI - Toolbar")));
            }
        }

        public static AutomationElement TitleBar
        {
            get
            {
                return MainWindow.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.TitleBar),
                    new PropertyCondition(AutomationElement.LocalizedControlTypeProperty, "title bar")));
            }
        }

        public static AutomationElement StarStopVideoButton
        {
            get
            {
                return MainWindowToolbar.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Button),
                    new PropertyCondition(AutomationElement.AutomationIdProperty, "StartStopButton")));
            }
        }

        private static AutomationElement MinimizeButton
        {
            get
            {
                return MainWindow.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Button),
                    new PropertyCondition(AutomationElement.NameProperty, "Minimize")));
            }
        }

        public static void MinimizeWindow()
        {
            MinimizeButton.ClickButton();
            PLogger.WriteCurrentStatistics("After window minimize: ");
        }

        private static AutomationElement CloseButton
        {
            get
            {
                return MainWindow.FindFirst(TreeScope.Descendants, new AndCondition(
                    new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Button),
                    new PropertyCondition(AutomationElement.NameProperty, "Close")));
            }
        }

        public static void ClickButton(this AutomationElement element)
        {
            PLogger.WriteCurrentStatistics("Before call button click: ");

            MouseOver(element);
            var invokePattern = element.GetCurrentPattern(InvokePattern.Pattern) as InvokePattern;
            invokePattern.Invoke();

            PLogger.WriteCurrentStatistics("After call button click: ");
        }

        public static void MoveWindowDown()
        {
            PLogger.WriteCurrentStatistics("Before move window down: ");

            MouseMoveEmulationAndUp(TitleBar, 450, 115);

            PLogger.WriteCurrentStatistics("After move window down: ");
        }

        public static void MoveWindowUp()
        {
            PLogger.WriteCurrentStatistics("Before move window up: ");

            MouseMoveEmulationAndUp(TitleBar, -500, -100);

            PLogger.WriteCurrentStatistics("After move window up: ");
        }

        private static void MouseMoveEmulationAndUp(AutomationElement element, int x_offset, int y_offset)
        {
            MouseOver(element);
            var centerPoint = element.GetCenterPoint();
            int xpoint = Convert.ToInt32(centerPoint.X + x_offset);
            int ypoint = Convert.ToInt32(centerPoint.Y + y_offset);

            MouseEmulate.MouseDown(MouseButton.Left);
            CommonHelper.MouseMoveEmulationAndUp(xpoint, ypoint);
        }

        private static void MouseOver(AutomationElement element)
        {
            var rect = element.Current.BoundingRectangle;
            MouseOver(element, Convert.ToInt32(rect.Width / 2), Convert.ToInt32(rect.Height / 2));
        }

        private static void MouseOver(AutomationElement element, int x_offset, int y_offset)
        {
            var rect = element.Current.BoundingRectangle;
            int xpoint = (int)(rect.X + x_offset);
            int ypoint = (int)(rect.Y + y_offset);

            Console.WriteLine("Clicking on X axis: {0}, and on Y axix {1}", xpoint, ypoint);
            MouseEmulate.Move(xpoint, ypoint);
            Thread.Sleep(500);
        }

        public static void ResizeWindowBig()
        {
            PLogger.WriteCurrentStatistics("Before window resize big: ");

            Resize(MainWindow, 1000, 600);

            PLogger.WriteCurrentStatistics("After window resize big: ");
        }

        public static void ResizeWindowSmall()
        {
            PLogger.WriteCurrentStatistics("Before window resize small: ");

            Resize(MainWindow, 500, 300);

            PLogger.WriteCurrentStatistics("After window resize small: ");
        }

        private static void Resize(AutomationElement element, double x, double y)
        {
            if (IsSupportPattern(element, TransformPattern.Pattern))
            {
                var transformPattern = (TransformPattern)element.GetCurrentPattern(TransformPattern.Pattern);
                transformPattern.Resize(x, y);
            }
        }

        private static bool IsSupportPattern(AutomationElement element, AutomationPattern pattern)
        {
            object obj;

            if (element.TryGetCurrentPattern(pattern, out obj))
                return true;
            else
                return false;
        }
        public static void logLog(string message)
        {
            Log.Info(message);
        }
    }
}
