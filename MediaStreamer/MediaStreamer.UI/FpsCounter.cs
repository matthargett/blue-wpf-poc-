using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace MediaStreamer.UI
{
    class FpsCounter
    {
        public void SetOutput(string legend, Label l)
        {
            _legend = legend;
            _label = l;
        }

        public void Restart()
        {
            _fpsCounter = 0;
            _timer.Restart();
        }

        public void Tick()
        {
            _fpsCounter++;
            if (_timer.ElapsedMilliseconds >= 1000)
            {
                _timer.Restart();
                if (_label != null)
                    _label.Content = $"{_legend}: {_fpsCounter} fps";
                _fpsCounter = 0;
            }
        }

        private int _fpsCounter = 0;
        private Stopwatch _timer = Stopwatch.StartNew();
        private Label _label;
        private string _legend;
    }
}
