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
    /// Interaction logic for FloatingToolbarWindow.xaml
    /// </summary>
    public partial class FloatingToolbarWindow : Window
    {
        private readonly MainWindow _mainWindow;

        public FloatingToolbarWindow(MainWindow mainWindow)
        {
            _mainWindow = mainWindow;
            InitializeComponent();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void FloatingToolbarControl_OnRedButtonClick(object sender, RoutedEventArgs e)
        {
            _mainWindow.StartStopVideo();
        }
    }
}
