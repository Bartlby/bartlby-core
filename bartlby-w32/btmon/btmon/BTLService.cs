using System;
using System.Collections.Generic;
using System.Text;

namespace WindowsApplication1
{
    class BTLService
    {
        private string ServiceName;
        private string ServerName;
        private string ServiceText;
        private string ServiceStatus;
        public BTLService(string SName,string ServerName, string ServiceText, string SStatus) {
            this.ServerName = ServerName;
            this.ServiceName = SName;
            this.ServiceText = ServiceText;
            this.ServiceStatus = SStatus;

        }
    }
}
