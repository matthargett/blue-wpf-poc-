set PATH_TO_VSTEST_RUNNER="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"
set PATH_TO_TESTS=%cd%"\MediaStreamer.AutomationTests\bin\Release\MediaStreamer.AutomationTests.dll"
%PATH_TO_VSTEST_RUNNER% %PATH_TO_TESTS% 
