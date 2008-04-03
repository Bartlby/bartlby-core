using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceProcess;



namespace CheckService
{
    class Program
    {
        static void Main(string[] args)
        {
            int ex = 2;

            try
            {

                if (args[0] != "")
                {
                    //jippe
                }
                
            }
            catch
            {
                System.Console.WriteLine("./checkService.exe ServiceName\n");
                System.Environment.Exit(1);
            }


            
            try
            {
                ServiceController sc = new ServiceController(args[0]);
                switch (sc.Status)
                {
                    case System.ServiceProcess.ServiceControllerStatus.Stopped:
                    case System.ServiceProcess.ServiceControllerStatus.StopPending:
                        ex = 2;            
                    break;
                    case System.ServiceProcess.ServiceControllerStatus.Paused:
                    case System.ServiceProcess.ServiceControllerStatus.PausePending:
                    case System.ServiceProcess.ServiceControllerStatus.StartPending:
                    case System.ServiceProcess.ServiceControllerStatus.ContinuePending:
                        ex = 1;
                    break;
                    case System.ServiceProcess.ServiceControllerStatus.Running:
                        ex = 0;
                    break;



                }
                System.Console.WriteLine("Service (" + args[0] + ") is [" + sc.Status.ToString() + "]");
                System.Environment.Exit(ex);
            }
            catch
            {
                System.Console.WriteLine("Service: " + args[0] + " not found");
                
                System.Environment.Exit(2);
            }

            

            
        }
    }
}
