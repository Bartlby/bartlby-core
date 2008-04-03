using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Management;


namespace CheckMemory
{
    class Program
    {
        static void Main(string[] args)
        {
           

            try
            {

                if (args[0] != "")
                {
                    //jippe
                }

            }
            catch
            {
                System.Console.WriteLine("./checkMemory.exe WARNING CRITICAL\n");
                System.Environment.Exit(1);
            }

            

        }
    }
}
