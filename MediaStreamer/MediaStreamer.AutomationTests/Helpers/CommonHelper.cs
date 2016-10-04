using log4net;
using System;
using System.Threading;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Forms;

namespace MediaStreamer.AutomationTests.Helpers
{
    public static class CommonHelper
    {
        private static readonly ILog Log = LogManager.GetLogger(typeof(CommonHelper).Name);
        public static T WaitForObject<T>(Func<T> method, int cycles = 30, int sleepMseconds = 500) where T : class
        {
            T element = null;
            int count = 0;
            while (element == null && count < cycles)
            {
                try { element = method.Invoke(); }
                catch (Exception e) { Log.WarnFormat("Exception appears during method invoke. StackTrace: '{0}'", e.StackTrace); }

                count++;
                Thread.Sleep(sleepMseconds);
            }

            return element;
        }

        public static void WaitForTrue(Func<bool> method, int cycles = 100, int sleepMseconds = 500)
        {
            bool result = false;
            int count = 0;
            while (!result && count < cycles)
            {
                try { result = method.Invoke(); }
                catch (Exception e) { Log.WarnFormat("Exception appears during method invoke. StackTrace: '{0}'", e.StackTrace); }

                count++;
                Thread.Sleep(sleepMseconds);
            }
        }


        public static void MouseMoveEmulationAndUp(int xTarget, int yTarget)
        {
            int xCurrent = Cursor.Position.X;
            int yCurrent = Cursor.Position.Y;

            Console.WriteLine($"Mouse Move and Up on X axis: {xTarget}, and on Y axix {yTarget}");

            int yIncrement = (yCurrent < yTarget) ? 1 : (-1);
            int xIncrement = (xCurrent < xTarget) ? 1 : (-1);

            while (yCurrent != yTarget && xCurrent != xTarget)
            {
                Thread.Sleep(10);
                yCurrent += yIncrement;
                xCurrent += xIncrement;
                MouseEmulate.Move(xCurrent, yCurrent);
            }            

            //while (xCurrent != xTarget)
            //{
            //    Thread.Sleep(10);
               
            //    MouseEmulate.Move(xCurrent, yTarget);
            //};

            Thread.Sleep(1000);
            MouseEmulate.MouseUp(MouseButton.Left);
        }

        public static Point GetCenterPoint(this AutomationElement element)
        {
            return new Point(element.Current.BoundingRectangle.X + element.Current.BoundingRectangle.Width / 2, element.Current.BoundingRectangle.Y + element.Current.BoundingRectangle.Height / 2);
        }
    }
   
}
