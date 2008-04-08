using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;
using TcpLib;

namespace WindowsService1
{
    public partial class Service1 : ServiceBase
    {
        private TcpServer agent_server;
        private BartlbyProvider provider;
        private string bartlby_evnt_source = "bartlbysource";
        private string bartlby_evnt_log_name = "bartlby";

      

        public Service1()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            // TODO: Add code here to start your service.
            if (!System.Diagnostics.EventLog.SourceExists(bartlby_evnt_source))
            {
                System.Diagnostics.EventLog.CreateEventSource(bartlby_evnt_source,bartlby_evnt_log_name);
            }
            eventLog1.Source = bartlby_evnt_source;
            eventLog1.Log = bartlby_evnt_log_name;

            WindowsService1.Properties.Settings st = new Properties.Settings();
            

            provider = new BartlbyProvider();
            agent_server = new TcpServer(provider, st.Port);
            agent_server.MaxConnections = 200;

            agent_server.Start();
            eventLog1.WriteEntry("agent loaded on Port: " + st.Port);

        }

        protected override void OnStop()
        {
            // TODO: Add code here to perform any tear-down necessary to stop your service.
            agent_server.Stop();
            
            eventLog1.WriteEntry("agent stopped");
        }
    }
}
