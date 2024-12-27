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
        public const int previewWidth = 1920;
        public const int previewHeight = 1080;
        private TimeSpan lastRender;

        public MainWindow()
        {
            InitializeComponent();
            windowPreview = new D3D11Image();
            image_windowPreview.Source = windowPreview;
            windowPreview.SetPixelSize(previewWidth, previewHeight);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var helper = new System.Windows.Interop.WindowInteropHelper(this);
            captureObj = NM_WindowCapture.createWindowCaptureObject(helper.Handle);
            windowPreview.WindowOwner = helper.Handle;
            windowPreview.OnRender += PreviewWindow_OnRender;
            CompositionTarget.Rendering += CompositionTarget_Render;
        }

        private void Button_picker_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.openWindowPicker(captureObj);
        }

        private void Button_cameraStart_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.startVirtualCamera(captureObj);
            button_cameraStart.IsEnabled = false;
            button_cameraReverse.IsEnabled = true;
            button_cameraStop.IsEnabled = true;
        }

        private void Button_cameraReverse_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.switchReverseCaptureWindow(captureObj);
        }

        private void Button_cameraStop_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.stopVirtualCamera(captureObj);
            button_cameraStart.IsEnabled = true;
            button_cameraReverse.IsEnabled = false;
            button_cameraStop.IsEnabled = false;
        }

        private void PreviewWindow_OnRender(nint resourcePtr, bool isNewSurface)
        {
            NM_WindowCapture.copyCapturePreviewToDXGIResource(captureObj, resourcePtr);
        }

        private void CompositionTarget_Render(object? sender, EventArgs e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e;
            if (lastRender != args.RenderingTime)
            {
                windowPreview.RequestRender();
                lastRender = args.RenderingTime;
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            NM_WindowCapture.deleteWindowCaptureObject(captureObj);
        }
    }
}
