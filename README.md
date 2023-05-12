# foobar2000 plugins

## Ducking control
Controls response to system ducking (volume reduce) events  
Opt in by default  
To opt out, enable  
**Preferences - Advanced - Tools - Ducking control - Opt out**

## TrackLog
Logs streams dynamic info to console and file  
Settings:  
**Preferences - Advanced - Tools - TrackLog**

## Source build instructions
1. Download and unpack [foobar2000 SDK](https://www.foobar2000.org/SDK) from the official site.  
2. Place content of this repository in the foobar2000 directory of the SDK root.  
4. Open foo_SDK.sln in Visual Studio, select desired configuration and platform.
Adjust Platform Toolset to suit your Visual Studio version.  
5. Build the solution.  

Optionally, the solution can be built using Visual Studio Build Tools by running **build.cmd** script from the
Developer Command Prompt. It builds for both x64 and x86 platforms.

If your build fails due to absent **atlapp.h** file, download and unpack
[WTL library](https://sourceforge.net/projects/wtl/files/WTL%2010/) to the new **WTL** directory inside
the SDK root directory. Add path to **WTL/Include** directory to the C/C++ Additional Include Directories
for both **foobar2000_sdk_helpers** and **libPPUI** projects.
