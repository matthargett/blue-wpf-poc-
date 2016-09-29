cd $env:APPLICATION_PATH
$VsTest = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"
$PATH_TO_TESTS = $env:APPLICATION_PATH + "\MediaStreamer.AutomationTests.dll"
Write-Host 'Start UI tests: ' $PATH_TO_TESTS
& $VsTest $PATH_TO_TESTS


If ($? -eq $false){
Write-Host 'Test failed!'
#failure! 
#need to find way to inform team
}
Else {
Write-Host 'Test succeeded!'
}