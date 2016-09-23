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
    }
}
