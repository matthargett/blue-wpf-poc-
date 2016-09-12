using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Interaction logic for FloatingToolbar.xaml
    /// </summary>
    public partial class FloatingToolbarControl : UserControl
    {
        public event Action OnRedButtonClick;

        public FloatingToolbarControl()
        {
            InitializeComponent();
        }

        private Point startPoint;

        private void Button_PreviewMouseLeftButtonDown(
            object sender, MouseButtonEventArgs e)
        {
            startPoint = e.GetPosition((IInputElement)sender);
        }

        private void Button_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            var input = (IInputElement)sender;

            var currentPoint = e.GetPosition(input);
            if (e.LeftButton == MouseButtonState.Pressed &&
                input.IsMouseCaptured &&
                (Math.Abs(currentPoint.X - startPoint.X) >
                    SystemParameters.MinimumHorizontalDragDistance ||
                Math.Abs(currentPoint.Y - startPoint.Y) >
                    SystemParameters.MinimumVerticalDragDistance))
            {
                // Prevent Click from firing
                input.ReleaseMouseCapture();
                Window.GetWindow(this).DragMove();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (OnRedButtonClick != null)
            {
                OnRedButtonClick();
            }
        }
    }
}
