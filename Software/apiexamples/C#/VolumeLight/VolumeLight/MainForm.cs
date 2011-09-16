using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using libLightpack;

namespace AudioLight
{
    /// <summary>
    /// Класс формы
    /// </summary>
    public partial class MainForm : Form
    {
        /// <summary>
        /// Массив всех аудио-устройств
        /// </summary>
        NAudio.CoreAudioApi.MMDeviceEnumerator en;

        /// <summary>
        /// Устройство, на котором будет измеряться громкость
        /// </summary>
        NAudio.CoreAudioApi.MMDevice mmdevice;

        /// <summary>
        /// Объект доступа к Lightpack API
        /// </summary>
        private ApiLightpack api;

        /// <summary>
        /// Флаг соединения. Если соединение с API-сервером установлено, то флаг принимает значение истины.
        /// </summary>
        private bool connected;

        /// <summary>
        /// Host API-сервера
        /// </summary>
        private string host;

        /// <summary>
        /// Порт API-сервера
        /// </summary>
        private int port;

        /// <summary>
        /// Период чтения информации о громкости и отправки данных на API-сервер, выраженное в миллисекундах (1/1000 секунды)
        /// </summary>
        private int period;

        /// <summary>
        /// Начальная позиция обработки громкости
        /// </summary>
        private int start;

        /// <summary>
        /// Конечная позиция обработки громкости
        /// </summary>
        private int end;

        /// <summary>
        /// Массив настроек светодиодов
        /// </summary>
        private List<State> states;

        /// <summary>
        /// Конструктор формы
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Метод события до загрузки формы
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {

#region Инициализация переменных и чтение параметров из конфигурационного файла 

                this.Text = "VolumeLight " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();

                connected = false;

                states = new List<State>(10);

                for (int i = 0; i < 10; i++)
                {
                    states.Add(new State(i + 1, Color.Black));
                }

                period = 1;
                start = 5;
                end = 105;
                host = "127.0.0.1";
                port = 3636;
                
                System.IO.StreamReader sr = new System.IO.StreamReader("config");
                while (!sr.EndOfStream)
                {
                    string line = sr.ReadLine();
                    if (line.IndexOf("led") > -1)
                    {
                        int lednumber = Int32.Parse(line.Substring(3, line.IndexOf("=") - 3));
                        line = line.Substring(line.IndexOf("=") + 1);
                        int position = Int32.Parse(line.Substring(0, line.IndexOf("|")));
                        string color = line.Substring(line.IndexOf("|") + 1, line.Length - line.IndexOf("|") - 1);

                        states[lednumber - 1] = new State(position, color);
                    }
                    if (line.IndexOf("period") > -1)
                    {
                        period = Int32.Parse(line.Substring(line.IndexOf("=") + 1, line.Length - line.IndexOf("=") - 1));
                    }
                    if (line.IndexOf("start") > -1)
                    {
                        start = Int32.Parse(line.Substring(line.IndexOf("=") + 1, line.Length - line.IndexOf("=") - 1));
                    }
                    if (line.IndexOf("end") > -1)
                    {
                        end = Int32.Parse(line.Substring(line.IndexOf("=") + 1, line.Length - line.IndexOf("=") - 1));
                    }
                    if (line.IndexOf("host") > -1)
                    {
                        host = line.Substring(line.IndexOf("=") + 1, line.Length - line.IndexOf("=") - 1);
                    }
                    if (line.IndexOf("port") > -1)
                    {
                        port = Int32.Parse(line.Substring(line.IndexOf("=") + 1, line.Length - line.IndexOf("=") - 1));
                    }
                }
                sr.Close();

                en = new NAudio.CoreAudioApi.MMDeviceEnumerator();
                mmdevice = en.GetDefaultAudioEndpoint(NAudio.CoreAudioApi.DataFlow.Render, NAudio.CoreAudioApi.Role.Multimedia);
                api = new ApiLightpack();

                numericUpDown2.Value = period;
                numericUpDown3.Value = start;
                numericUpDown4.Value = end;
                textBox1.Text = host;
                numericUpDown1.Value = port;

                label_1.Text = states[0].Position.ToString();
                label_2.Text = states[1].Position.ToString();
                label_3.Text = states[2].Position.ToString();
                label_4.Text = states[3].Position.ToString();
                label_5.Text = states[4].Position.ToString();
                label_6.Text = states[5].Position.ToString();
                label_7.Text = states[6].Position.ToString();
                label_8.Text = states[7].Position.ToString();
                label_9.Text = states[8].Position.ToString();
                label_10.Text = states[9].Position.ToString();

                timer1.Interval = period;

#endregion

                //Запуск таймера
                timer1.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Сообщение: " + ex.Message + Environment.NewLine + "StackTrace: " + Environment.NewLine + ex.StackTrace, "Произошла ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }

