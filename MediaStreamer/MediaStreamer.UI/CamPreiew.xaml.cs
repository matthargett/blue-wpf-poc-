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
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Interaction logic for CamPreiew.xaml
    /// </summary>
    public partial class CamPreiew : Window
    {
        private MainWindow _mainWindow;

        public D3DImage WebcamSurface
        {
            get { return this.previewCamStream; }
        }

        public Label WebcamFpsLabel
        {
            get { return this.previewCamFpsLabel; }
        }

        public CamPreiew(MainWindow mainWindow)
        {
            _mainWindow = mainWindow;

            InitializeComponent();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }
    }
}
