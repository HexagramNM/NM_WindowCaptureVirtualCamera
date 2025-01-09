using Microsoft.Win32;
using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration.Install;
using System.Diagnostics;

namespace InstallerAction
{
    [RunInstaller(true)]
    public class CustomInstallAction: Installer
    {
        private const int win11MajorVersion = 10;
        private const int win11MinorVersion = 0;
        private const int win11BuildVersion = 22000;

        private void ExecuteCommand(string exeFile, string args, string cwd)
        {
            ProcessStartInfo psi = new ProcessStartInfo
            {
                FileName = exeFile,
                Arguments = args,
                WorkingDirectory = cwd,
                UseShellExecute = false,
                Verb = "runas"
            };

            try
            {
                using (Process process = Process.Start(psi))
                {
                    process.WaitForExit();
                }
            }
            catch (Exception ex)
            {
            }
            
        }

        public override void Install(IDictionary stateSaver)
        {
            base.Install(stateSaver);

            string targetDir = Context.Parameters["targetdir"];
            if (string.IsNullOrEmpty(targetDir))
            {
                return;
            }

            ExecuteCommand("regsvr32.exe", "/s NM_WCVCam_DS_64.dll", targetDir);
            ExecuteCommand("regsvr32.exe", "/s NM_WCVCam_DS_32.dll", targetDir);

            string versionKey = @"SOFTWARE\Microsoft\Windows NT\CurrentVersion";
            using (RegistryKey regKey = Registry.LocalMachine.OpenSubKey(versionKey))
            {
                if (regKey != null)
                {
                    int majorVersion = (int)regKey.GetValue("CurrentMajorVersionNumber", 0);
                    int minorVersion = (int)regKey.GetValue("CurrentMinorVersionNumber", 0);
                    int buildVersion = 0;
                    int.TryParse((string)regKey.GetValue("CurrentBuildNumber", "0"), out buildVersion);
                    if (majorVersion > win11MajorVersion || (majorVersion == win11MajorVersion &&
                        (minorVersion > win11MinorVersion || (minorVersion == win11MinorVersion && buildVersion >= win11BuildVersion))))
                    {
                        ExecuteCommand("regsvr32.exe", "/s NM_WCVCam_MF.dll", targetDir);
                        ExecuteCommand("icacls.exe", "NM_WCVCam_MF.dll /grant BUILTIN\\Users:RX", targetDir);
                    }
                }
            }
        }

        public override void Commit(IDictionary stateSaver)
        {
            base.Commit(stateSaver);
        }

        public override void Rollback(IDictionary stateSaver)
        {
            base.Rollback(stateSaver);
        }

        public override void Uninstall(IDictionary stateSaver)
        {
            base.Uninstall(stateSaver);

            string targetDir = Context.Parameters["targetdir"];
            if (string.IsNullOrEmpty(targetDir))
            {
                return;
            }

            ExecuteCommand("regsvr32.exe", "/s /u NM_WCVCam_DS_64.dll", targetDir);
            ExecuteCommand("regsvr32.exe", "/s /u NM_WCVCam_DS_32.dll", targetDir);

            string versionKey = @"SOFTWARE\Microsoft\Windows NT\CurrentVersion";
            using (RegistryKey regKey = Registry.LocalMachine.OpenSubKey(versionKey))
            {
                if (regKey != null)
                {
                    int majorVersion = (int)regKey.GetValue("CurrentMajorVersionNumber", 0);
                    int minorVersion = (int)regKey.GetValue("CurrentMinorVersionNumber", 0);
                    int buildVersion = 0;
                    int.TryParse((string)regKey.GetValue("CurrentBuildNumber", "0"), out buildVersion);
                    if (majorVersion > win11MajorVersion || (majorVersion == win11MajorVersion &&
                        (minorVersion > win11MinorVersion || (minorVersion == win11MinorVersion && buildVersion >= win11BuildVersion))))
                    {
                        ExecuteCommand("regsvr32.exe", "/s /u NM_WCVCam_MF.dll", targetDir);
                        ExecuteCommand("icacls.exe", "NM_WCVCam_MF.dll /remove:g BUILTIN\\Users", targetDir);
                    }
                }
            }
        }
    }
}