        /// <summary>
        /// "Тик" таймера
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void timer1_Tick(object sender, EventArgs e)
        {
            //Получение значений громкости
            double L = mmdevice.AudioMeterInformation.PeakValues[0];
            double R = mmdevice.AudioMeterInformation.PeakValues[1];

            //Преобразование к целому виду
            int lcount = (int)(L * 100);
            int rcount = (int)(R * 100);

            //Установка длин индикаторов L и R, маскимальной громкостью считается значение 100 (хотя это не так, может быть и больше), минимальной -- 0
            panelL.Size = new Size((int)(lcount * 4.27), 30);
            panelR.Size = new Size((int)(rcount * 4.27), 30);

            //Попределение значения прироста для формирования зон дейтельности светодиодов
            int add = (int)((end - start) / 5);

            //Обработка состояния, если текущее значение громкости меньше start
            if (lcount < start)
            {
                //Установка цвета индикатора L, равного цвету виртуального светодиода номер 5
                //Виртуальные светодиоды располагаются 1-5 слева (1 вверху) и 6-10 справа (6 вверху)
                panelL.BackColor = states[4].Color;
            }

            
            if (lcount >= start)
            {
                //Загорание виртуального светодиода номер 5, если мы вошли в область его действия
                //Установка цвета реального и виртуального светодиодов
                SetColor(states[4].Position, 5, states[4].Color);
                //Установка цвета индикатора громкости
                panelL.BackColor = states[4].Color;
            }
            else
            {
                //Гашение виртуального светодиода номер 5, если мы не вошли в область его действия
                SetColor(states[4].Position, 5, Color.Black);
            }

            //Далее аналогично для других виртуальных светодиодов левой области, а потом и правой области
            if (lcount >= start + add)
            {
                SetColor(states[3].Position, 4, states[3].Color);
                panelL.BackColor = states[3].Color;
            }
            else
            {
                SetColor(states[3].Position, 4, Color.Black);
            }

            if (lcount >= start + 2 * add)
            {
                SetColor(states[2].Position, 3, states[2].Color);
                panelL.BackColor = states[2].Color;
            }
            else
            {
                SetColor(states[2].Position, 3, Color.Black);
            }

            if (lcount >= start + 3 * add)
            {
                SetColor(states[1].Position, 2, states[1].Color);
                panelL.BackColor = states[1].Color;
            }
            else
            {
                SetColor(states[1].Position, 2, Color.Black);
            }

            if (lcount >= start + 4 * add)
            {
                SetColor(states[0].Position, 1, states[0].Color);
                panelL.BackColor = states[0].Color;
            }
            else
            {
                SetColor(states[0].Position, 1, Color.Black);
            }

            if (rcount < start)
            {
                panelR.BackColor = states[9].Color;
            }

            if (rcount >= start)
            {
                SetColor(states[9].Position, 10, states[9].Color);
                panelR.BackColor = states[9].Color;
            }
            else
            {
                SetColor(states[9].Position, 10, Color.Black);
            }

            if (rcount >= start + add)
            {
                SetColor(states[8].Position, 9, states[8].Color);
                panelR.BackColor = states[8].Color;
            }
            else
            {
                SetColor(states[8].Position, 9, Color.Black);
            }

            if (rcount >= start + 2 * add)
            {
                SetColor(states[7].Position, 8, states[7].Color);
                panelR.BackColor = states[7].Color;
            }
            else
            {
                SetColor(states[7].Position, 8, Color.Black);
            }

            if (rcount >= start + 3 * add)
            {
                SetColor(states[6].Position, 7, states[6].Color);
                panelR.BackColor = states[6].Color;
            }
            else
            {
                SetColor(states[6].Position, 7, Color.Black);
            }

            if (rcount >= start + 4 * add)
            {
                SetColor(states[5].Position, 6, states[5].Color);
                panelR.BackColor = states[5].Color;
            }
            else
            {
                SetColor(states[5].Position, 6, Color.Black);
            }
        }

