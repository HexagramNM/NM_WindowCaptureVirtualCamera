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
            internal static extern void setLeftMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void setRightMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void setTopMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void setBottomMargin(IntPtr captureWindowObj, int margin);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getLeftMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getRightMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getTopMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getBottomMargin(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern void setEnabledCapturePreview(IntPtr captureWindowObj, bool enabled);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getCapturePreviewWidth();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getCapturePreviewHeight();

            [DllImport("NM_WindowCapture.dll")]
            internal static extern bool isCapturing(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getCaptureWindowWidth(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern int getCaptureWindowHeight(IntPtr captureWindowObj);

            [DllImport("NM_WindowCapture.dll")]
            internal static extern float getCaptureFPS(IntPtr captureWindowObj);

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

        public static void setLeftMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setLeftMargin(captureWindowObj, margin);
        }

        public static void setRightMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setRightMargin(captureWindowObj, margin);
        }

        public static void setTopMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setTopMargin(captureWindowObj, margin);
        }

        public static void setBottomMargin(IntPtr captureWindowObj, int margin)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setBottomMargin(captureWindowObj, margin);
        }

        public static int getLeftMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getLeftMargin(captureWindowObj);
        }

        public static int getRightMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getRightMargin(captureWindowObj);
        }

        public static int getTopMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getTopMargin(captureWindowObj);
        }

        public static int getBottomMargin(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getBottomMargin(captureWindowObj);
        }

        public static void setEnabledCapturePreview(IntPtr captureWindowObj, bool enabled)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return;
            }
            NativeMethods.setEnabledCapturePreview(captureWindowObj, enabled);
        }

        public static int getCapturePreviewWidth()
        {
            return NativeMethods.getCapturePreviewWidth();
        }

        public static int getCapturePreviewHeight()
        {
            return NativeMethods.getCapturePreviewHeight();
        }

        public static bool isCapturing(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return false;
            }
            return NativeMethods.isCapturing(captureWindowObj);
        }

        public static int getCaptureWindowWidth(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getCaptureWindowWidth(captureWindowObj);
        }

        public static int getCaptureWindowHeight(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0;
            }
            return NativeMethods.getCaptureWindowHeight(captureWindowObj);
        }

        public static float getCaptureFPS(IntPtr captureWindowObj)
        {
            if (captureWindowObj == IntPtr.Zero)
            {
                return 0.0f;
            }
            return NativeMethods.getCaptureFPS(captureWindowObj);
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
