using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Xml.Serialization;

using libLightpack;

namespace TestLightpack
{
	public struct Options
    {
        public string host;
        public string port;
        public string apikey;
    }
	
    public partial class Form1 : Form
    {
    	string configFile = Application.StartupPath+@"\option.cfg";
    	
        private ApiLightpack api;

        public Form1()
        {
            InitializeComponent();
            api = new ApiLightpack();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            label1.Text = String.Empty;
            label6.Text = String.Empty;
            string configFile = Application.StartupPath + @"\option.cfg";
        	if (File.Exists(configFile))
        	{
	        	XmlSerializer myXmlSer = new XmlSerializer(typeof(Options));
	            FileStream mySet = new FileStream(configFile, FileMode.Open);
	            Options o = (Options)myXmlSer.Deserialize(mySet);
	            mySet.Close();
	        	textBox1.Text = o.host;
	        	textBox2.Text = o.port;
	        	textBox3.Text = o.apikey;
        	}
            
        }
    
        void Form1FormClosed(object sender, FormClosedEventArgs e)
        {
        	Options o;
        	o.host = textBox1.Text;
        	o.port = textBox2.Text;
        	o.apikey = textBox3.Text;
        	XmlSerializer myXmlSer = new XmlSerializer(o.GetType());
            StreamWriter myWriter = new StreamWriter(configFile);
            myXmlSer.Serialize(myWriter,o);
            myWriter.Close();
        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            if (button1.Text != "Disconnect")
            {
                api.Host = textBox1.Text;
                api.Port = Convert.ToInt32(textBox2.Text);
                api.ApiKey = textBox3.Text;
                api.Connect();
                if (api.Connected)
                    button1.Text = "Disconnect";

                label1.Text = "Version API - " + api.Version;

                label1.Text += " Auth:" + api.IsAuth.ToString();

                label1.Text += " Count leds:" + api.CountLeds.ToString();

                button3.Text = "Lock";

                button2.Enabled = api.IsLock;
                button4.Enabled = api.IsLock;
                button5.Enabled = api.IsLock;
                    


                if (api.IsAuth)
                {
                    label6.Text = " Status:" + api.GetStatus().ToString();

                    button3.Enabled = true;
                    button6.Enabled = true;
                    string[] list = api.GetProfiles();
                    listBox1.Items.Clear();
                    foreach (string s in list)
                    {
                        listBox1.Items.Add(s);
                    }
                    listBox1.Enabled = true;
                }
                else
                {
                    button3.Enabled = false;
                    button6.Enabled = false;
                    listBox1.Enabled = false;
                }
            }
            else
            {
                api.UnLock();
                api.Disconnect();
                label1.Text = String.Empty;
                label6.Text = String.Empty;
                button1.Text = "Connect";
                button2.Enabled = false;
                button4.Enabled = false;
                button3.Enabled = false;
                button5.Enabled = false;
                button6.Enabled = false;
                    
                listBox1.Enabled = false;
                listBox1.Items.Clear();
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            ColorDialog cd = new ColorDialog();
            if (cd.ShowDialog()==DialogResult.OK)
            {
                api.SetAllColor(cd.Color);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (button3.Text == "Lock")
            {
                api.Lock();
            }
            else
            {
                api.UnLock();
            }
            button3.Text = (api.IsLock) ? "UnLock" : "Lock";
            button2.Enabled = api.IsLock;
            button4.Enabled = api.IsLock;
            button5.Enabled = api.IsLock;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            api.SetAllColor(Color.Black);
            for (int i = 0; i < api.CountLeds; i++)
            {
                Thread.Sleep(200);
                api.SetColor(i+1,Color.Red);
                if (i>0)
                    api.SetColor(i,Color.Black);
            }
        }

        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex>=0)
            {
                api.Lock();
                api.SetProfile(listBox1.Items[listBox1.SelectedIndex].ToString());
                api.UnLock();
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (api.GetStatus()==Status.On)
                api.SetStatus(Status.Off);
            else
                api.SetStatus(Status.On);
            label6.Text = " Status:" + api.GetStatus().ToString();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            HelpForm hf = new HelpForm();
            hf.richTextBox1.Text = api.Help();
            hf.ShowDialog();
        }
        
        
    }
}