        /// <summary>
        /// Установка цвета реального и виртуального светодиода
        /// </summary>
        /// <param name="indexPosition">номер реального светодиода в системе Лайтпак</param>
        /// <param name="indexLED">номер виртуального светодиода в программе (виртуальные светодиоды располагаются 1-5 слева (1 вверху) и 6-10 справа (6 вверху))</param>
        /// <param name="color">цвет</param>
        private void SetColor(int indexPosition, int indexLED, Color color)
        {
            if (connected)
            {
                try
                {
                    api.SetColor(indexPosition, color);
                }
                catch (Exception) { }
            }

            switch (indexLED)
            {
                case 1:
                    panel_1.BackColor = color;
                    break;
                case 2:
                    panel_2.BackColor = color;
                    break;
                case 3:
                    panel_3.BackColor = color;
                    break;
                case 4:
                    panel_4.BackColor = color;
                    break;
                case 5:
                    panel_5.BackColor = color;
                    break;
                case 6:
                    panel_6.BackColor = color;
                    break;
                case 7:
                    panel_7.BackColor = color;
                    break;
                case 8:
                    panel_8.BackColor = color;
                    break;
                case 9:
                    panel_9.BackColor = color;
                    break;
                case 10:
                    panel_10.BackColor = color;
                    break;
            }

        }

        /// <summary>
        /// Обработчик нажатия кнопки Connect/Disconnect
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void button1_Click(object sender, EventArgs e)
        {
            //Вызов метода изменения состояния кнопки и соединения
            ConnectChanged();
        }

