@echo off

rem Setting on submodules.
cd External\windows-classic-samples

git config core.sparsecheckout true

cd ..\..

echo Samples/Win7Samples/multimedia/directshow/baseclasses > .git\modules\External\windows-classic-samples\info\sparse-checkout

git -C External/windows-classic-samples read-tree -mu HEAD


rem Create directories to store exe and dll.
mkdir bin
mkdir bin\Release
mkdir bin\Debug


rem Get the path to devenv.exe
set VSWHERE="C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"

for /f "usebackq delims=" %%a in (`%VSWHERE% ^| findstr productPath`) do set VSWHERE_RESULT=%%a

set DEVENV="%VSWHERE_RESULT:productPath: =%"
set DEVENVCOM=%DEVENV:.exe=.com%


rem Update the solution for DirectShow library and build it.
set BASECLASSES_SLN=External\windows-classic-samples\Samples\Win7Samples\multimedia\directshow\baseclasses\baseclasses.sln

%DEVENVCOM% %BASECLASSES_SLN% /Upgrade 

%DEVENVCOM% %BASECLASSES_SLN% /Rebuild "Debug^|Win32"
%DEVENVCOM% %BASECLASSES_SLN% /Rebuild "Release^|Win32"
%DEVENVCOM% %BASECLASSES_SLN% /Rebuild "Debug^|x64"
%DEVENVCOM% %BASECLASSES_SLN% /Rebuild "Release^|x64"

