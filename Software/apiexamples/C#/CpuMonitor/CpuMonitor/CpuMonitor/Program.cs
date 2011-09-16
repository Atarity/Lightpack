using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace CpuMonitor
{
    class Program
    {

        private static StreamWriter swSender;
        private static StreamReader srReceiver;
        private static TcpClient tcpClient;
        private static Thread thrMessaging;
        private static bool Connected;

        private static PerformanceCounter cpucounter;
        private static Thread thrCpu;

        #region Cpu

        public static void Cpu()
        {
            int Cpu;
            float percent = 2.55f;
            while (true)
            {
                cpucounter.NextValue();
                Thread.Sleep(500);
                Cpu = (int)cpucounter.NextValue();
                string y1 = ((int)(((float)Cpu) * percent)).ToString();
                string y2 = (255 - (int)(((float)Cpu) * percent)).ToString();
                SendMessage("setcolor:1-" + y1 + ",128," + y2 + ";2-" + y1 + ",128," + y2 + ";3-" + y1 + ",128," + y2 + ";4-" + y1 + ",128," + y2 + ";5-" + y1 + ",128," + y2 + ";6-" + y1 + ",128," + y2 + ";7-" + y1 + ",128," + y2 + ";8-" + y1 + ",128," + y2 + ";9-" + y1 + ",128," + y2 + ";10-" + y1 + ",128," + y2 + ";");
                Thread.Sleep(2000);
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
                Console.WriteLine(@"start CpuMonitor.exe 2.15 100" + "\n");
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
                    thrCpu = new Thread(new ThreadStart(Cpu));
                    thrCpu.Start();
                }
            }
        }
    }
}
