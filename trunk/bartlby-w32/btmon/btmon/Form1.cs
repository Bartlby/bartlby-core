
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.ServiceProcess;
using Nwc.XmlRpc;
using System.Collections;

namespace WindowsApplication1
{
    public partial class Form1 : Form
    {
       
        private int glob_serverID;
        private int glob_serviceID;
        private WindowsApplication1.Properties.Settings st = new WindowsApplication1.Properties.Settings();

        private void Form1_Move(object sender, EventArgs e)
        {
            if (this == null)
            { //This happen on create.
                return;
            }
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Hide();

            }
            else
            {//any other windows state show it.
                this.Show();
            }



        }

          private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            //There are several ways to close an application.
            //We are trying to find the click of the X in the upper right hand corner
            //We will only allow the closing of this app if it is minized.
            if (this.WindowState != FormWindowState.Minimized)
            {
                //we don't close the app...
                e.Cancel = true;
                //minimize the app and then display a message to the user so
                //they understand they didn't close the app they just sent it to the tray.
                this.WindowState = FormWindowState.Minimized;
                //Show the message.
               
            }
        }

      

        public Form1()
        {
            InitializeComponent();
            listView1.View = View.Details;
            // Allow the user to edit item text.
            listView1.LabelEdit = false;
            // Allow the user to rearrange columns.
            listView1.AllowColumnReorder = true;
            // Display check boxes.
            listView1.CheckBoxes = false;
            // Select the item and subitems when selection is made.
            listView1.FullRowSelect = true;
            // Display grid lines.
            listView1.GridLines = false;
            // Sort the items in the list in ascending order.
            listView1.Columns.Add("Date",150);
           
            listView1.Columns.Add("Message", 400);

            

            addLog(3, "startup...");

            this.EnableStuff();

            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            notifyIcon1.ShowBalloonTip(2000, "Title", "Tiptext", ToolTipIcon.Info);


        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            st.Reload();

            if (st.Username == "" || st.Password == "" || st.BaseURL == "")
            {
                Form3 fm3 = new Form3();
                fm3.ShowDialog();
                return;
            }
            
            webBrowser1.AllowWebBrowserDrop = false;
            webBrowser1.IsWebBrowserContextMenuEnabled = true;
            webBrowser1.WebBrowserShortcutsEnabled = false;
            webBrowser1.ObjectForScripting = this;
            // Uncomment the following line when you are finished debugging.
            webBrowser1.ScriptErrorsSuppressed = false;

            webBrowser1.Navigate(st.BaseURL + "/event_stream.php?data="  + System.DateTime.Now);
            this.DisableStuff();



        }
        public void cleartTreeview()
        {
            treeView1.BeginUpdate();

            // Clear the TreeView each time the method is called.
            treeView1.Nodes.Clear();
        }
        public void BaloonIT(String Code, String message)
        {
            String[] splittet_array;
            String HRLabel="CRITICAL";
            ToolTipIcon levelIcon=ToolTipIcon.Error;

            
            switch(Code) {
                
                case "2":
                case "3":
                    splittet_array = message.Split(new Char[] { ';' });
                    switch (splittet_array[3])
                    {
                        case "0":
                            HRLabel = "OK";
                            levelIcon = ToolTipIcon.Info;
                        break;
                        case "1":
                            HRLabel = "WARNING";
                            levelIcon = ToolTipIcon.Warning;
                        break;

                        case "2":
                            HRLabel = "CRITICAL";
                            levelIcon = ToolTipIcon.Error;
                        break;
                    }
                    if (Code == "3")
                    {
                        notifyIcon1.ShowBalloonTip(5000, "(" + HRLabel + ")" + splittet_array[2], splittet_array[4], levelIcon);
                        
                    }

                    findServiceNodeIndex(splittet_array[1]);
                    if (glob_serverID >= 0 && glob_serviceID >= 0)
                    {
                        treeView1.Nodes[glob_serverID].Nodes[glob_serviceID].ImageIndex = Convert.ToInt32(splittet_array[3]);
                        treeView1.Nodes[glob_serverID].Nodes[glob_serviceID].SelectedImageIndex = Convert.ToInt32(splittet_array[3]);

                    }
                    if (Code == "3")
                    {
                        addLog(4, "Trigger Fired: " + splittet_array[2] + "  " + splittet_array[4]);
                    }
                    else
                    {
                        addLog(Convert.ToInt32(splittet_array[3]), "State changed: " + splittet_array[2] + "  " + splittet_array[4]);
                    }
               break;

            }
        }
        private void  findServiceNodeIndex(string idx)
        {



            for (int x = 0; x < treeView1.Nodes.Count; x++)
            {
                for (int y = 0; y < treeView1.Nodes[x].Nodes.Count; y++)
                {
                    
                    if (treeView1.Nodes[x].Nodes[y].ToolTipText == idx)
                    {
                        glob_serverID = x;
                        glob_serviceID = y;
                        return;

                    }
                }
            }

            glob_serverID = -1;
            glob_serviceID = -1;

        }
     

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void webBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            

            this.EnableStuff();


        }

        private void EnableStuff()
        {
            button1.Enabled = true;
            button2.Enabled = false;



            toolStripStatusLabel1.Text = "Idle";
            toolStripStatusLabel2.Text = "";
            toolStripProgressBar1.Value = 0;
            groupBox1.Visible = false;
            treeView1.BeginUpdate();

            // Clear the TreeView each time the method is called.
            treeView1.Nodes.Clear();
            treeView1.EndUpdate();


        }
        public void setLoggedInName(string uname)
        {
            label5.Text = uname;

        }
        public void bzsetBarColor(string color, string proz)
        {
            
            
            toolStripProgressBar1.Value = Convert.ToInt32(proz);
            toolStripStatusLabel2.Text = " Health: " + proz + " %";
            notifyIcon1.Text = toolStripStatusLabel2.Text;

        }
        private void DisableStuff()
        {
            button1.Enabled = false;
            button2.Enabled = true;

            toolStripStatusLabel1.Text = "Running....";
            groupBox1.Visible = true;


        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            
        }
        public void FinishTree()
        {
            treeView1.EndUpdate();

        }
        public void addServer(String ServerID, String ServerName)
        {
            TreeNode tr = new TreeNode(ServerName);
            tr.Name = ServerID;
            tr.ImageIndex = 3;
            tr.SelectedImageIndex = 3;


            treeView1.Nodes.Add(tr);
            

            
        }
        private int findServerNodeIndex(string ServerID) {
                for(int x=0; x<treeView1.Nodes.Count; x++) {
                        if(treeView1.Nodes[x].Name == ServerID) {
                                //MessageBox.Show("ID:" + ServerID + " found @ " + x);
                                return x;
                        }
                }
                MessageBox.Show("ID:" + ServerID + " LOST");
            return -1;
        }
        public void addService(string serverID, string serviceID, string serviceName, string serviceState, string serviceText)
        {
            int Sidx = findServerNodeIndex(serverID);
            TreeNode tr = new TreeNode(serviceName);
            tr.ImageIndex = Convert.ToInt32(serviceState);
            tr.SelectedImageIndex = Convert.ToInt32(serviceState);
            tr.ToolTipText = serviceID;


            tr.Name = serviceID;

            if (Sidx < 0)
            {
                return;
            }
            treeView1.Nodes[Sidx].Nodes.Add(tr);

            

        }
        private void button2_Click(object sender, EventArgs e)
        {
            
            treeView1.BeginUpdate();
            treeView1.Nodes.Add(new TreeNode("Name"));
            treeView1.Nodes[0].Name = "Server1";

            treeView1.Nodes[0].Nodes.Add(new TreeNode("sub"));
            treeView1.Nodes[0].Nodes[0].Name = "Sub1";

            MessageBox.Show(treeView1.Nodes[0].Name);
            treeView1.EndUpdate();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
           

        }
        public void setHostsUPDown(string up, string down)
        {
            label7.Text = up + "/" + down;
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {

        }

        private void exitToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
                        
            st.Save();
            
            Close();

        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            webBrowser1.Navigate("about:blank");

            


        }
        private void addLog(int Code, string MSG)
        {
            ListViewItem list1 = new ListViewItem(DateTime.Now.ToString(),Code);
            list1.SubItems.Add(MSG);
            listView1.Items.Add(list1);
        }

        private void notifyIcon1_mousemove(object sender, MouseEventArgs e)
        {
            
        }
        public void setOWCDA(string o, string w, string c, string d, string a)
        {
            if (o == "")
            {
                o = "0";
            }
            if (w == "")
            {
                w = "0";
            }
            if (c == "")
            {
                c = "0";
            }
            if (d == "")
            {
                d = "0";
            }
            if (a == "")
            {
                a = "0";
            }

            label6.Text = o + "/" +  w + "/" + c + "/" + d + "/" + a;

        }
        public void setServerOWCDA(string sid,string nam,  string o, string w, string c, string d, string a)
        {
            int Sidx = findServerNodeIndex(sid);
            if (o == "")
            {
                o = "0";
            }
            if (w == "")
            {
                w = "0";
            }
            if (c == "")
            {
                c = "0";
            }
            if (d == "")
            {
                d = "0";
            }
            if (a == "")
            {
                a = "0";
            }


            treeView1.Nodes[Sidx].Text = nam + "  " + o + "/" + w + "/" + c + "/" + d + "/" + a;

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        public void setVersionUptimeRoundTime(string version, string uptime, string avgms)
        {
            label8.Text = uptime;
            label10.Text = version;
            label12.Text = avgms + " ms";

        }


        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show("'" + treeView1.SelectedNode.ToolTipText + "'");
        }

      

       

      

      

        
        
        void ContextMenuStrip2Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (treeView1.Nodes.Count == 0)
            {
                e.Cancel = true;
                return;


            }
            if (treeView1.SelectedNode.ToolTipText == "")
            {
                e.Cancel = true;
                return;
            }
        }
        
        void ToolStripMenuItem2Click(object sender, System.EventArgs e)
        {
        	
        }

        private void notifyIcon1_DoubleClick(object sender, MouseEventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Show();
                this.WindowState = FormWindowState.Normal;
            }

            // Activate the form.
            this.Activate();
            this.Focus();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form fm = new Form2();
            fm.ShowDialog();


        }

        private void forceToolStripMenuItem_Click(object sender, EventArgs e)
        {

            if (st.Username == "" || st.Password == "" || st.BaseURL == "")
            {
                MessageBox.Show("Config first");
                return;

            }

            XmlRpcRequest cl = new XmlRpcRequest();

            cl.MethodName = "bartlby.force_service";
            cl.Params.Add(treeView1.SelectedNode.ToolTipText);

            try
            {
                XmlRpcResponse r = cl.Send(st.BaseURL + "/xml_api.php");
                MessageBox.Show("Service: '" + treeView1.SelectedNode.Text + "' force!");
            }
            catch (Exception rex)
            {
                MessageBox.Show(rex.ToString());

            }

            
            




        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            
        }

        private void clickMenuShow(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Show();
                this.WindowState = FormWindowState.Normal;
            }

            // Activate the form.
            this.Activate();
            this.Focus();
        }

        private void ballclicked(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Show();
                this.WindowState = FormWindowState.Normal;
            }

            // Activate the form.
            this.Activate();
            this.Focus();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int is_r;
            is_r = 3;
            try
            {
                ServiceController sc = new ServiceController("bartlbyagent");
                switch (sc.Status)
                {
                    case System.ServiceProcess.ServiceControllerStatus.Stopped:
                       is_r = 0;
                    break;

                    case System.ServiceProcess.ServiceControllerStatus.StartPending:
                    case System.ServiceProcess.ServiceControllerStatus.StopPending:
                    case System.ServiceProcess.ServiceControllerStatus.PausePending:
                    case System.ServiceProcess.ServiceControllerStatus.ContinuePending:
                        is_r = 2;
                    break;

                    
                    
                    case System.ServiceProcess.ServiceControllerStatus.Running:
                        is_r = 1;
                    break;

                    case System.ServiceProcess.ServiceControllerStatus.Paused:
                        is_r = 3;
                    break;



                }

                switch (is_r)
                {
                    case 0:
                        //process is stopped
                        button3.Enabled = true;
                        button4.Enabled = false;
                        
                    break;
                    case 1:
                        //running
                        button3.Enabled = false;
                        button4.Enabled = true;
                    break;
                    case 2:
                        //pending
                        button3.Enabled = false;
                        button4.Enabled = false;
                    break;
                    case 3:
                        //paused
                        button3.Enabled = true;
                        button4.Enabled = true;
                    break;
                }
                label14.Text = "BartlbyAgent is [" + sc.Status.ToString() + "]";

            }
            catch
            {
                label14.Text = "BartlbyAgent is not installed";
                button3.Enabled = false;
                button4.Enabled = false;
            }

        }

        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                ServiceController sc = new ServiceController("bartlbyagent");
                sc.Stop();
                sc.Stop();
            }
            catch 
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                ServiceController sc = new ServiceController("bartlbyagent");
                sc.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            
            
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
           
            
        }

        private void optionsToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            Form3 fm3 = new Form3();
            fm3.ShowDialog();
        }

        private void outputToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XmlRpcRequest client = new XmlRpcRequest();
            client.MethodName = "bartlby.get_service_with_id";
            client.Params.Add(treeView1.SelectedNode.ToolTipText);

            try
	        {
                Hashtable results = (Hashtable)client.Invoke(st.BaseURL + "/xml_api.php");
                /*
	            Hashtable results = (Hashtable)client.Invoke(st.BaseURL + "/xml_api.php");
                if (results.Contains("new_server_text") && results.Contains("current_state"))
                {
                    MessageBox.Show(results["new_server_text"].ToString(), results["server_name"].ToString() + ":" + results["client_port"] + "/" + results["service_name"] ,MessageBoxButtons.OK, MessageBoxIcon.Information);

                }


                */
                Form4 fm4 = new Form4();
                fm4.listView1.View = View.Details;
                // Allow the user to edit item text.
                fm4.listView1.LabelEdit = true;
                // Allow the user to rearrange columns.
                fm4.listView1.AllowColumnReorder = true;
                // Display check boxes.
                fm4.listView1.CheckBoxes = false;
                // Select the item and subitems when selection is made.
                fm4.listView1.FullRowSelect = true;
                // Display grid lines.
                fm4.listView1.GridLines = false;
                // Sort the items in the list in ascending order.

                foreach (DictionaryEntry reso in results)
                {


                    try
                    {
                        ListViewItem tls = new ListViewItem(reso.Key.ToString());
                        tls.SubItems.Add(reso.Value.ToString());
                        fm4.listView1.Items.Add(tls);
                    }
                    catch
                    {
                        ///
                    }

                }

                fm4.ShowDialog();

                
	            

	            
	        }
	        
	        catch (Exception ex)
	        {
                MessageBox.Show(ex.ToString());          
	        }

      


        }

        private void detailsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(st.BaseURL + "/service_detail.php?service_id=" + treeView1.SelectedNode.ToolTipText);

        }

        private void modifyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(st.BaseURL + "/modify_service.php?service_id=" + treeView1.SelectedNode.ToolTipText);
        }

        private void treeView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try {
                if (treeView1.SelectedNode.ToolTipText != "")
                {
                    this.outputToolStripMenuItem_Click(sender, e);


                }  
             } catch {
             }


        }

        private void reloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XmlRpcRequest cl = new XmlRpcRequest();

            cl.MethodName = "bartlby.reload";
            cl.Params.Add("");

            try
            {
                XmlRpcResponse r = cl.Send(st.BaseURL + "/xml_api.php");
                MessageBox.Show("Command: " + cl.MethodName.ToString() + " executed with result: " + r.Value.ToString());
            }
            catch (Exception rex)
            {
                MessageBox.Show(rex.ToString());

            }
        }

    

      

     

      
       
    }
}