        /// <summary>
        /// Метод изменения состояния кнопки Connect/Disconnect и соединения
        /// </summary>
        private void ConnectChanged()
        {
            try
            {
                if (button1.Text == "Connect")
                {
                    host = textBox1.Text;
                    port = (int)numericUpDown1.Value;
                    api.Host = host;
                    api.Port = port;
                    if (api.Connect())
                    {
                        label15.Text = "Version API: " + api.Version + ", Status: " + api.GetStatus().ToString();
                        button1.Text = "Disconnect";
                        connectMenuItem.Text = "Disconnect";
                        string[] list = api.GetProfiles();
                        listBox1.Items.Clear();
                        foreach (string s in list)
                        {
                            listBox1.Items.Add(s);
                        }
                        connected = true;
                    }
                }
                else
                {
                    label15.Text = "---";
                    connected = false;
                    api.UnLock();
                    api.Disconnect();
                    button1.Text = "Connect";
                    button2.Text = "Lock";
                    connectMenuItem.Text = "Connect";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Сообщение: " + ex.Message + Environment.NewLine + "StackTrace: " + Environment.NewLine + ex.StackTrace, "Произошла ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Обработчик двойного клика по списку профилей, выбирает профиль
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                if (listBox1.SelectedIndex > -1)
                {
                    if (button2.Text == "Lock")
                    {
                        api.Lock();
                        api.SetProfile(listBox1.Items[listBox1.SelectedIndex].ToString());
                        api.UnLock();
                    }
                    else
                    {
                        api.SetProfile(listBox1.Items[listBox1.SelectedIndex].ToString());
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Сообщение: " + ex.Message + Environment.NewLine + "StackTrace: " + Environment.NewLine + ex.StackTrace, "Произошла ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Обработчик нажатия кнопки Lock/UnLock
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void button2_Click(object sender, EventArgs e)
        {
            //Вызов метода изменения состояния кнопки и блокировки
            LockChanged();
        }

        /// <summary>
        /// Метод изменения состояния кнопки Lock/UnLock и блокировки
        /// </summary>
        private void LockChanged()
        {
            try
            {
                if (connected)
                {
                    if (button2.Text == "Lock")
                    {
                        api.Lock();
                        button2.Text = "UnLock";
                        lockMenuItem.Text = "UnLock";
                    }
                    else
                    {
                        api.UnLock();
                        button2.Text = "Lock";
                        lockMenuItem.Text = "Lock";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Сообщение: " + ex.Message + Environment.NewLine + "StackTrace: " + Environment.NewLine + ex.StackTrace, "Произошла ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Обработчик нажатия кнопки установки значений period, start и end 
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void button3_Click(object sender, EventArgs e)
        {
            period = (int)numericUpDown2.Value;
            start = (int)numericUpDown3.Value;
            end = (int)numericUpDown4.Value;

            timer1.Interval = period;
        }

        /// <summary>
        /// Обработчик нажатия кнопки меню выхода в контекстном меню иконки в трее
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void ExitMenuItem_Click(object sender, EventArgs e)
        {
            //Метод выхода из программы
            Exit();
        }

        /// <summary>
        /// Метод выхода из программы, обеспечивает сохранение параметров и отключение от API-сервера
        /// </summary>
        private void Exit()
        {
            if (connected)
            {
                try
                {
                    api.UnLock();
                    api.Disconnect();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Сообщение: " + ex.Message + Environment.NewLine + "StackTrace: " + Environment.NewLine + ex.StackTrace, "Произошла ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

            System.IO.StreamWriter sw = new System.IO.StreamWriter("config");
            for (int i = 0; i < 10; i++)
            {
                sw.WriteLine("led" + (i + 1) + "=" + states[i].ToSaveFormat());
            }
            sw.WriteLine("period=" + period);
            sw.WriteLine("start=" + start);
            sw.WriteLine("end=" + end);
            sw.WriteLine("host=" + host);
            sw.WriteLine("port=" + port);

            sw.Close();

            Application.Exit();
        }

        /// <summary>
        /// Обработчик нажатия кнопки меню открытия/закрытия окна в контекстном меню иконки в трее
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void CloseMenuItem_Click(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Normal)
            {
                this.WindowState = FormWindowState.Minimized;
                this.ShowInTaskbar = false;
                closeMenuItem.Text = "Open";
            }
            else
            {
                this.ShowInTaskbar = true;
                this.WindowState = FormWindowState.Normal;
                this.ShowInTaskbar = true;
                closeMenuItem.Text = "Close";
            }
        }

        /// <summary>
        /// Обработчик нажатия кнопки меню соедения/рассоединения окна в контекстном меню иконки в трее
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void connectMenuItem_Click(object sender, EventArgs e)
        {
            ConnectChanged();
        }

        /// <summary>
        /// Обработчик нажатия кнопки меню блокировки/разблокировки окна в контекстном меню иконки в трее
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void lockMenuItem_Click(object sender, EventArgs e)
        {
            LockChanged();
        }

        /// <summary>
        /// Обработчик события деактивации окна для нормального сворачивания в трей
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void Form1_Deactivate(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.ShowInTaskbar = false;
                closeMenuItem.Text = "Open";
            }
        }

        /// <summary>
        /// Обработчик события перед закрытием формы
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Exit();
        }

        /// <summary>
        /// Обработчик события двойного клика на иконку в трее, раскрывает или сворачивает в трей окно программы
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Normal)
            {
                this.WindowState = FormWindowState.Minimized;
                this.ShowInTaskbar = false;
                closeMenuItem.Text = "Open";
            }
            else
            {
                this.ShowInTaskbar = true;
                this.WindowState = FormWindowState.Normal;
                this.ShowInTaskbar = true;
                closeMenuItem.Text = "Close";
            }
        }

        /// <summary>
        /// Обработчик двойного клика на область индикации виртуального светодиода, позволяет установить цвет этого светодиода, далее аналогично для всех светодиодов
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void panel_1_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[0].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[0].Color = colorDialog1.Color;
            }
        }

        private void panel_2_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[1].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[1].Color = colorDialog1.Color;
            }
        }

        private void panel_3_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[2].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[2].Color = colorDialog1.Color;
            }
        }

