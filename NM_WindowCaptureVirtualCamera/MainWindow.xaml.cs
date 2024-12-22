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

namespace NM_WindowCaptureVirtualCamera
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public IntPtr captureObj = IntPtr.Zero;
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var helper = new System.Windows.Interop.WindowInteropHelper(this);
            captureObj = NM_WindowCapture.createWindowCaptureObject(helper.Handle);
        }

        private void button_picker_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.openWindowPicker(captureObj);
        }

        private void button_cameraStart_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.startVirtualCamera(captureObj);
        }

        private void button_cameraReverse_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.switchReverseCaptureWindow(captureObj);
        }

        private void button_cameraStop_Click(object sender, RoutedEventArgs e)
        {
            NM_WindowCapture.stopVirtualCamera(captureObj);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            NM_WindowCapture.deleteWindowCaptureObject(captureObj);
        }
    }
}
