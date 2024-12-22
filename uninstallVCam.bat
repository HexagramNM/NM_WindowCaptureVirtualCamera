@echo off

cd %~dp0

for /f "usebackq delims=" %%A in (`ver`) do set WINDOWS_VER_RESULT=%%A
set WINDOWS_VER_RESULT=%WINDOWS_VER_RESULT:Microsoft Windows [Version =%
set WINDOWS_VERSION=%WINDOWS_VER_RESULT:]=%

regsvr32 bin\Release\NM_WCVCam_DS_64.dll /u
regsvr32 bin\Release\NM_WCVCam_DS_32.dll /u

if %WINDOWS_VERSION% geq 10.0.22000.0 (
    regsvr32 bin\Release\NM_WCVCam_MF.dll /u
    icacls bin\Release\NM_WCVCam_MF.dll /remove:g BUILTIN\Users
)
