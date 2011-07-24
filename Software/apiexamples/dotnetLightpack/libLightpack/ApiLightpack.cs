using System;
using System.Drawing;
using System.Net.Sockets;
using System.Text;

namespace libLightpack
{
    public class ApiLightpack
    {

        private string _version;

        public string Host = "127.0.0.1";
        public int Port = 3636;

        private bool isLock = false;

        public string Version
        {
            get { return _version; }
        }

        private readonly TcpClient _client;

        public ApiLightpack()
        {
            _client = new TcpClient();
        }

        public ApiLightpack(string host, int port)
        {
            Host = host;
            Port = port;
        }

        private string _readData()
        {
            Byte[] bytesReceived = new Byte[256];
            string data = null;

            int bytes = _client.Client.Receive(bytesReceived, bytesReceived.Length, 0);
            data = data + Encoding.UTF8.GetString(bytesReceived, 0, bytes);

            data = data.Replace("\n", string.Empty);
            return data;
        }
        private void _sendData(string data)
        {
            Byte[] bytesSent = Encoding.UTF8.GetBytes(data);
            _client.Client.Send(bytesSent);
        }

        public bool Connect()
        {
            try
            {
                _client.Connect(Host, Port);
                setVersion(_readData());
                return true;
            }
            catch (Exception)
            {
                return false;
            }

        }

        public void Disconnect()
        {
            _client.Close();
        }

        public Status GetStatus()
        {
            _sendData("getstatus\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                if (list[1] == "off") return Status.Off;
                if (list[1] == "on") return Status.On;
            }
            return Status.Error;
        }

        public void SetStatus(Status status)
        {
            if (!isLock) return;
            string s="off";
            if (status == Status.On) s = "on";
            _sendData(String.Format("setstatus:{0}\n",s));
            _readData();
        }

        public StatusApi GetStatusApi()
        {
            _sendData("getstatusapi\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                if (list[1] == "idle") return StatusApi.Idle;
                if (list[1] == "busy") return StatusApi.Busy;
            }
            return StatusApi.Busy;
        }

        public string GetProfile()
        {
            _sendData("getprofile\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                return list[1];
            }
            return "";
        }

        public void SetProfile(string profile)
        {
            if (!isLock) return;
            _sendData(String.Format("setprofile:{0}\n", profile));
            _readData();
        }

        public string[] GetProfiles()
        {
            _sendData("getprofiles\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                list = list[1].Split(';');
                return list;
            }
            return null;
        }

        private void setVersion(string readData)
        {
            string[] list = readData.Split(':');
            if (list.Length > 1)
                _version = list[1];
        }

        public void Lock()
        {
            _sendData("lock\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                if (list[1] == "success")
                   isLock = true;
            }
        }

        public void UnLock()
        {
            if (!isLock) return;
            _sendData("unlock\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                if (list[1] == "success")
                    isLock = false;
            }
        }

        public void SetAllColor(Color color)
        {
            string com = "setcolor:";
            for (int i = 0; i < 10; i++)
            {
                com += String.Format("{0}-{1},{2},{3};", i+1,color.R,color.G,color.B);
            }
            com += "\n";
            _sendData(com);
            _readData();
        }

        public void SetColor(int num, Color color)
        {
            if (num < 1 && num > 10) return;
            string com = String.Format("setcolor:{0}-{1},{2},{3};", num, color.R, color.G, color.B);
            com += "\n";
            _sendData(com);
            _readData();
        }

        public int Smooth
        {
            set
            {
               _sendData(String.Format("setsmooth:{0}\n",value));
               _readData();
            }
        }

        public int Gamma
        {
            set
            {
                _sendData(String.Format("setgamma:{0}\n", value));
                _readData();
            }
        }
    }
}
