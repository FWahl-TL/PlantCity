using PlantCity.Core;
using System;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Input;
using Forms = System.Windows.Forms;
namespace PlantCity
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public bool IsFullScreen { get; set; }
        public bool IsMinimized { get; set; }

        public MainWindow()
        {
            Logger.Log("starting components");
            InitializeComponent();
            //ApiHandler.init();
            Logger.Log("Done starting components");
        }

        private void CloseBtn_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void FullScreenBtn_Click(object sender, RoutedEventArgs e)
        {
            if(IsFullScreen)
            {
                this.WindowState = WindowState.Normal;
            }
            else
            {
                this.WindowState = WindowState.Maximized;
            }
            IsFullScreen = !IsFullScreen;
        }

        private void MinimizeBtn_Click(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }

        private void DockPanel_MouseDrag(object sender, MouseEventArgs e)
        {
            this.WindowState = WindowState.Normal;
            DragMove();
        }
    }
}
