using RestSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.ToolTip;

namespace PlantCity.Core
{
    public static class ApiHandler
    {
        public static RestClient client;

        public static void init()
        {
            if(client == null)
            {
                var options = new RestClientOptions("http://api.florianwahl.digital/api")
                {
                    ThrowOnAnyError = true,
                    MaxTimeout = 2000
                };
                client = new RestClient(options);

            }
        }

        public static async Task <(bool, string)> getData()
        {
            showWaiting();
            var request = new RestRequest("all", Method.Get);
            //request.RequestFormat = DataFormat.Json;
            
            var response = await client.ExecuteAsync<RestResponse>(request);

            if (response.StatusCode == System.Net.HttpStatusCode.OK)
            {
                endWaiting();
                return (true, response.Content);
            }
            endWaiting();
            return (false, "Datenbank-Fehler!");
        }

        public static void showWaiting()
        {
            Cursor.Current = Cursors.WaitCursor;
        }

        public static void endWaiting()
        {
            Cursor.Current = Cursors.Default;
        }
    }
}
