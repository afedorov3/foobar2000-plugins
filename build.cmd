@echo off

SETLOCAL EnableDelayedExpansion

if x%VCToolsVersion% == x goto nodevenv
for /f "delims=. TOKENS=1,2" %%i in ("%VCToolsVersion%") do (
set VCTVmaj=%%i
set VCTVmin=%%j
set PlatformToolset=v!VCTVmaj!!VCTVmin:~0,1!
set VCTVmaj=
set VCTVmin=
)

%~d0
cd %~dp0

msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x64" /t:Clean
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x64"
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x86" /t:Clean
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x86"

goto exit

:nodevenv
echo.Start this script from the Developer Command Prompt

:exit
