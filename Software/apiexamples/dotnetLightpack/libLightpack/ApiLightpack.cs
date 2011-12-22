using System;
using System.Drawing;
using System.Net.Sockets;
using System.Text;

namespace libLightpack
{
	public class ApiLightpack : IDisposable
    {

        private string _version;
	    private int _countLeds;

        public string Host = "127.0.0.1";
        public int Port = 3636;
        public string ApiKey = "";

	    private bool isAuth = false;
        private bool isLock = false;


        public string Version
        {
            get { return _version; }
        }

        public int CountLeds
        {
            get
            {
                return _countLeds;
            }
        }

	    public bool IsAuth
	    {
            get { return isAuth; }
	    }

        public bool IsLock
        {
            get { return isLock; }
        }

        private TcpClient _client;

        public ApiLightpack()
        {
            
        }
        
        public void Dispose()
        {
        	Disconnect();
        }

        public ApiLightpack(string host, int port)
        {
            Host = host;
            Port = port;
        }

        public ApiLightpack(string host, int port, string apikey) : this(host, port)
        {
            ApiKey = apikey;
        }

        private string _readData()
        {
            return _readData(true);
        }

	    private string _readData(bool clear)
        {
            Byte[] bytesReceived = new Byte[256];
            string data = null;
            int bytes = 0;
            
            do
            {
                bytes = _client.Client.Receive(bytesReceived, bytesReceived.Length, 0);
                data = data + Encoding.UTF8.GetString(bytesReceived, 0, bytes);
            }
            while (_client.Available > 0);

            if (clear)
            {
                data = data.Replace("\n", string.Empty);
                data = data.Replace("\r", string.Empty);
            }
	        return data;
        }
        private void _sendData(string data)
        {
            Byte[] bytesSent = Encoding.UTF8.GetBytes(data);
            _client.Client.Send(bytesSent);
        }

        public void Connect()
        {
            _client = new TcpClient();
            if (!_client.Connected)
            {
                isLock = false;
                isAuth = false;
	            _client.Connect(Host, Port);
	            string[] list = _readData().Split(':');
		     	if (list.Length > 1)
		         	_version = list[1];
                // old version api
                if (_version == "1.0")
                    isAuth = true;
                else
                    Login();

                GetCountLeds();
            }
        }

        public void Login()
        {
            Login(ApiKey);
        }

        public void Login(string apikey)
        {
            if (!_client.Connected) return;

             _sendData(String.Format("apikey:{0}\n", ApiKey));
             string s = _readData();
             if (s.IndexOf("ok")!=-1)
                 isAuth = true;
        }
        
        public bool Connected
        {
        	get { return _client.Connected;}
        }

        public void Disconnect()
        {
            if (_client.Connected)
            {
                _sendData("exit\n");
                _client.Close();
            }
        }

        public Status GetStatus()
        {
            _sendData("getstatus\n");   
            string s = _readData();
            string[] list = s.Split(':');
            return (Status)Enum.Parse(typeof(Status),list[1],true);
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
            return (StatusApi)Enum.Parse(typeof(StatusApi),list[1],true);
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

        public int GetCountLeds()
        {
            _countLeds = 0;
            _sendData("getcountleds\n");
            string s = _readData();
            string[] list = s.Split(':');
            if (list.Length > 1)
            {
                _countLeds = Convert.ToInt32(list[1]);
            }
            return _countLeds;
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
            for (int i = 0; i < _countLeds; i++)
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

        public string Help()
        {
            _sendData("help\n");
            return _readData(false);
        }
    }
}
