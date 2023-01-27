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

namespace PlantCity.MVVM.View
{

    public partial class HomeView : UserControl
    {
        public List<DataResponse> DataPoints { get; set; }
        public ISeries[] SeriesCollection { get; set; }

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

            SeriesCollection = new ISeries[] { new LineSeries<int> { Values = new int[]{0,1,2,3,4,5,6,7,8, } } };
            DafaultChart.Series = SeriesCollection;
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
