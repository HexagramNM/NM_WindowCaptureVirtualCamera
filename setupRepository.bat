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


for /f "usebackq delims=" %%b in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set MSBUILD="%%b"

%MSBUILD% %BASECLASSES_SLN% /p:ForceImportBeforeCppTargets="%~dp0customForMTd.prop" /p:Configuration=Debug /p:Platform=Win32 /t:Rebuild

%MSBUILD% %BASECLASSES_SLN% /p:ForceImportBeforeCppTargets="%~dp0customForMTd.prop" /p:Configuration=Debug /p:Platform=x64 /t:Rebuild

%MSBUILD% %BASECLASSES_SLN% /p:ForceImportBeforeCppTargets="%~dp0customForMT.prop" /p:Configuration=Release /p:Platform=Win32 /t:Rebuild

%MSBUILD% %BASECLASSES_SLN% /p:ForceImportBeforeCppTargets="%~dp0customForMT.prop" /p:Configuration=Release /p:Platform=x64 /t:Rebuild
