using System;
using System.Text;
using System.Diagnostics;


using TcpLib;


namespace WindowsService1
{
	/// <SUMMARY>
	/// BartlbyProvider. Just replies messages received from the clients.
	/// </SUMMARY>
	public class BartlbyProvider: TcpServiceProvider
	{
		private string _receivedStr;
        private string hello = "OS: windows.NET V: 0.9\n";
        private WindowsService1.Properties.Settings st = new WindowsService1.Properties.Settings();

		public override object Clone()
		{
			return new BartlbyProvider();
		}

		public override void OnAcceptConnection(ConnectionState state)
		{
            
			_receivedStr = "";
            if (!state.Write(Encoding.UTF8.GetBytes(hello), 0, hello.Length))
				state.EndConnection(); //if write fails... then close connection
		}


		public override void OnReceiveData(ConnectionState state)
		{

            EventLog evnt = new EventLog();
            evnt.Log = "bartlby";
            evnt.Source = "bartlbysource";

			byte[] buffer = new byte[1024];
            char[] bufferp = new char[1024];
            string back;

			while(state.AvailableData > 0)
			{
				int readBytes = state.Read(buffer, 0, 1024);
				if(readBytes > 0)
				{
					_receivedStr += Encoding.UTF8.GetString(buffer, 0, readBytes);
                    
       				/* state.Write(Encoding.UTF8.GetBytes(_receivedStr), 0,
						_receivedStr.Length);
                    */
                        string [] pack = _receivedStr.Split('|');
                        if (pack.Length != 3)
                        {
                            back = "2|Protocoll mismatch\n\n";
                            state.Write(Encoding.UTF8.GetBytes(back),0,back.Length);
                            state.EndConnection();
                            return;

                        }
                        /*
                         *  Plugin exists ?!
                         *
                        */
                        if (!System.IO.File.Exists(st.PluginDir + "\\" + pack[0]))
                        {
                             back = "2|Plugin: " + st.PluginDir + "\\" +pack[0]+" not found\n\n";
                            state.Write(Encoding.UTF8.GetBytes(back), 0, back.Length);
                            state.EndConnection();
                            return;
                        }
                        

                        
                        Process p = new Process();
                        p.StartInfo.RedirectStandardError = true;
                        p.StartInfo.RedirectStandardOutput = true;
                        p.StartInfo.UseShellExecute = false;
                        p.StartInfo.CreateNoWindow = true;
                        p.StartInfo.FileName = st.PluginDir + "\\" + pack[0];
                        p.StartInfo.Arguments = pack[1];
                        p.Start();
                        string op = p.StandardOutput.ReadToEnd();
						
                        try
                        {
                            p.WaitForExit(40 * 1000);
                        }
                        catch 
                        {

                        	back = "2|Plugin: " + st.PluginDir + "\\" + pack[0] + " timed out 40seconds ";
                            state.Write(Encoding.UTF8.GetBytes(back), 0, back.Length);
                            state.EndConnection();
                            
                            return;
                        }
                        

                        



                        op.Replace('\n', ' ');
                        op.Replace('\r', ' ');
                        

                        back = p.ExitCode.ToString() + "|" + op.ToString() + "\n";
                        state.Write(Encoding.UTF8.GetBytes(back), 0, back.Length);

                        //evnt.WriteEntry("Exec: '" +  op + "' Code: '" + p.ExitCode.ToString() + "'");

                        state.EndConnection();
                        


						_receivedStr = "";
					
				}
				else state.EndConnection(); //If read fails then close connection
			}
		}


		public override void OnDropConnection(ConnectionState state)
		{
			//Nothing to clean here
		}
	}
}
