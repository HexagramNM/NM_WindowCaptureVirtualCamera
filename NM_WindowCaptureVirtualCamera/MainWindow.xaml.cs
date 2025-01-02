using Microsoft.Wpf.Interop.DirectX;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace NM_WindowCaptureVirtualCamera
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public IntPtr captureObj = IntPtr.Zero;
        public D3D11Image windowPreview;
        private TimeSpan lastRender;
        private bool enableVCam = false;

        private const double MOUSE_TOLERANCE = 5.0;
        private Point previousMousePos;
        private bool leftMarginMoveFlag = false;
        private bool rightMarginMoveFlag = false;
        private bool topMarginMoveFlag = false;
        private bool bottomMarginMoveFlag = false;
        private bool boxMoveFlag = false;
        private double previousLeftMargin = 0.0;
        private double previousRightMargin = 0.0;
        private double previousTopMargin = 0.0;
        private double previousBottomMargin = 0.0;

        public MainWindow()
        {
            InitializeComponent();

            windowPreview = new D3D11Image();
            int previewWidth = NM_WindowCapture.GetCapturePreviewWidth();
            int previewHeight = NM_WindowCapture.GetCapturePreviewHeight();
            windowPreview.SetPixelSize(previewWidth, previewHeight);
            image_windowPreview.Source = windowPreview;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var helper = new System.Windows.Interop.WindowInteropHelper(this);
            captureObj = NM_WindowCapture.CreateWindowCaptureObject(helper.Handle);
            NM_WindowCapture.SetEnabledCapturePreview(captureObj, true);
            windowPreview.WindowOwner = helper.Handle;
            windowPreview.OnRender += PreviewWindow_OnRender;
            CompositionTarget.Rendering += CompositionTarget_Render;
            MouseMove += Canvas_windowPreview_MouseMove;
            MouseLeftButtonUp += Canvas_windowPreview_MouseLeftButtonUp;
        }

        private void DrawRectangleRange()
        {
            bool isCapturing = NM_WindowCapture.IsCapturing(captureObj);
            Visibility visibleStatus = Visibility.Visible;
            if (isCapturing)
            {
                double previewWidth = canvas_windowPreview.Width;
                double previewHeight = canvas_windowPreview.Height;
                double rate = previewWidth / previewHeight;

                double captureWindowWidth = NM_WindowCapture.GetCaptureWindowWidth(captureObj);
                double captureWindowHeight = NM_WindowCapture.GetCaptureWindowHeight(captureObj);

                double leftMargin = NM_WindowCapture.GetLeftMargin(captureObj);
                double rightMargin = NM_WindowCapture.GetRightMargin(captureObj);
                double topMargin = NM_WindowCapture.GetTopMargin(captureObj);
                double bottomMargin = NM_WindowCapture.GetBottomMargin(captureObj);

                if (captureWindowHeight * rate > captureWindowWidth)
                {
                    double scale = previewHeight / captureWindowHeight;
                    double widthSpace = previewWidth - captureWindowWidth * scale;
                    rectangle_leftMargin.Width = leftMargin * scale + widthSpace * 0.5;
                    rectangle_rightMargin.Width = rightMargin * scale + widthSpace * 0.5;
                    rectangle_topMargin.Height = topMargin * scale;
                    rectangle_bottomMargin.Height = bottomMargin * scale;

                    rectangle_range.SetValue(Canvas.LeftProperty, leftMargin * scale + widthSpace * 0.5);
                    rectangle_range.SetValue(Canvas.TopProperty, topMargin * scale);
                    rectangle_range.Width = previewWidth - ((leftMargin + rightMargin) * scale + widthSpace);
                    rectangle_range.Height = previewHeight - (topMargin + bottomMargin) * scale;
                }
                else
                {
                    double scale = previewWidth / captureWindowWidth;
                    double heightSpace = previewHeight - captureWindowHeight * scale;
                    rectangle_leftMargin.Width = leftMargin * scale;
                    rectangle_rightMargin.Width = rightMargin * scale;
                    rectangle_topMargin.Height = topMargin * scale + heightSpace * 0.5;
                    rectangle_bottomMargin.Height = bottomMargin * scale + heightSpace * 0.5;

                    rectangle_range.SetValue(Canvas.LeftProperty, leftMargin * scale);
                    rectangle_range.SetValue(Canvas.TopProperty, topMargin * scale + heightSpace * 0.5);
                    rectangle_range.Width = previewWidth - (leftMargin + rightMargin) * scale;
                    rectangle_range.Height = previewHeight - ((topMargin + bottomMargin) * scale + heightSpace);
                }
            }
            else 
            {
                visibleStatus = Visibility.Hidden;
            }

            rectangle_range.Visibility = visibleStatus;
            rectangle_exclude.Visibility = visibleStatus;
        }

        private void Button_picker_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.OpenWindowPicker(captureObj);
        }

        private void Button_cameraStart_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.StartVirtualCamera(captureObj);
            enableVCam = true;
        }

        private void Button_cameraReverse_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.SwitchReverseCaptureWindow(captureObj);
        }

        private void Button_cameraStop_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.StopVirtualCamera(captureObj);
            enableVCam = false;
        }

        private void Button_trimmingReset_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.SetLeftMargin(captureObj, 0);
            NM_WindowCapture.SetRightMargin(captureObj, 0);
            NM_WindowCapture.SetTopMargin(captureObj, 0);
            NM_WindowCapture.SetBottomMargin(captureObj, 0);
        }

        private void Checkbox_captureCursor_Checked(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.SetCaptureCursor(captureObj, true);
        }

        private void Checkbox_captureCursor_Unchecked(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.SetCaptureCursor(captureObj, false);
        }

        private void PreviewWindow_OnRender(nint resourcePtr, bool isNewSurface)
        {
            NM_WindowCapture.CopyCapturePreviewToDXGIResource(captureObj, resourcePtr);
        }

        private Point ConvertPositionToWindowCoordinate(Point pos)
        {
            bool isCapturing = NM_WindowCapture.IsCapturing(captureObj);
            if (!isCapturing)
            {
                return new Point(0.0, 0.0);
            }

            Point result = new Point();
            double previewWidth = canvas_windowPreview.Width;
            double previewHeight = canvas_windowPreview.Height;
            double rate = previewWidth / previewHeight;

            double captureWindowWidth = NM_WindowCapture.GetCaptureWindowWidth(captureObj);
            double captureWindowHeight = NM_WindowCapture.GetCaptureWindowHeight(captureObj);

            double leftMargin = NM_WindowCapture.GetLeftMargin(captureObj);
            double rightMargin = NM_WindowCapture.GetRightMargin(captureObj);
            double topMargin = NM_WindowCapture.GetTopMargin(captureObj);
            double bottomMargin = NM_WindowCapture.GetBottomMargin(captureObj);

            if (captureWindowHeight * rate > captureWindowWidth)
            {
                double scale = captureWindowHeight / previewHeight;
                double widthSpace = previewWidth * scale - captureWindowWidth;
                result.X = pos.X * scale - widthSpace * 0.5;
                result.Y = pos.Y * scale;
            }
            else
            {
                double scale = captureWindowWidth / previewWidth;
                double heightSpace = previewHeight * scale - captureWindowHeight;
                result.X = pos.X * scale;
                result.Y = pos.Y * scale - heightSpace * 0.5;
            }
            return result;
        }

        private void GetRangeBoxFlagFromMousePos(Point mousePosInCaptureWindow, 
            out bool leftFlag, out bool rightFlag, out bool topFlag, out bool bottomFlag, out bool boxFlag)
        {
            leftFlag = false;
            rightFlag = false;
            topFlag = false;
            bottomFlag = false;
            boxFlag = false;

            double currentLeftMargin = NM_WindowCapture.GetLeftMargin(captureObj);
            double currentRightMargin = NM_WindowCapture.GetRightMargin(captureObj);
            double currentTopMargin = NM_WindowCapture.GetTopMargin(captureObj);
            double currentBottomMargin = NM_WindowCapture.GetBottomMargin(captureObj);
            double captureWindowWidth = NM_WindowCapture.GetCaptureWindowWidth(captureObj);
            double captureWindowHeight = NM_WindowCapture.GetCaptureWindowHeight(captureObj);

            double previewWidth = canvas_windowPreview.Width;
            double previewHeight = canvas_windowPreview.Height;
            double rate = previewWidth / previewHeight;
            double mouseToleranceInCaptureWindow = MOUSE_TOLERANCE;

            if (captureWindowHeight * rate > captureWindowWidth)
            {
                mouseToleranceInCaptureWindow = MOUSE_TOLERANCE * captureWindowHeight / previewHeight;
            }
            else
            {
                mouseToleranceInCaptureWindow = MOUSE_TOLERANCE * captureWindowWidth / previewWidth;
            }

            if (mousePosInCaptureWindow.Y > currentTopMargin && mousePosInCaptureWindow.Y < captureWindowHeight - currentBottomMargin)
            {
                if (Math.Abs(currentLeftMargin - mousePosInCaptureWindow.X) <= mouseToleranceInCaptureWindow)
                {
                    leftFlag = true;
                }
                else if (Math.Abs((captureWindowWidth - currentRightMargin) - mousePosInCaptureWindow.X) <= mouseToleranceInCaptureWindow)
                {
                    rightFlag = true;
                }
            }

            if (mousePosInCaptureWindow.X > currentLeftMargin && mousePosInCaptureWindow.X < captureWindowWidth - currentRightMargin)
            {
                if (Math.Abs(currentTopMargin - mousePosInCaptureWindow.Y) <= mouseToleranceInCaptureWindow)
                {
                    topFlag = true;
                }
                else if (Math.Abs((captureWindowHeight - currentBottomMargin) - mousePosInCaptureWindow.Y) <= mouseToleranceInCaptureWindow)
                {
                    bottomFlag = true;
                }
            }

            if (!leftFlag && !rightFlag && !topFlag && !bottomFlag
                && mousePosInCaptureWindow.X > currentLeftMargin && mousePosInCaptureWindow.X < captureWindowWidth - currentRightMargin
                && mousePosInCaptureWindow.Y > currentTopMargin && mousePosInCaptureWindow.Y < captureWindowHeight - currentBottomMargin)
            {
                boxFlag = true;
            }
        }

        private void Canvas_windowPreview_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Point mousePosInCaptureWindow = ConvertPositionToWindowCoordinate(e.GetPosition(canvas_windowPreview));

            previousLeftMargin = NM_WindowCapture.GetLeftMargin(captureObj);
            previousRightMargin = NM_WindowCapture.GetRightMargin(captureObj);
            previousTopMargin = NM_WindowCapture.GetTopMargin(captureObj);
            previousBottomMargin = NM_WindowCapture.GetBottomMargin(captureObj);

            GetRangeBoxFlagFromMousePos(mousePosInCaptureWindow, out leftMarginMoveFlag, out rightMarginMoveFlag,
                out topMarginMoveFlag, out bottomMarginMoveFlag, out boxMoveFlag);

            previousMousePos = mousePosInCaptureWindow;
            Mouse.Capture(this);
        }

        private void ChangeRangeBox(Point mousePosInCaptureWindow)
        {
            if (leftMarginMoveFlag)
            {
                NM_WindowCapture.SetLeftMargin(captureObj, (int)(previousLeftMargin + (mousePosInCaptureWindow.X - previousMousePos.X)));
            }

            if (rightMarginMoveFlag)
            {
                NM_WindowCapture.SetRightMargin(captureObj, (int)(previousRightMargin - (mousePosInCaptureWindow.X - previousMousePos.X)));
            }

            if (topMarginMoveFlag)
            {
                NM_WindowCapture.SetTopMargin(captureObj, (int)(previousTopMargin + (mousePosInCaptureWindow.Y - previousMousePos.Y)));
            }

            if (bottomMarginMoveFlag)
            {
                NM_WindowCapture.SetBottomMargin(captureObj, (int)(previousBottomMargin - (mousePosInCaptureWindow.Y - previousMousePos.Y)));
            }

            if (boxMoveFlag)
            {
                double moveX = mousePosInCaptureWindow.X - previousMousePos.X;
                if (moveX < -previousLeftMargin)
                {
                    moveX = -previousLeftMargin;
                }
                else if (moveX > previousRightMargin)
                {
                    moveX = previousRightMargin;
                }

                NM_WindowCapture.SetLeftMargin(captureObj, (int)(previousLeftMargin + moveX));
                NM_WindowCapture.SetRightMargin(captureObj, (int)(previousRightMargin - moveX));

                double moveY = mousePosInCaptureWindow.Y - previousMousePos.Y;
                if (moveY < -previousTopMargin)
                {
                    moveY = -previousTopMargin;
                }
                else if (moveY > previousBottomMargin)
                {
                    moveY = previousBottomMargin;
                }

                NM_WindowCapture.SetTopMargin(captureObj, (int)(previousTopMargin + moveY));
                NM_WindowCapture.SetBottomMargin(captureObj, (int)(previousBottomMargin - moveY));
            }
        }

        private void ChangeCursorForRangeBox(Point mousePosInCaptureWindow)
        {
            bool isCapturing = NM_WindowCapture.IsCapturing(captureObj);
            if (!isCapturing)
            {
                Cursor = Cursors.Arrow;
                return;
            }

            bool leftFlag = false;
            bool rightFlag = false;
            bool topFlag = false;
            bool bottomFlag = false;
            bool boxFlag = false;

            if (leftMarginMoveFlag || rightMarginMoveFlag || topMarginMoveFlag || bottomMarginMoveFlag || boxMoveFlag)
            {
                leftFlag = leftMarginMoveFlag;
                rightFlag = rightMarginMoveFlag;
                topFlag = topMarginMoveFlag;
                bottomFlag = bottomMarginMoveFlag;
                boxFlag = boxMoveFlag;
            }
            else
            {
                GetRangeBoxFlagFromMousePos(mousePosInCaptureWindow, out leftFlag, out rightFlag,
                    out topFlag, out bottomFlag, out boxFlag);
            }

            if (boxFlag)
            {
                Cursor = Cursors.SizeAll;
            }
            else if ((leftFlag && topFlag) || (rightFlag && bottomFlag))
            {
                Cursor = Cursors.SizeNWSE;
            }
            else if ((leftFlag && bottomFlag) || (rightFlag && topFlag))
            {
                Cursor = Cursors.SizeNESW;
            }
            else if (leftFlag || rightFlag)
            {
                Cursor = Cursors.SizeWE;
            }
            else if (topFlag || bottomFlag)
            {
                Cursor = Cursors.SizeNS;
            }
            else
            {
                Cursor = Cursors.Arrow;
            }
        }

        private void Canvas_windowPreview_MouseMove(object sender, MouseEventArgs e)
        {
            Point mousePosInCaptureWindow = ConvertPositionToWindowCoordinate(e.GetPosition(canvas_windowPreview));
            ChangeRangeBox(mousePosInCaptureWindow);
            ChangeCursorForRangeBox(mousePosInCaptureWindow);
        }

        private void Canvas_windowPreview_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            leftMarginMoveFlag = false;
            rightMarginMoveFlag = false;
            topMarginMoveFlag = false;
            bottomMarginMoveFlag = false;
            boxMoveFlag = false;
            Mouse.Capture(null);
        }

        private void CompositionTarget_Render(object? sender, EventArgs e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e;
            if (lastRender != args.RenderingTime)
            {
                windowPreview.RequestRender();

                DrawRectangleRange();

                float fps = NM_WindowCapture.GetCaptureFPS(captureObj);
                label_captureFPS.Content = fps.ToString("F1");

                bool isCapturing = NM_WindowCapture.IsCapturing(captureObj);
                button_cameraStart.IsEnabled = (!enableVCam && isCapturing);
                button_cameraReverse.IsEnabled = (enableVCam && isCapturing);
                button_cameraStop.IsEnabled = (enableVCam && isCapturing);
                button_trimmingReset.IsEnabled = isCapturing;
                checkbox_captureCursor.IsEnabled = isCapturing;

                if (isCapturing)
                {
                    checkbox_captureCursor.Foreground = new SolidColorBrush(Colors.Black);
                }
                else
                {
                    checkbox_captureCursor.Foreground = new SolidColorBrush(Colors.Gray);
                }

                lastRender = args.RenderingTime;
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            NM_WindowCapture.DeleteWindowCaptureObject(captureObj);
        }
    }
}
