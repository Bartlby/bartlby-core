using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WindowsApplication1
{
    public partial class Form3 : Form
    {
        public Form3()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            WindowsApplication1.Properties.Settings st;
            st = new WindowsApplication1.Properties.Settings();
            st.Username = textBox1.Text;
            st.Password = textBox2.Text;
            st.BaseURL = textBox3.Text;
            
            st.Save();
            
            this.Close();

        }

        private void Form3_Load(object sender, EventArgs e)
        {
            WindowsApplication1.Properties.Settings st;
            st = new WindowsApplication1.Properties.Settings();
            textBox1.Text = st.Username;
            textBox2.Text = st.Password;
            textBox3.Text = st.BaseURL;
            

        }
    }
}