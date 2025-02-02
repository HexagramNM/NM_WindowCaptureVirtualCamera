using System;
using System.Runtime.InteropServices;

namespace NM_WindowCaptureVirtualCamera
{
    public static class NM_WindowCapture
    {
        private static class NativeMethods
        {
            [DllImport("NM_WindowCapture.dll")]
            internal static extern IntPtr CreateWindowCaptureObject(IntPtr baseHwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void CopyCapturePreviewToDXGIResource(IntPtr windowCaptureObj, IntPtr resourcePtr);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void OpenWindowPicker(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetTargetWindowForCapture(IntPtr windowCaptureObj, IntPtr hwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void StartVirtualCamera(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SwitchReverseCaptureWindow(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetLeftMargin(IntPtr windowCaptureObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetRightMargin(IntPtr windowCaptureObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetTopMargin(IntPtr windowCaptureObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetBottomMargin(IntPtr windowCaptureObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetLeftMargin(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetRightMargin(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetTopMargin(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetBottomMargin(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetEnabledCapturePreview(IntPtr windowCaptureObj, bool enabled);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCapturePreviewWidth();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCapturePreviewHeight();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern bool IsCapturing(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCaptureWindowWidth(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int GetCaptureWindowHeight(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern float GetCaptureFPS(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void SetCaptureCursor(IntPtr windowCaptureObj, bool isCaptured);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void StopVirtualCamera(IntPtr windowCaptureObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void DeleteWindowCaptureObject(IntPtr windowCaptureObj);
        }

        public static IntPtr CreateWindowCaptureObject(IntPtr baseHwnd)
        {
            return NativeMethods.CreateWindowCaptureObject(baseHwnd);
        }

        public static void CopyCapturePreviewToDXGIResource(IntPtr windowCaptureObj, IntPtr resourcePtr)
        {
            if (windowCaptureObj == IntPtr.Zero || resourcePtr == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.CopyCapturePreviewToDXGIResource(windowCaptureObj, resourcePtr);
        }

        public static void OpenWindowPicker(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.OpenWindowPicker(windowCaptureObj);
        }

        public static void SetTargetWindowForCapture(IntPtr windowCaptureObj, IntPtr hwnd)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetTargetWindowForCapture(windowCaptureObj, hwnd);
        }

        public static void StartVirtualCamera(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.StartVirtualCamera(windowCaptureObj);
        }

        public static void SwitchReverseCaptureWindow(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SwitchReverseCaptureWindow(windowCaptureObj);
        }

        public static void SetLeftMargin(IntPtr windowCaptureObj, int margin)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetLeftMargin(windowCaptureObj, margin);
        }

        public static void SetRightMargin(IntPtr windowCaptureObj, int margin)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetRightMargin(windowCaptureObj, margin);
        }

        public static void SetTopMargin(IntPtr windowCaptureObj, int margin)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetTopMargin(windowCaptureObj, margin);
        }

        public static void SetBottomMargin(IntPtr windowCaptureObj, int margin)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetBottomMargin(windowCaptureObj, margin);
        }

        public static int GetLeftMargin(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetLeftMargin(windowCaptureObj);
        }

        public static int GetRightMargin(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetRightMargin(windowCaptureObj);
        }

        public static int GetTopMargin(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetTopMargin(windowCaptureObj);
        }

        public static int GetBottomMargin(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetBottomMargin(windowCaptureObj);
        }

        public static void SetEnabledCapturePreview(IntPtr windowCaptureObj, bool enabled)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetEnabledCapturePreview(windowCaptureObj, enabled);
        }

        public static int GetCapturePreviewWidth()
        {
            return NativeMethods.GetCapturePreviewWidth();
        }

        public static int GetCapturePreviewHeight()
        {
            return NativeMethods.GetCapturePreviewHeight();
        }

        public static bool IsCapturing(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return false;
            }
            return NativeMethods.IsCapturing(windowCaptureObj);
        }

        public static int GetCaptureWindowWidth(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetCaptureWindowWidth(windowCaptureObj);
        }

        public static int GetCaptureWindowHeight(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.GetCaptureWindowHeight(windowCaptureObj);
        }

        public static float GetCaptureFPS(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return 0.0f;
            }
            return NativeMethods.GetCaptureFPS(windowCaptureObj);
        }

        public static void SetCaptureCursor(IntPtr windowCaptureObj, bool isCaptured)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.SetCaptureCursor(windowCaptureObj, isCaptured);
        }

        public static void StopVirtualCamera(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.StopVirtualCamera(windowCaptureObj);
        }

        public static void DeleteWindowCaptureObject(IntPtr windowCaptureObj)
        {
            if (windowCaptureObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.DeleteWindowCaptureObject(windowCaptureObj);
        }
    }
}
