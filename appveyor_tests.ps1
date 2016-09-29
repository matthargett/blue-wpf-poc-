# TESTS SCRIPT

    #$env:CONFIGURATION="Release"

# UNIT TESTS
    Write-Host Running Unit tests -foregroundcolor "green"

    nunit3-console .\MediaStreamer\MediaStreamer.Tests\bin\$env:CONFIGURATION\MediaStreamer.Tests.dll

    if ($LastExitCode -ne 0) {
        $host.SetShouldExit($LastExitCode)
        throw "Unit tests failed"
    }

    # Get package versions
    [xml]$packagesXml = Get-Content .\MediaStreamer\MediaStreamer.Tests\packages.config
    $opencoverPath = ".\MediaStreamer\packages\OpenCover." + $packagesXml.SelectNodes("//packages/package[@id='OpenCover']").version + "\tools\OpenCover.Console.exe"
    $reportGeneratorPath = ".\MediaStreamer\packages\ReportGenerator." + $packagesXml.SelectNodes("//packages/package[@id='ReportGenerator']").version + "\tools\ReportGenerator.exe"

    # Run OpenCover
    & $opencoverPath -register:user -target:nunit3-console.exe "-targetargs:"".\MediaStreamer\MediaStreamer.Tests\bin\$env:CONFIGURATION\MediaStreamer.Tests.dll"""-filter:"+[MediaStreamer.UI*]*" -output:opencover.xml

    # Generate and process report on coverage
    & $reportGeneratorPath -reports:opencover.xml -targetdir:coverageReport -verbosity:Info -reporttypes:XmlSummary

    [xml]$report = Get-Content .\coverageReport\Summary.xml
    $lineCoverage = $report.CoverageReport.Summary.Coveredlines / $report.CoverageReport.Summary.Coverablelines * 100

    if ($lineCoverage -lt $env:LinecoverageThreshold) { 
        $host.SetShouldExit(1)
        throw "Line Coverage " +$lineCoverage+ " is less than needed "+ $env:LinecoverageThreshold
    }
    else {
        Write-Host Unit Tests Line Coverage: +$lineCoverage +"%" -foregroundcolor "green"
    }

# AUTOMATION TESTS

    Write-Host Running Automation tests -foregroundcolor "green"

    # Run OpenCover in background
    $opencoverJob = Start-Job -ScriptBlock {
        Set-Location $args[0]
        $opencoverPath = $args[1]
        & $opencoverPath -register:user -target:".\MediaStreamer\MediaStreamer.UI\bin\$env:CONFIGURATION\MediaStreamer.UI.exe"-filter:"+[MediaStreamer.UI*]*" -output:opencoverAutomation.xml  
    } -ArgumentList (Get-Item -Path ".\" -Verbose).FullName, $opencoverPath

    # Wait until application started
    $checkProcessStartedJob = Start-Job -ScriptBlock {
        $proc = $null
        do {
            Start-Sleep -Seconds 1
            $proc = Get-Process MediaStreamer.UI -ErrorAction SilentlyContinue
        }
        while (-Not $proc)
    }

    $checkProcessStartedJob | Wait-Job

    # Application started - Run Automation tests
    & vstest.console.exe ".\MediaStreamer\MediaStreamer.AutomationTests\bin\$env:CONFIGURATION\MediaStreamer.AutomationTests.dll"

    if ($LastExitCode -ne 0) {
        $tempExitCode = $LastExitCode
        $opencoverJob | Stop-Job
        $host.SetShouldExit($tempExitCode)
        throw "Automation tests failed"
    }

    # Wait until opencover job is completed
    $opencoverJob | Wait-Job

    # Generate and process report on coverage
    & $reportGeneratorPath -reports:opencoverAutomation.xml -targetdir:automationCoverageReport -verbosity:Info -reporttypes:XmlSummary

    [xml]$automationReport = Get-Content .\automationCoverageReport\Summary.xml
    $automationLineCoverage = $automationReport.CoverageReport.Summary.Coveredlines / $automationReport.CoverageReport.Summary.Coverablelines * 100

    if ($automationLineCoverage -lt $env:AutomationLinecoverageThreshold) { 
        $host.SetShouldExit(1)
        throw "Line Coverage " +$automationLineCoverage+ " is less than needed "+ $env:AutomationLinecoverageThreshold
    }
	else {
        Write-Host Automation Tests Line Coverage: $automationLineCoverage% -foregroundcolor "green"
    }