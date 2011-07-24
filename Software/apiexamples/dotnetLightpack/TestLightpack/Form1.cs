using System;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using libLightpack;


namespace TestLightpack
{
    public partial class Form1 : Form
    {
        private ApiLightpack api;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            api = new ApiLightpack();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (button1.Text != "Disconnect")
            {
                api.Host = textBox1.Text;
                api.Port = Convert.ToInt32(textBox2.Text);
                if (api.Connect())
                    button1.Text = "Disconnect";

                label1.Text = "Version API - " + api.Version;

                label1.Text += " Status:" + api.GetStatus().ToString();

                string[] list = api.GetProfiles();
                listBox1.Items.Clear();
                foreach (string s in list)
                {
                    listBox1.Items.Add(s);
                }
                
            }
            else
            {
                api.UnLock();
                api.Disconnect();
                button1.Text = "Connect";
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
                button3.Text = "UnLock";
            }
            else
            {
                api.UnLock();
                button3.Text = "Lock";
            }

        }

        private void button4_Click(object sender, EventArgs e)
        {
            api.SetAllColor(Color.Black);
            for (int i = 0; i < 10; i++)
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
    }
}
