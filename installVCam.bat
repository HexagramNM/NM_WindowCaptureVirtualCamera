@echo off

cd %~dp0

for /f "usebackq delims=" %%A in (`ver`) do set WINDOWS_VER_RESULT=%%A
set WINDOWS_VER_RESULT=%WINDOWS_VER_RESULT:Microsoft Windows [Version =%
set WINDOWS_VERSION=%WINDOWS_VER_RESULT:]=%

regsvr32 /s bin\Release\NM_WCVCam_DS_64.dll
regsvr32 /s bin\Release\NM_WCVCam_DS_32.dll

if %WINDOWS_VERSION% geq 10.0.22000.0 (
    regsvr32 /s bin\Release\NM_WCVCam_MF.dll
    icacls bin\Release\NM_WCVCam_MF.dll /grant BUILTIN\Users:RX
)
