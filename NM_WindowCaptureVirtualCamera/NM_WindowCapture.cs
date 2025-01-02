using System;
using System.Runtime.InteropServices;

namespace NM_WindowCaptureVirtualCamera
{
    public static class NM_WindowCapture
    {
        private static class NativeMethods
        {
            [DllImport("NM_WindowCapture.dll")]
            internal static extern void StartupForMF();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern IntPtr CreateWindowCaptureObject(IntPtr baseHwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void CopyCapturePreviewToDXGIResource(IntPtr captureWindowObj, IntPtr resourcePtr);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void OpenWindowPicker(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetTargetWindowForCapture(IntPtr captureWindowObj, IntPtr hwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void StartVirtualCamera(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SwitchReverseCaptureWindow(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetLeftMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetRightMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetTopMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetBottomMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetLeftMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetRightMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetTopMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetBottomMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetEnabledCapturePreview(IntPtr captureWindowObj, bool enabled);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCapturePreviewWidth();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCapturePreviewHeight();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern bool IsCapturing(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCaptureWindowWidth(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCaptureWindowHeight(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern float GetCaptureFPS(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void StopVirtualCamera(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void DeleteWindowCaptureObject(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void ShutdownForMF();
        }

        public static void StartupForMF()
        {
            NativeMethods.StartupForMF();
        }

        public static IntPtr CreateWindowCaptureObject(IntPtr baseHwnd)
        {
            return NativeMethods.CreateWindowCaptureObject(baseHwnd);
        }

        public static void CopyCapturePreviewToDXGIResource(IntPtr captureWindowObj, IntPtr resourcePtr)
        {
            if (captureWindowObj == IntPtr.Zero || resourcePtr == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.CopyCapturePreviewToDXGIResource(captureWindowObj, resourcePtr);
        }

        public static void OpenWindowPicker(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.OpenWindowPicker(captureWindowObj);
        }

        public static void SetTargetWindowForCapture(IntPtr captureWindowObj, IntPtr hwnd)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetTargetWindowForCapture(captureWindowObj, hwnd);
        }

        public static void StartVirtualCamera(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.StartVirtualCamera(captureWindowObj);
        }

        public static void SwitchReverseCaptureWindow(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SwitchReverseCaptureWindow(captureWindowObj);
        }

        public static void SetLeftMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetLeftMargin(captureWindowObj, margin);
        }

        public static void SetRightMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetRightMargin(captureWindowObj, margin);
        }

        public static void SetTopMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetTopMargin(captureWindowObj, margin);
        }

        public static void SetBottomMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetBottomMargin(captureWindowObj, margin);
        }

        public static int GetLeftMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetLeftMargin(captureWindowObj);
        }

        public static int GetRightMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetRightMargin(captureWindowObj);
        }

        public static int GetTopMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetTopMargin(captureWindowObj);
        }

        public static int GetBottomMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetBottomMargin(captureWindowObj);
        }

        public static void SetEnabledCapturePreview(IntPtr captureWindowObj, bool enabled)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetEnabledCapturePreview(captureWindowObj, enabled);
        }

        public static int GetCapturePreviewWidth()
        {
            return NativeMethods.GetCapturePreviewWidth();
        }

        public static int GetCapturePreviewHeight()
        {
            return NativeMethods.GetCapturePreviewHeight();
        }

        public static bool IsCapturing(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return false;
            }
            return NativeMethods.IsCapturing(captureWindowObj);
        }

        public static int GetCaptureWindowWidth(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetCaptureWindowWidth(captureWindowObj);
        }

        public static int GetCaptureWindowHeight(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetCaptureWindowHeight(captureWindowObj);
        }

        public static float GetCaptureFPS(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0.0f;
            }
            return NativeMethods.GetCaptureFPS(captureWindowObj);
        }

        public static void StopVirtualCamera(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.StopVirtualCamera(captureWindowObj);
        }

        public static void DeleteWindowCaptureObject(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.DeleteWindowCaptureObject(captureWindowObj);
        }

        public static void ShutdownForMF()
        {
            NativeMethods.ShutdownForMF();
        }
    }
}
