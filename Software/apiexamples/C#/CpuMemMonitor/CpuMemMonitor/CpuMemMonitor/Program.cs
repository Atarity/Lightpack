using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace CpuMemMonitor
{
    class Program
    {

        private static StreamWriter swSender;
        private static StreamReader srReceiver;
        private static TcpClient tcpClient;
        private static Thread thrMessaging;
        private static bool Connected;

        private static PerformanceCounter cpucounter;
        private static PerformanceCounter memcounter;
        private static Thread thrCpuMem;
        public static int[] ChekColor = new int[] { 0, 0 };

        #region CpuMem

        public static void CpuMem()
        {
            int Cpu;
            int Mem;
            while (true)
            {
                cpucounter.NextValue();
                Thread.Sleep(500);
                Cpu = (int)cpucounter.NextValue();

                #region CpuColor
                if ((Cpu >= 0) && (Cpu < 20) && (ChekColor[0] != 1))
                {
                    SendMessage("setcolor:1-0,178,22;2-0,178,22;3-0,178,22;4-0,178,22;5-0,178,22;");
                    ChekColor[0] = 1;
                }
                else
                    if ((Cpu >= 20) && (Cpu < 40) && (ChekColor[0] != 2))
                    {
                        SendMessage("setcolor:1-131,236,9;2-131,236,9;3-131,236,9;4-131,236,9;5-131,236,9;");
                        ChekColor[0] = 2;
                    }
                    else
                        if ((Cpu >= 40) && (Cpu < 60) && (ChekColor[0] != 3))
                        {
                            SendMessage("setcolor:1-254,244,0;2-254,244,0;3-254,244,0;4-254,244,0;5-254,244,0;");
                            ChekColor[0] = 3;
                        }
                        else
                            if ((Cpu >= 60) && (Cpu < 80) && (ChekColor[0] != 4))
                            {
                                SendMessage("setcolor:1-254,138,0;2-254,138,0;3-254,138,0;4-254,138,0;5-254,138,0;");
                                ChekColor[0] = 4;
                            }
                            else
                                if ((Cpu >= 80) && (Cpu <= 100) && (ChekColor[0] != 5))
                                {
                                    SendMessage("setcolor:1-254,0,0;2-254,0,0;3-254,0,0;4-254,0,0;5-254,0,0;");
                                    ChekColor[0] = 5;
                                }
                #endregion


                memcounter.NextValue();
                Thread.Sleep(500);
                Mem = (int)memcounter.NextValue();

                #region MemColor
                if ((Mem >= 0) && (Mem < 20) && (ChekColor[1] != 1))
                {
                    SendMessage("setcolor:10-0,178,22;9-0,178,22;8-0,178,22;7-0,178,22;6-0,178,22;");
                    ChekColor[1] = 1;
                }
                else
                    if ((Mem >= 20) && (Mem < 40) && (ChekColor[1] != 2))
                    {
                        SendMessage("setcolor:10-131,236,9;9-131,236,9;8-131,236,9;7-131,236,9;6-131,236,9;");
                        ChekColor[1] = 2;
                    }
                    else
                        if ((Mem >= 40) && (Mem < 60) && (ChekColor[1] != 3))
                        {
                            SendMessage("setcolor:10-254,244,0;9-254,244,0;8-254,244,0;7-254,244,0;6-254,244,0;");
                            ChekColor[1] = 3;
                        }
                        else
                            if ((Mem >= 60) && (Mem < 80) && (ChekColor[1] != 4))
                            {
                                SendMessage("setcolor:10-254,138,0;9-254,138,0;8-254,138,0;7-254,138,0;6-254,138,0;");
                                ChekColor[1] = 4;
                            }
                            else
                                if ((Mem >= 80) && (Mem <= 100) && (ChekColor[1] != 5))
                                {
                                    SendMessage("setcolor:10-254,0,0;9-254,0,0;8-254,0,0;7-254,0,0;6-254,0,0;");
                                    ChekColor[1] = 5;
                                }
                #endregion

            }
        }
        #endregion

        #region Client

        private static void ReceiveMessages()
        {
            srReceiver = new StreamReader(tcpClient.GetStream());
            while (Connected)
            {
                try
                {
                    string _msg = srReceiver.ReadLine();

                    if (_msg.Contains("status:on"))
                    {
                        Console.WriteLine(_msg);
                        continue;
                    }
                    if (_msg.Contains("status:off"))
                    {
                        Console.WriteLine(_msg);
                        SendMessage("setstatus:on");
                        SendMessage("getstatus");
                        continue;
                    }
                    if (_msg.Contains("setgamma"))
                    {
                        Console.WriteLine(_msg);
                        continue;
                    }
                    if (_msg.Contains("setsmooth"))
                    {
                        Console.WriteLine(_msg);
                        continue;
                    }
                }
                catch (IOException)
                {
                }
            }
        }

        private static void SendMessage(string Msg)
        {
            swSender.WriteLine(Msg);
            swSender.Flush();
        }

        #endregion

        static void Main(string[] args)
        {
            if (args.Length == 0 || args.Length != 2)
            {
                if (args.Length > 2)
                {
                    Console.WriteLine("Переданы лишние данные!\n");
                }
                else
                {
                    Console.WriteLine("Нужные данные не переданы!\n");
                }
                Console.WriteLine("1й параметр: setgamma \n2й параметр: setsmooth");
                Console.WriteLine("Пример командной строки(ярлыка): ");
                Console.WriteLine(@"start CpuMemMonitor.exe 2.15 100" + "\n");
                Console.Write("Для выхода нажмите любую клавишу...");
                Console.ReadKey();
            }
            else
            {
                #region InitializeConnection
            Metka1:
                int chek = 0;
                try
                {
                    string IP = "127.0.0.1"; int Port = 3636;
                    tcpClient = new TcpClient();
                    tcpClient.Connect(IPAddress.Parse(IP), Port);

                    Connected = true;

                    swSender = new StreamWriter(tcpClient.GetStream());
                    thrMessaging = new Thread(new ThreadStart(ReceiveMessages));
                    thrMessaging.Start();
                }
                catch (Exception)
                {
                    Console.WriteLine("Лайтпак не запущен! Попробывать соединиться еще раз?(y/n)");
                    string temp = Console.ReadLine();
                    if (temp == "y")
                    {
                        goto Metka1;
                    }
                    else
                    {
                        if (temp == "n")
                        {
                            chek = 1;
                        }
                        else
                        {
                            chek = 1;
                            Console.WriteLine("Не корректный ввод. Приложение будет закрыто.");
                        }
                    }
                }
                #endregion

                if (chek == 0)
                {
                    SendMessage("getstatus");
                    SendMessage("lock");
                    SendMessage("setgamma:" + args[0]);
                    SendMessage("setsmooth:" + args[1]);

                    Thread.CurrentThread.CurrentCulture = new System.Globalization.CultureInfo(127);
                    cpucounter = new PerformanceCounter("Processor", "% Processor Time", "_Total");
                    memcounter = new PerformanceCounter("Memory", "% Committed Bytes In Use");
                    thrCpuMem = new Thread(new ThreadStart(CpuMem));
                    thrCpuMem.Start();
                }
            }
        }
    }
}
