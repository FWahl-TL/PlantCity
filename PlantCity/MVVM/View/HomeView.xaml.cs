using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using PlantCity.Core;
using RestSharp;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;
using LiveChartsCore.Measure;

namespace PlantCity.MVVM.View
{

    public partial class HomeView : UserControl
    {
        public List<DataResponse> DataPoints { get; set; }
        public ISeries[] HumiditySeries { get; set; }
        public ISeries[] TankSeries { get; set; }
        public ISeries[] RainSeries { get; set; }
        public ISeries[] TempSeries { get; set; }
        public ISeries[] LightSeries { get; set; }

        public IEnumerable<ISeries> HumidityGaugeSeries { get; set; }
        public IEnumerable<ISeries> TankGaugeSeries { get; set; }

        public Axis XAxes { get; set; }

        public HomeView()
        {
            InitializeComponent();
            Loaded += HomeView_Loaded;
        }

        private void HomeView_Loaded(object sender, RoutedEventArgs e)
        {
            GetData();
        }

        public async void GetData()
        { 
            var DataContent = await RestTest();

            JObject json = JObject.Parse(DataContent);

            var Data = json.SelectToken("data");

            DataPoints = Data.ToObject<List<DataResponse>>();

            List<double> humidity = new List<double>();
            List<double> tank = new List<double>();
            List<double> light = new List<double>();
            List<double> temp = new List<double>();
            List<double> rain = new List<double>();

            List<string> labels = new List<string>();
           


            foreach(var item in DataPoints)
            {
                humidity.Add(item.measured_humidity);
                labels.Add(item.time_measured.ToString());
                tank.Add(item.measured_tank);
                light.Add(item.measured_light);
                rain.Add(item.current_rain);
                temp.Add(item.current_temp);
            }
            HumiditySeries = new ISeries[] { new LineSeries<double> { Values = humidity } };
            LightSeries = new ISeries[] { new LineSeries<double> { Values = light } };
            RainSeries = new ISeries[] { new LineSeries<double> { Values = rain } };
            TempSeries = new ISeries[] { new LineSeries<double> { Values = temp } };
            TankSeries = new ISeries[] { new LineSeries<double> { Values = tank } };

            XAxes = new Axis
            {
                Labels = labels,
                LabelsRotation = 0,
                SeparatorsPaint = new SolidColorPaint(new SKColor(200, 200, 200)),
                SeparatorsAtCenter = false,
                TicksPaint = new SolidColorPaint(new SKColor(35, 35, 35)),
                TicksAtCenter = true
            };
            HumidityChart.Series = HumiditySeries;
            LightChart.Series = LightSeries;
            RainChart.Series = RainSeries;
            TempChart.Series = TempSeries;
            TankChart.Series = TankSeries;



            HumidityChart.XAxes = new Axis[] { XAxes};

            double currentHumidity = humidity[humidity.Count - 1];
            double currentTank = tank[tank.Count - 1];

            HumidityGaugeSeries = new GaugeBuilder()
            .WithLabelsSize(50)
            .WithInnerRadius(75)
            .WithBackgroundInnerRadius(75)
            .WithBackground(new SolidColorPaint(new SKColor(100, 181, 246, 90)))
            .WithLabelsPosition(PolarLabelsPosition.ChartCenter)
            .AddValue(Math.Round(currentHumidity), "Feuchtigkeit", SKColors.BlueViolet, SKColors.LightBlue) // defines the value and the color 
            .BuildSeries();

            TankGaugeSeries = new GaugeBuilder()
            .WithLabelsSize(50)
            .WithInnerRadius(75)
            .WithBackgroundInnerRadius(75)
            .WithBackground(new SolidColorPaint(new SKColor(100, 181, 246, 90)))
            .WithLabelsPosition(PolarLabelsPosition.ChartCenter)
            .AddValue(Math.Round(currentTank), "Füllstand", SKColors.BlueViolet, SKColors.LightBlue) // defines the value and the color 
            .BuildSeries();

            HumidityGauge.Series = HumidityGaugeSeries;
            TankGauge.Series = TankGaugeSeries;

        }

        public async Task<string> RestTest()
        {
            
            RestClient client = new RestClient("https://api.florianwahl.digital/api");
            var req = new RestRequest("all", Method.Get);
            req.RequestFormat = RestSharp.DataFormat.Json;
            try
            {
                var res = await client.GetAsync(req);

                return res.Content;
            }
            catch (Exception ex)
            {
                Notify.sendError(ex.Message);
                return null;
            }
        }

        public class DataResponse
        {
            public int id { get; set; }
            public DateTime time_measured { get; set; }
            public double measured_humidity { get; set; }
            public double measured_tank { get; set; }
            public double measured_light { get; set; }
            public double current_temp { get; set; }
            public double current_rain { get; set; }
        }

    }

}
