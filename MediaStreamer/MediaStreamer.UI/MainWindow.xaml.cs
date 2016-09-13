using MediaStreamer.UI.Properties;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;

namespace MediaStreamer.UI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Streamer streamer = null;

        private bool _isStreaming = true;
        public bool IsStreaming
        {
            get { return _isStreaming; }
            set
            {
                _isStreaming = value;
            }
        }

        private CamPreiew _camPreviewSeparateWindow;
        private bool _isCamPreviewWindowClosed = false;

        private readonly WebcamStreamInfo _webcamStream;

        private readonly VideoFileStreamInfo[] _previewStreams;
        private readonly Dictionary<D3DImage, VideoStreamInfo> _streamInfoByImage = new Dictionary<D3DImage, VideoStreamInfo>();
        private readonly Dictionary<int, VideoStreamInfo> _streamInfoByIndex = new Dictionary<int, VideoStreamInfo>();
        private readonly Dictionary<D3DImage, Window> _windowByImage = new Dictionary<D3DImage, Window>();

        private WindowState _stateBeforeMinimize = WindowState.Normal;

        private Stopwatch _timeFromLaunch = new Stopwatch();
        private DispatcherTimer _timeUpdateTimer = new DispatcherTimer();

        public MainWindow()
        {
            InitializeComponent();

            CompositionTarget.Rendering += CompositionTarget_Rendering;

            _camPreviewSeparateWindow = new CamPreiew(this);
            _camPreviewSeparateWindow.Closing += camPreviewSeparateWindow_Closing;

            _webcamStream = new WebcamStreamInfo(Settings.Default.CameraIndex,
                new[] { mainStreamImage, camStreamImage, _camPreviewSeparateWindow.WebcamSurface },
                new[] { mainStreamFpsLabel, camStreamFpsLabel, _camPreviewSeparateWindow.WebcamFpsLabel });
            _previewStreams = new VideoFileStreamInfo[]
            {
                new VideoFileStreamInfo(ConfigurationManager.AppSettings["previewStream1FilePath"], new[] { previewStream1Image }, new[] { previewStream1FpsLabel }),
                new VideoFileStreamInfo(ConfigurationManager.AppSettings["previewStream2FilePath"], new[] { previewStream2Image }, new[] { previewStream2FpsLabel }),
                new VideoFileStreamInfo(ConfigurationManager.AppSettings["previewStream3FilePath"], new[] { previewStream3Image }, new[] { previewStream3FpsLabel }),
                new VideoFileStreamInfo(ConfigurationManager.AppSettings["previewStream4FilePath"], new[] { previewStream4Image }, new[] { previewStream4FpsLabel }),
                new VideoFileStreamInfo(ConfigurationManager.AppSettings["previewStream5FilePath"], new[] { previewStream5Image }, new[] { previewStream5FpsLabel })
            };

            AddImageKey(mainStreamImage, _webcamStream, this);
            AddImageKey(camStreamImage, _webcamStream, this);
            AddImageKey(_camPreviewSeparateWindow.WebcamSurface, _webcamStream, _camPreviewSeparateWindow);
            AddImageKey(previewStream1Image, _previewStreams[0], this);
            AddImageKey(previewStream2Image, _previewStreams[1], this);
            AddImageKey(previewStream3Image, _previewStreams[2], this);
            AddImageKey(previewStream4Image, _previewStreams[3], this);
            AddImageKey(previewStream5Image, _previewStreams[4], this);

            FloatingToolbarWindow toolbarWindow = new FloatingToolbarWindow(this);
            toolbarWindow.Show();

            this.DataContext = this;
            IsStreaming = false;
            _sampleReadyCallback = new Streamer.RenderCallback(SampleReadyCallback);

            _timeUpdateTimer.Tick += timeUpdateTimer_Tick;
            _timeUpdateTimer.Interval = new TimeSpan(0, 0, 0, 0, 500);

            Application.Current.Deactivated += Window_Deactivated;
        }

        private void camPreviewSeparateWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _isCamPreviewWindowClosed = true;
        }

        private void AddImageKey(D3DImage key, VideoStreamInfo streamInfo, Window window)
        {
            _streamInfoByImage[key] = streamInfo;
            _windowByImage[key] = window;
        }

        private void timeUpdateTimer_Tick(object sender, EventArgs e)
        {
            labelTime.Content = _timeFromLaunch.Elapsed.ToString(@"mm\:ss");

            CommandManager.InvalidateRequerySuggested();
        }

        public void StartStopVideo()
        {
            if (IsStreaming)
            {
                foreach (VideoStreamInfo streamInfo in _previewStreams)
                {
                    StopDXRendering(streamInfo);
                }
                StopDXRendering(_webcamStream);

                _timeFromLaunch.Stop();
                _timeUpdateTimer.Stop();
            }
            else
            {
                foreach (VideoStreamInfo streamInfo in _previewStreams)
                {
                    StartDXRendering(streamInfo);
                }
                StartDXRendering(_webcamStream);

                _timeFromLaunch.Restart();
                _timeUpdateTimer.Start();
            }

            IsStreaming = !IsStreaming;
        }

        


        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (!IsStreaming)
            {
                return;
            }

            foreach (VideoStreamInfo streamInfo in _streamInfoByIndex.Values)
            {
                if (!streamInfo.IsNewFrameReady)
                {
                    continue;
                }

                foreach (D3DImage image in streamInfo.Images)
                {
                    Window wnd = _windowByImage[image];
                    if (!wnd.IsVisible || wnd.WindowState == WindowState.Minimized)
                        continue;

                    image.Lock();
                    image.AddDirtyRect(new Int32Rect(0, 0, streamInfo.Width, streamInfo.Height));
                    image.Unlock();
                }

                streamInfo.IsNewFrameReady = false;

                streamInfo.FlushFpsToScreen();
            }
        }

        private Streamer.RenderCallback _sampleReadyCallback;

        private void StartDXRendering(VideoStreamInfo streamInfo)
        {
            this.WindowState = _stateBeforeMinimize;

            IntPtr scene;
            int videoWidth, videoHeight;

            if (streamInfo is WebcamStreamInfo)
            {
                bool isHwEnabled = false;
                streamInfo.Index = streamer.StartStream(((WebcamStreamInfo)streamInfo).WebcamIndex, out videoWidth, out videoHeight, out scene, out isHwEnabled, _sampleReadyCallback);
                camHwEnabled.Visibility = isHwEnabled ? Visibility.Visible : Visibility.Hidden;
            }
            else if (streamInfo is VideoFileStreamInfo)
            {
                streamInfo.Index = streamer.StartStream(((VideoFileStreamInfo)streamInfo).Source, out videoWidth, out videoHeight, out scene, _sampleReadyCallback);
            }
            else { throw new ApplicationException(); }

            _streamInfoByIndex[streamInfo.Index] = streamInfo;

            streamInfo.Width = videoWidth;
            streamInfo.Height = videoHeight;

            foreach (D3DImage image in streamInfo.Images)
            {
                image.Lock();
                image.SetBackBuffer(D3DResourceType.IDirect3DSurface9, scene, true);
                image.Unlock();
            }

            streamer.Next(streamInfo.Index);

            streamInfo.IsRunning = true;
        }

        private void SampleReadyCallback(int index, int fps)
        {
            Dispatcher.InvokeAsync(() =>
            {
                VideoStreamInfo streamInfo = _streamInfoByIndex[index];

                if (!streamInfo.IsRunning)
                {
                    return;
                }

                if (WindowState == WindowState.Minimized && !(streamInfo is WebcamStreamInfo))
                {
                    return;
                }

                streamer.Next(index);

                streamInfo.IsNewFrameReady = true;
                streamInfo.SetFps(fps);
            });
        }

        private void StopDXRendering(VideoStreamInfo streamInfo)
        {
            streamInfo.IsRunning = false;

            foreach (D3DImage image in streamInfo.Images)
            {
                image.Lock();
                image.SetBackBuffer(D3DResourceType.IDirect3DSurface9, IntPtr.Zero);
                image.Unlock();
            }

            streamer.StopStream(streamInfo.Index);
            streamInfo.SetFps(0);
        }


        private Point _anchorPoint = new Point(0, 0);
        private Point _currentPoint;
        private TranslateTransform _camPreviewTransform = new TranslateTransform();
        private bool _isCamStreamWrapperDragging = false;

        private void SetCamStreamWrapperTransform(double deltaX, double deltaY)
        {
            Point camStreamWrapperPos = camStreamWrapper.TransformToAncestor(gridTopWrapper).Transform(new Point(0, 0));

            double nextPosX = camStreamWrapperPos.X + deltaX;
            double nextPosY = camStreamWrapperPos.Y + deltaY;

            if (nextPosX >= 0 && nextPosX + camStreamWrapper.ActualWidth <= gridTopWrapper.ActualWidth)
            {
                _camPreviewTransform.X += deltaX;
            }

            if (nextPosY >= 0 && nextPosY + camStreamWrapper.ActualHeight <= gridTopWrapper.ActualHeight)
            {
                _camPreviewTransform.Y += deltaY;
            }

            camStreamWrapper.RenderTransform = _camPreviewTransform;
        }

        private void camPreviewWindow_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isCamStreamWrapperDragging)
            {
                _currentPoint = e.GetPosition(null);

                SetCamStreamWrapperTransform(_currentPoint.X - _anchorPoint.X, _currentPoint.Y - _anchorPoint.Y);

                _anchorPoint = _currentPoint;
            }
        }


        private void camPreviewWindow_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var element = sender as FrameworkElement;
            _anchorPoint = e.GetPosition(null);
            element.CaptureMouse();
            _isCamStreamWrapperDragging = true;
            e.Handled = true;
        }

        private void camPreviewWindow_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (_isCamStreamWrapperDragging)
            {
                var element = sender as FrameworkElement;
                element.ReleaseMouseCapture();
                _isCamStreamWrapperDragging = false;
                e.Handled = true;
            }
        }


        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            StartStopVideo();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            streamer = new Streamer(new WindowInteropHelper(this).Handle);
        }

        private void Window_Deactivated(object sender, EventArgs e)
        {
            if (_isCamPreviewWindowClosed)
            {
                return;
            }

            if (IsStreaming && !_camPreviewSeparateWindow.IsVisible)
            {
                _camPreviewSeparateWindow.Show();
            }
        }

        private void Window_Activated(object sender, EventArgs e)
        {
            if (_isCamPreviewWindowClosed)
            {
                return;
            }

            if (IsStreaming && _camPreviewSeparateWindow.IsVisible)
            {
                _camPreviewSeparateWindow.Hide();
            }
        }

        private void Window_StateChanged(object sender, EventArgs e)
        {
            if (IsStreaming)
            {
                if (WindowState == WindowState.Minimized && !_camPreviewSeparateWindow.IsVisible)
                {
                    _camPreviewSeparateWindow.Show();
                }
                else if (WindowState != WindowState.Minimized && _camPreviewSeparateWindow.IsVisible)
                {
                    _camPreviewSeparateWindow.Hide();
                }

                if (WindowState == WindowState.Minimized)
                    return;

                foreach (VideoStreamInfo streamInfo in _previewStreams)
                {
                    streamer.Next(streamInfo.Index);
                }
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            _stateBeforeMinimize = this.WindowState;

            labelTitle.FontSize = Math.Max(24, this.ActualWidth / 53);

            _camPreviewTransform.X = 0;
            _camPreviewTransform.Y = 0;
            camStreamWrapper.RenderTransform = _camPreviewTransform;
        }

    }
}