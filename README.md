# WPF POC

## Project structure

- _MediaDevice_ library that is responsible for media part (capturing web camera, streaming video)
- _MediaStreamer.UI_ WPF based GUI
- _Tests_ Automation tests


### Before first run

Should be properly configured _MediaStreamer.UI.exe.config_ file.

#### Config File
Config file contains a few parameters that should be set up correctly:
- **previewStream1FilePath** - **previewStream5FilePath** - path to file that will be used as stream in FilmStrip area;
- **CameraIndex** - zero based index of WEB camera that is used.
- **VideoRateFps** - desired stream update rate (FPS) multiplied to 1.5 The value is approximate.

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
    </MediaStreamer.UI.Properties.Settings>
  </applicationSettings>
</configuration>
```

## Autotests

All measurements are taken with help of PerformanceCounter class from System.Diagnostics and System.Runtime.InteropServices libraries (for GDI measurements). All data are collected for “MediaStreamer.UI.exe” process.

Custom methods that are using in tests could be found in `Mediastreamer/Tests/ResourceCounter.cs`

### Before first run

- download **Agents for Microsoft Visual Studio 2015** from
[official site](https://www.microsoft.com/en-us/download/details.aspx?id=48152)
- install with default parameters.

### How to run

For running custom test, you have to edit bat file and add test name :
_/test:[testname]_

testname: |duration, h
:--|:-:
CrazyWindowTest|2
launchDropApp|1
startStopVideo|2

All test setting could be found in _\Tests\Settings1.settings_ (all this setting are static, and copping to app.config, after its change you have to rebuild project):<br />
* **ApplicationPath** - path to application exe file **{default: C:\MediaStreamer\MediaStreamer.UI.exe }**
* **LaunchApp** - setting for lauching or not application **{default: True}**
* **ClickCallButton** - setting for clicking call button in CrazyWindowTest test in it start **{default: True}**
* **LogFolder**- path to folder where all logs will be placed. **{default: C:\MediaStreamer\log }**
