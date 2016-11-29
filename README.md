# WPF POC

[![Build status](https://ci.appveyor.com/api/projects/status/95j5iw9jjxrxw39x/branch/master?svg=true)](https://ci.appveyor.com/project/matthargett/blue-wpf-poc-ynbu5/branch/master)

## Project structure

- `.\MediaStreamer\MediaDevice` - native library that is responsible for media part (capturing web camera, streaming video)
- `.\MediaStreamer\MediaStreamer.UI` - WPF based GUI
- `.\MediaStreamer\MediaStreamer.Tests` - Unit tests (NUnit)
- `.\MediaStreamer\MediaStreamer.AutomationTests` - Automated UI tests
##
- `.\appveyor.yml` - AppVeyor Continuous Integration configuration file
- `.\appveyor_tests.ps1` - Unit tests and Automation UI script for AppVeyor CI
- `.\MediaStreamer\MediaStreamer.AutomationTests\deploy.ps1` - Deployment script wit send mail functionality for AppVeyor
- `.\MediaStreamer\RunTests.bat` - simple batch script for running Automation UI manually
- `.\MediaStreamer\VLC_Stream_CommandLine.txt` - sample command for configuring VLC for streaming
##
- `.\artifacts\h264_320x240_sample.zip` - sample video file for testing purposes

##
### Before first run

Should be properly configured _MediaStreamer.UI.exe.config_ file.

#### Config File
Config file contains a few parameters that should be set up correctly:

- `previewStream1FilePath`...`previewStream5FilePath` - path to file that will be used as stream in FilmStrip area;
- `CameraIndex` - zero based index of WEB camera that is used.
- `VideoRateFps` - desired stream update rate (FPS) multiplied to 1.5 The value is approximate.
- `CameraFrameFormat` - Frame resolution and FPS that will be set for WebCamera - if there is no desired format, web camera will be disabled.

```xml
<?xml version="1.0" encoding="utf-8"?>
<configuration>
    <configSections>
        <sectionGroup name="applicationSettings" type="System.Configuration.ApplicationSettingsGroup, System, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" >
            <section name="MediaStreamer.UI.Properties.Settings" type="System.Configuration.ClientSettingsSection, System, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" requirePermission="false" />
        </sectionGroup>
    </configSections>
    <startup>
        <supportedRuntime version="v4.0" sku=".NETFramework,Version=v4.5.1"/>
    </startup>
    <!-- Path to files for filmstrip area -->
  <appSettings>
    <add key="previewStream1FilePath" value="c:/temp/off_small.mp4"/>
    <add key="previewStream2FilePath" value="c:/temp/bj_1.mp4"/>
    <add key="previewStream3FilePath" value="c:/temp/bj_2.mp4"/>
    <add key="previewStream4FilePath" value="c:/temp/bj_3.mp4"/>
    <add key="previewStream5FilePath" value="c:/temp/bj_4.mp4"/>
  </appSettings>
  <applicationSettings>
    <MediaStreamer.UI.Properties.Settings>
      <setting name="CameraIndex" serializeAs="String">
        <!-- Index of Web camera -->
        <value>0</value>
      </setting>
      <setting name="VideoRateFps" serializeAs="String">
        <value>45</value>
      </setting>
       <setting name="CameraFrameFormat" serializeAs="String">
        <value>640x360@30</value>
      </setting>
    </MediaStreamer.UI.Properties.Settings>
  </applicationSettings>
</configuration>
```

## Autotests

### Before first run

- download **Agents for Microsoft Visual Studio 2015** from
[official site](https://www.microsoft.com/en-us/download/details.aspx?id=48152)
- install with default parameters.
- configure `MediaStreamer.AutomationTests.dll.config` file

**_Configuration parameters_** are following:

- `ApplicationPath` - path to application exe file _{default: C:\MediaStreamer\MediaStreamer.UI.exe }_
- `LogFolder` - path to log files _{default: C:\MediaStreamer\log }_
- `ClickCallButton` - setting for clicking call button in MainTest test in it start _{default: True}_
- `IterationQuantity` - the number of test iterations _{default: 1 }_
- `LaunchApp` - setting for lauching or not application _{default: True }_

### Run tests
`RunTest.bat` in root folder launches test execution.


## AppVeyor

### Build configuration

This application uses AppVeyor for CI, Unit Test runs and build Deployments.

All configuration is done inside appveyor.yml and should work correctly by default.

Here goes an explanation how to setup AppVeyor from scratch. **Prerequisites:** AppVeyor project for this git repo.

##### General tab
Enable AssemblyInfo patching with default options

##### Environment tab
Add variables:

- `CodeAnalysisTreatWarningsAsErrors`: `true` for running Roslyn Code Analysis after build. 
- `LinecoverageThreshold`: Fail build if Line coverage of Unit Tests is less than this number.
- `AutomationLinecoverageThreshold`: Fail build if Line coverage of Automated UI tests is less than this number.

##### Build tab
- Default configuration is `Release`
- Default platform is `x64`
- Set "Visual Studio solution or project file" to `MediaStreamer\MediaStreamer.sln`
- "Before build" script should restore nuget packages: `ps: nuget restore MediaStreamer\MediaStreamer.sln`

##### Tests tab
Running Unit and Automation UI Tests is done by executing powershell script: `.\appveyor_tests.ps1` which can be found in root directory of git repo.

##### Artifacts tab
To successfully deploy the project into an Environment and run there Automation UI tests some artifacts should be created:

- ```video_sample``` with path ```artifacts/h264_320x240_sample.zip```
- ```app``` with path ```MediaStreamer/MediaStreamer.UI/bin/$(CONFIGURATION)```
- ```automation_test``` with path ```MediaStreamer/MediaStreamer.AutomationTests/bin/$(CONFIGURATION)```

That's it


### Local Environment configuration
**Prerequisites:** Existing Environment in AppVeyor with `AppVeyor Agent` Provider

To be able run UI Tests the agent must be run as user with administrative privilege, not as a service. To do so, the service must be stoped and the agent should be ran as administrator manually.

_Windows 7_ Work on Windows 7 officially is not supported, but it works. Before to deploy Power Shell for Windows 7 should be updated to version 3.0 or above. [Details](https://social.technet.microsoft.com/wiki/contents/articles/21016.how-to-install-windows-powershell-4-0.aspx) 

##### General tab
- Use **Environment access key** to connect installed AppVeyor Agent

Add some **Provider settings**:

- `video_sample.deploy_app` = `true`
- `video_sample.path` = `%DEPLOY_ROOT_PATH%\video_sample`
- `group` = `WPF_POC`
- `app.deploy_app` = `true`
- `app.path` = `%DEPLOY_ROOT_PATH%`
- `automation_test.deploy_app` = `true`
- `automation_test.path` = `%DEPLOY_ROOT_PATH%\automation`
- `perf_counters.deploy_app` = `true`
- `perf_counters.path` = `%DEPLOY_ROOT_PATH%\pcm`
- `video_sample.deploy_order` = `1`
- `app.deploy_order` = `2`
- `perf_counters.deploy_order` = `3`
- `automation_test.deploy_order` = `10`

Add environment variable `DEPLOY_ROOT_PATH` = `c:\MediaStreamer` (Currently it's a default folder for Automation UI tests.
