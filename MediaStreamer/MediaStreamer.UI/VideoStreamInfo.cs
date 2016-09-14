using System.Windows.Controls;
using System.Windows.Interop;

namespace MediaStreamer.UI
{
    internal abstract class VideoStreamInfo
    {
        private int externalFps = 0;

        public int Index { get; set; }
        public D3DImage[] Images { get; private set; }
        public Label[] FpsLabels { get; private set; }
        public bool IsRunning { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public bool IsNewFrameReady { get; set; }

        protected VideoStreamInfo(D3DImage[] images, Label[] fpsLabels)
        {
            Images = images;
            FpsLabels = fpsLabels;
        }

        public void SetFps(int fps)
        {
            externalFps = fps;
        }

        public void FlushFpsToScreen()
        {
            foreach (Label label in FpsLabels)
            {
                label.Content = $"FPS: {externalFps}";
            }
        }
    }

    internal class VideoFileStreamInfo : VideoStreamInfo
    {
        public string Source { get; private set; }

        public VideoFileStreamInfo(string source, D3DImage[] images, Label[] fpsLabels) : base(images, fpsLabels)
        {
            Source = source;
        }
    }

    internal class WebcamStreamInfo : VideoStreamInfo
    {
        public int WebcamIndex { get; private set; }
        public WebcamStreamInfo(int webcamIndex, D3DImage[] images, Label[] fpsLabels) : base(images, fpsLabels)
        {
            WebcamIndex = webcamIndex;
        }
    }

}
