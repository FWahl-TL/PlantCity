using PlantCity.Core;
using System;
using System.Collections.Generic;
using System.IO.Ports;
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

namespace PlantCity.MVVM.View
{
    /// <summary>
    /// Interaction logic for ConnectorView.xaml
    /// </summary>
    public partial class ConnectorView : UserControl
    {
        public List<string> ComPorts { get; set; }

        public SerialPort SerialConnection {get;set;}

        public ConnectorView()
        {
            InitializeComponent();
            Loaded += ConnectorView_Loaded;
        }

        private void ConnectorView_Loaded(object sender, RoutedEventArgs e)
        {
            CheckPorts();
            SetErrorStatus("Stopped");
        }

        public void CheckPorts()
        {
            ComPorts = SerialPort.GetPortNames().ToList();
            CBPort.ItemsSource = ComPorts;
        }

        public void StartConnection()
        {
            if(string.IsNullOrEmpty(CBPort?.SelectedValue?.ToString())) {
                Notify.sendError("No Port Selected");
                SetErrorStatus("Stopped");
                return;
            }
            SerialConnection = new SerialPort(CBPort.SelectedValue.ToString(), 155200);
            SetStatus("Opening Serial Connection...");
            SerialConnection.Open();
            SetStatus("Port ready");

            //DEMO:
            SerialConnection.Write("Hello World");
            var x = SerialConnection.Read(new byte[] { }, 0, 255);
        }
            
        private void StartBtn_Click(object sender, RoutedEventArgs e)
        {
            Notify.sendSuccess("Stared Connection");
            StartConnection();
        }

        private void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            Notify.sendSuccess("Stopped");
            SetErrorStatus("Stopped");
        }

        public void SetStatus(string msg)
        {
            StatusText.Foreground = Brushes.White;
            StatusText.Content = msg;
        }
        public void SetErrorStatus(string msg)
        {
            StatusText.Foreground = Brushes.Red;
            StatusText.Content = msg;
        }
    }
}
