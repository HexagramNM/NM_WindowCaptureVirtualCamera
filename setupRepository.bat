@echo off

setlocal enabledelayedexpansion

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


rem Update the solution for DirectShow library.
set BASECLASSES_SLN=External\windows-classic-samples\Samples\Win7Samples\multimedia\directshow\baseclasses\baseclasses.sln

set BASECLASSES_PROJECT=External\windows-classic-samples\Samples\Win7Samples\multimedia\directshow\baseclasses\BaseClasses.vcxproj

if not exist %BASECLASSES_PROJECT% (
  %DEVENVCOM% %BASECLASSES_SLN% /Upgrade 
)


rem Create a modified vcxproj to use setting /MT or /MTd
set BASECLASSES_PROJECT_MT=External\windows-classic-samples\Samples\Win7Samples\multimedia\directshow\baseclasses\BaseClasses_MT.vcxproj

if not exist %BASECLASSES_PROJECT_MT% (
  type nul > %BASECLASSES_PROJECT_MT%

  for /f "usebackq delims=" %%p in (`type %BASECLASSES_PROJECT%`) do (
    set LINE=%%p

    rem change from MultiThreadedDebugDLL to MultiThreadedDebug
    set MOD_LINE1=!LINE:^<RuntimeLibrary^>MultiThreadedDebugDLL^</RuntimeLibrary^>=^<RuntimeLibrary^>MultiThreadedDebug^</RuntimeLibrary^>!

    rem change from MultiThreadedDLL to MultiThreaded
    set MOD_LINE2=!MOD_LINE1:^<RuntimeLibrary^>MultiThreadedDLL^</RuntimeLibrary^>=^<RuntimeLibrary^>MultiThreaded^</RuntimeLibrary^>!

    echo !MOD_LINE2! >> %BASECLASSES_PROJECT_MT%
  )
)


rem Build the modified project
for /f "usebackq delims=" %%b in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set MSBUILD="%%b"

%MSBUILD% %BASECLASSES_PROJECT_MT% /p:Configuration=Debug /p:Platform=Win32 /t:Rebuild 

%MSBUILD% %BASECLASSES_PROJECT_MT% /p:Configuration=Debug /p:Platform=x64 /t:Rebuild

%MSBUILD% %BASECLASSES_PROJECT_MT% /p:Configuration=Release /p:Platform=Win32 /t:Rebuild

%MSBUILD% %BASECLASSES_PROJECT_MT% /p:Configuration=Release /p:Platform=x64 /t:Rebuild
