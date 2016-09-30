# WPF POC

## Project structure

- _MediaDevice_ library that is responsible for media part (capturing web camera, streaming video)
- _MediaStreamer.UI_ WPF based GUI
- _MediaStreamer.AutomationTests_ Automation tests


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

### Before first run

- download **Agents for Microsoft Visual Studio 2015** from
[official site](https://www.microsoft.com/en-us/download/details.aspx?id=48152)
- install with default parameters.
- configure `MediaStreamer.AutomationTests.dll.config` file

**_Configuration parameters_** are following:
- **ApplicationPath** - path to application exe file _{default: C:\MediaStreamer\MediaStreamer.UI.exe }_
- **LogFolder** - path to log files _{default: C:\MediaStreamer\log }_
- **ClickCallButton** - setting for clicking call button in MainTest test in it start _{default: True}_
- **IterationQuantity** - the number of test iterations _{default: 1 }_
- **LaunchApp** - setting for lauching or not application _{default: True }_

### Run tests
`RunTest.bat` in root folder launches test execution.



