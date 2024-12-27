using System;
using System.Runtime.InteropServices;

namespace NM_WindowCaptureVirtualCamera
{
    public static class NM_WindowCapture
    {
        private static class NativeMethods
        {
            [DllImport("NM_WindowCapture.dll")]
            internal static extern void startupForMF();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern IntPtr createWindowCaptureObject(IntPtr baseHwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void copyCapturePreviewToDXGIResource(IntPtr captureWindowObj, IntPtr resourcePtr);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void openWindowPicker(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void setTargetWindowForCapture(IntPtr captureWindowObj, IntPtr hwnd);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void startVirtualCamera(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void switchReverseCaptureWindow(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void stopVirtualCamera(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void deleteWindowCaptureObject(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void shutdownForMF();
        }

        public static void startupForMF()
        {
            NativeMethods.startupForMF();
        }

        public static IntPtr createWindowCaptureObject(IntPtr baseHwnd)
        {
            return NativeMethods.createWindowCaptureObject(baseHwnd);
        }

        public static void copyCapturePreviewToDXGIResource(IntPtr captureWindowObj, IntPtr resourcePtr)
        {
            if (captureWindowObj == IntPtr.Zero || resourcePtr == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.copyCapturePreviewToDXGIResource(captureWindowObj, resourcePtr);
        }

        public static void openWindowPicker(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.openWindowPicker(captureWindowObj);
        }

        public static void setTargetWindowForCapture(IntPtr captureWindowObj, IntPtr hwnd)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setTargetWindowForCapture(captureWindowObj, hwnd);
        }

        public static void startVirtualCamera(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.startVirtualCamera(captureWindowObj);
        }

        public static void switchReverseCaptureWindow(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.switchReverseCaptureWindow(captureWindowObj);
        }

        public static void stopVirtualCamera(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.stopVirtualCamera(captureWindowObj);
        }

        public static void deleteWindowCaptureObject(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.deleteWindowCaptureObject(captureWindowObj);
        }

        public static void shutdownForMF()
        {
            NativeMethods.shutdownForMF();
        }
    }
}
