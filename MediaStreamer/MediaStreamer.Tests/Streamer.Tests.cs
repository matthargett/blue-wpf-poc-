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
    }
}