        private void panel_4_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[3].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[3].Color = colorDialog1.Color;
            }
        }

        private void panel_5_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[4].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[4].Color = colorDialog1.Color;
            }
        }

        private void panel_6_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[5].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[5].Color = colorDialog1.Color;
            }
        }

        private void panel_7_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[6].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[6].Color = colorDialog1.Color;
            }
        }

        private void panel_8_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[7].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[7].Color = colorDialog1.Color;
            }
        }

        private void panel_9_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[8].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[8].Color = colorDialog1.Color;
            }
        }

        private void panel_10_DoubleClick(object sender, EventArgs e)
        {
            colorDialog1.Color = states[9].Color;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                states[9].Color = colorDialog1.Color;
            }
        }

        /// <summary>
        /// Обработчик двойного клика на номере реального светодиода в системе Лайтпак, позволяет установить этот номер, тем самым изменив привязку реального светодиода к виртуальному (маппинг), далее аналогично для всех светодиодов
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void label_1_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "1";
            numericUpDown5.Value = states[0].Position;
            panel1.Visible = true;
        }

        private void label_2_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "2";
            numericUpDown5.Value = states[1].Position;
            panel1.Visible = true;
        }

        private void label_3_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "3";
            numericUpDown5.Value = states[2].Position;
            panel1.Visible = true;
        }

        private void label_4_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "4";
            numericUpDown5.Value = states[3].Position;
            panel1.Visible = true;
        }

        private void label_5_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "5";
            numericUpDown5.Value = states[4].Position;
            panel1.Visible = true;
        }

        private void label_6_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "6";
            numericUpDown5.Value = states[5].Position;
            panel1.Visible = true;
        }

        private void label_7_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "7";
            numericUpDown5.Value = states[6].Position;
            panel1.Visible = true;
        }

        private void label_8_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "8";
            numericUpDown5.Value = states[7].Position;
            panel1.Visible = true;
        }

        private void label_9_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "9";
            numericUpDown5.Value = states[8].Position;
            panel1.Visible = true;
        }

        private void label_10_DoubleClick(object sender, EventArgs e)
        {
            label4.Text = "10";
            numericUpDown5.Value = states[9].Position;
            panel1.Visible = true;
        }

        /// <summary>
        /// Обработчик установщика позиции реального светодиода в системе Лайтпак, позволяет делать привязку реального светодиода и виртуального
        /// </summary>
        /// <param name="sender">отправитель</param>
        /// <param name="e">параметры</param>
        private void button4_Click(object sender, EventArgs e)
        {
            int led = Int32.Parse(label4.Text);
            int position = (int)numericUpDown5.Value;
            states[led - 1].Position = position;
            switch (led)
            {
                case 1:
                    label_1.Text = position.ToString();
                    break;
                case 2:
                    label_2.Text = position.ToString();
                    break;
                case 3:
                    label_3.Text = position.ToString();
                    break;
                case 4:
                    label_4.Text = position.ToString();
                    break;
                case 5:
                    label_5.Text = position.ToString();
                    break;
                case 6:
                    label_6.Text = position.ToString();
                    break;
                case 7:
                    label_7.Text = position.ToString();
                    break;
                case 8:
                    label_8.Text = position.ToString();
                    break;
                case 9:
                    label_9.Text = position.ToString();
                    break;
                case 10:
                    label_10.Text = position.ToString();
                    break;
            }
            panel1.Visible = false;
        }
    }

/// <summary>
    /// Класс определения настройки светодиода
    /// </summary>
    public class State
    {
        /// <summary>
        /// Реальный номер светодиода в системе Лайтпак
        /// </summary>
        public int Position { get; set; }

        /// <summary>
        /// Цвет светодиода
        /// </summary>
        public Color Color { get; set; }

        /// <summary>
        /// Конструктор, принимающий номер реальной позиции светодиода в системе Лайтпак и строковое описание цвета в виде шестнадцатиричного числа RRGGBB
        /// </summary>
        /// <param name="position">номер реальной позиции светодиода в системе Лайтпак</param>
        /// <param name="color">строковое описание цвета в виде шестнадцатиричного числа RRGGBB</param>
        public State(int position, string color)
        {
            string sR = color.Substring(0, 2);
            string sG = color.Substring(2, 2);
            string sB = color.Substring(4, 2);

            int iR = Int32.Parse(sR, System.Globalization.NumberStyles.HexNumber);
            int iG = Int32.Parse(sG, System.Globalization.NumberStyles.HexNumber);
            int iB = Int32.Parse(sB, System.Globalization.NumberStyles.HexNumber);

            Color = Color.FromArgb(iR, iG, iB);

            Position = position;
        }
        /// <summary>
        /// Конструктор, принимающий номер реальной позиции светодиода в системе Лайтпак и цвет в виде стандартного класса
        /// </summary>
        /// <param name="position">номер реальной позиции светодиода в системе Лайтпак</param>
        /// <param name="color">цвет в виде стандартного класса</param>
        public State(int position, Color color)
        {
            Color = color;
            Position = position;
        }

        /// <summary>
        /// Форматированный вывод информации для сохранения данных о состоянии в конфигурационный файл
        /// </summary>
        /// <returns></returns>
        public string ToSaveFormat()
        {
            return Position + "|" + Color.R.ToString("X2") + Color.G.ToString("X2") + Color.B.ToString("X2");
        }
    }
}
