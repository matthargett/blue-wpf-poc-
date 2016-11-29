using System;
using NUnit.Framework;
using MediaStreamer.UI;
using Moq;

namespace MediaStreamer.Tests
{
    [TestFixture]
    public class StreamerTests
    {
        private Mock<IMediaDeviceWrapper> _mediaDeviceWrapperMock;

        [SetUp]
        public void SetupTests()
        {
            _mediaDeviceWrapperMock = new Mock<IMediaDeviceWrapper>();
        }

        [Test]
        public void InitIsCalledInConstructorWithParameter()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            _mediaDeviceWrapperMock.Verify(m => m.Init(It.Is<IntPtr>(i => i.ToInt32() == 12345)));
        }

        [Test]
        public void CreatePipelineIsCalledOnStartStreamForFile()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            int width, height;
            IntPtr surface;
            RenderCallback callback = (int x, int y) => { };
            streamer.StartStream("foo", out width, out height, out surface, callback);

            _mediaDeviceWrapperMock.Verify(m => m.CreatePipeline(
                It.Is<string>(s => s == "foo"),
                out surface,
                ref width,
                ref height,
                It.Is<RenderCallback>(c => c == callback)));
        }

        [Test]
        public void CreateCamStreamIsCalledOnStartStreamFromCamera()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            int width, height;
            bool isHwEnabled;
            IntPtr surface;
            RenderCallback callback = (int x, int y) => { };
            streamer.StartStream(12345, out width, out height, out surface, out isHwEnabled, callback);

            _mediaDeviceWrapperMock.Verify(m => m.CreateCamStream(
                It.Is<int>(i => i == 12345),
                It.Is<int>(i => i == 0),
                out surface,
                ref width,
                ref height,
                ref isHwEnabled,
                It.Is<RenderCallback>(c => c == callback)));
        }

        [Test]
        public void ShutdownIsCalledOnStopStream()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            streamer.StopStream(123456);

            _mediaDeviceWrapperMock.Verify(m => m.Shutdown(It.Is<int>(i => i == 123456)));
        }

        [Test]
        public void SetFrameRateIsCalledOnSetHwRate()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            streamer.SetHwRate(123456);

            _mediaDeviceWrapperMock.Verify(m => m.SetFrameRate(It.Is<int>(i => i == 123456)));
        }

        [Test]
        public void NextFrameIsCalledOnNextWithPositiveIndex()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            streamer.Next(123456);

            _mediaDeviceWrapperMock.Verify(m => m.NextFrame(It.Is<int>(i => i == 123456)));
        }

        [Test]
        public void NextFrameIsNOTCalledOnNextWithNegativeIndex()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            streamer.Next(-123456);

            _mediaDeviceWrapperMock.Verify(m => m.NextFrame(It.IsAny<int>()), Times.Never);
        }

        [Test]
        public void NextFrameIsNOTCalledOnNextWithZeroIndex()
        {
            Streamer streamer = new Streamer(_mediaDeviceWrapperMock.Object, new IntPtr(12345));

            streamer.Next(0);

            _mediaDeviceWrapperMock.Verify(m => m.NextFrame(It.IsAny<int>()), Times.Never);
        }
    }
}
