set PATH_TO_MSTEST_RUNNER="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\"
set PATH_TO_TESTS=%cd%"\Test\bin\Release\AutomationTests.dll"
%PATH_TO_MSTEST_RUNNER%MSTest.exe /testcontainer:%PATH_TO_TESTS% 
