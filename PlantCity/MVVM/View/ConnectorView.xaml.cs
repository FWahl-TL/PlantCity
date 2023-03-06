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

        public string LogText { get; set; }

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
            PushSerialLog("Reading Ports:");
            ComPorts.ForEach(x => PushSerialLog("Found Port: " + x));
        }

        public void StartConnection()
        {
            if(string.IsNullOrEmpty(CBPort?.SelectedValue?.ToString())) {
                Notify.sendError("No Port Selected");
                SetErrorStatus("Stopped");
                PushSerialLog("Stopped");
                return;
            }
            SerialConnection = new SerialPort(CBPort.SelectedValue.ToString(), 9600);
            SetStatus("Opening Serial Connection...");
            PushSerialLog("Opening Serial Connection...");
            try
            {
                SerialConnection.Open();
                Notify.sendSuccess("Stared Connection");
            }
            catch (Exception ex)
            {
                PushSerialLog("Error: " + ex.ToString());
                SetStatus("Stopped");
                PushSerialLog("Stopping Service");
                Notify.sendError("Error: " + ex.ToString());
                return;
            }
            SetStatus("Port ready");
            PushSerialLog("Port ready");

            SerialConnection.DataReceived += SerialConnection_DataReceived;

            //DEMO:
            WriteToSerial("Hello World");
            //var x = SerialConnection.Read(new byte[] { }, 0, 255);
        }

        private void SerialConnection_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;   //Zugriff auf auslösenden Serial Port
            string data = sp.ReadLine();
            PushSerialLog(data, true);
        }

        public void WriteToSerial(string msg)
        {
            SerialConnection.Write(msg);
            PushSerialLog(msg);
        }

        public void PushSerialLog(string message, bool arduino = false)
        {
            string msg;
            if (arduino)
            {
                msg = "Arduino: " + message + "\n";
            }
            else
            {
                msg = "Pc: " + message + "\n";
            }
            LogText = msg + LogText;


            Application.Current.Dispatcher.Invoke(
            () =>
            {
                // Code to run on the GUI thread.
                SerialLog.Text = LogText;
            });
        }

        private void StartBtn_Click(object sender, RoutedEventArgs e)
        {
            StartConnection();
        }

        private void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            Notify.sendSuccess("Stopped");
            SetErrorStatus("Stopped");
            SerialConnection.Close();
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

        private void SaveFeuchtigkeit_Click(object sender, RoutedEventArgs e)
        {
            string message = "FG=" + (CBFeuchtigkeit.SelectedItem as ComboBoxItem).Content;
            WriteToSerial(message);
        }
    }
}
