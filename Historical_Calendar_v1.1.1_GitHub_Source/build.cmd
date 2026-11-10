@echo off
setlocal
cd /d "%~dp0"
if "%SWOFTY_SDK_ROOT%"=="" (
    echo Set SWOFTY_SDK_ROOT to the SwoftyLauncher source checkout.
    exit /b 1
)
if not exist "%SWOFTY_SDK_ROOT%\packages\sdk\include\swofty_api.h" (
    echo The Swofty SDK headers were not found.
    exit /b 1
)
if not exist build mkdir build
cl /nologo /std:c++20 /EHsc /Zc:preprocessor /DNOMINMAX /DDEJW_PUBLIC_RELEASE /O2 /MT /LD /Iinclude /I"%SWOFTY_SDK_ROOT%\packages\sdk\include" /I"%SWOFTY_SDK_ROOT%\native\include" /Fobuild\calendar.obj /Febuild\dejw_public_calendar.dll src\calendar_swofty_2022.cpp /link /INCREMENTAL:NO /IMPLIB:build\calendar.lib
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /EHsc /Zc:preprocessor /DNOMINMAX /O2 /MT /I"%SWOFTY_SDK_ROOT%\packages\sdk\include" /Fobuild\optional-tracks.obj /Febuild\optional-tracks.exe src\optional_tracks.cpp /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
python package.py
exit /b %errorlevel%
