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

%~d0 || goto exit
cd %~dp0 || goto exit

msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x64" /t:Clean
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x64"
set x64status=%ERRORLEVEL%
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x86" /t:Clean
msbuild /p:PlatformToolset=!PlatformToolset! /p:"Configuration=Release" /p:"Platform=x86"
set x86status=%ERRORLEVEL%

if not %x64status% == 0 (
  echo.
  echo.*** x64 build FAILED ***
  echo.
)

if not %x86status% == 0 (
  echo.
  echo.*** x86 build FAILED ***
  echo.
)

if not %x64status% == 0 goto cleanup
if not %x86status% == 0 goto cleanup

if exist bin\ (

  cd %~dp0bin\Release || goto releasefail

  if exist foo_ducking.fb2k-component del foo_ducking.fb2k-component
  if exist foo_tracklog.fb2k-component del foo_tracklog.fb2k-component

  cd %~dp0bin\Release\Win32 || goto releasefail
  7z a -bso0 -tzip -mx9 -sae %~dp0bin\Release\foo_ducking.fb2k-component foo_ducking.dll || (
    del %~dp0bin\Release\foo_ducking.fb2k-component 2>nul
    goto releasefail
  )
  7z a -bso0 -tzip -mx9 -sae %~dp0bin\Release\foo_tracklog.fb2k-component foo_tracklog.dll || (
    del %~dp0bin\Release\foo_tracklog.fb2k-component 2>nul
    goto releasefail
  )
  cd %~dp0bin\Release || goto releasefail
  7z a -bso0 -tzip -mx9 -sae foo_ducking.fb2k-component x64\foo_ducking.dll || (
    del foo_ducking.fb2k-component 2>nul
    goto releasefail
  )
  7z a -bso0 -tzip -mx9 -sae foo_tracklog.fb2k-component x64\foo_tracklog.dll || (
    del foo_tracklog.fb2k-component 2>nul
    goto releasefail
  )

  echo.
  echo.=======================================================
  echo.=                                                     =
  echo.=   bin\Release\foo_ducking.fb2k-component created    =
  echo.=   bin\Release\foo_tracklog.fb2k-component created   =
  echo.=                                                     =
  echo.=======================================================

  rd /s /q %~dp0bin\Release\x64 2>nul
  rd /s /q %~dp0bin\Release\Win32 2>nul

  goto releasedone

:releasefail
  echo,
  echo.*** FAILURE releasing files ***
  echo.

:releasedone
  cd %~dp0 || goto exit

)

if exist %~dp0build rd /s /q %~dp0build 2>nul
if exist %~dp0foobar2000_component_client\Release rd /s /q %~dp0foobar2000_component_client\Release 2>nul
goto exit

:nodevenv
echo.Start this script from the Developer Command Prompt

:exit
