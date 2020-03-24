using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;
using System.Threading;
using Microsoft.Win32.TaskScheduler;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace TaskTest
{
    class Program
    {
        static void Main(string[] args)
        {
            connectionMqtt();
        }
        static void connectionMqtt () 
        {
            // Connection
            MqttClient mqttClient = new MqttClient("167.205.7.226");
            mqttClient.MqttMsgPublishReceived += client_receivedMessage;

            string clientId = Guid.NewGuid().ToString();

            mqttClient.Connect(clientId, "/homeauto:homeauto", "homeauto12345!");

            if (mqttClient.IsConnected)
            {
                Console.WriteLine("Subscribe Setting: setting_receiver  ");
                // Subscribe From Topic
                mqttClient.Subscribe(new String[] { "setting_receiver" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE });

            }
            else
            {
                var connected = mqttClient.IsConnected;
                while (!mqttClient.IsConnected)
                {
                    try
                    {
                        mqttClient.Connect(clientId);
                    }
                    catch
                    {

                        Console.WriteLine("Error Connection: ");
                    }
                    connected = mqttClient.IsConnected;
                }
            }
           

        }

        static void client_receivedMessage(object sender, MqttMsgPublishEventArgs e)
        {
            /*testPublish02#XXXX:XXXX:XXXX:XXXX#1000101#4000#2#24#1#11#13#daily*/
            /*testPublish02#XXXX:XXXX:XXXX:XXXX#on#1#0#0#11#13#daily#days*/

            // Split Message From RMQ
            string message = System.Text.Encoding.Default.GetString(e.Message);

            string taskIdName = message.Split("#")[0];
            string sendToMac = message.Split("#")[1];
            string settingCommand = message.Split("#")[2];
            string durationCommand = message.Split("#")[3]; //perintah waktu ke device
            short repeatDaily = short.Parse(message.Split("#")[4]); // untuk mengulangi task tersebut berapa menit/jam sekali // recure every
            int repeatDuration = int.Parse( message.Split("#")[5]); // untuk durasi berapa jam // for a duration of
            int repeatInterval = int.Parse(message.Split("#")[6]); // repeat task every
            double hoursStart = double.Parse(message.Split("#")[7]); // pemulaian jam?
            double minutesStart = double.Parse(message.Split("#")[8]); // pemulian task menit?
            string triggerCommand = message.Split("#")[9]; //
            string jadwal = message.Split("#")[10]; // 

            // Switch Case By Selection
            switch (triggerCommand)
            {
                case "onetime":
                    TaskOneTime(taskIdName, sendToMac, settingCommand, durationCommand, repeatDaily, repeatDuration, repeatInterval, hoursStart, minutesStart,jadwal);
                    break;
                case "daily":
                    TaskDaily(taskIdName, sendToMac, settingCommand, durationCommand, repeatDaily, repeatDuration, repeatInterval, hoursStart, minutesStart,jadwal);
                    break;
                case "weekly":
                    string Days = message.Split("#")[11];
                    TaskWeekly(taskIdName, sendToMac, settingCommand, durationCommand, repeatDaily, repeatDuration, repeatInterval, hoursStart, minutesStart, jadwal, Days);
                    break;
                case "monthly":
                    string DaysMonthly = message.Split("#")[11];
                    TaskMonthly(taskIdName, sendToMac, settingCommand, durationCommand, repeatDaily, repeatDuration, repeatInterval, hoursStart, minutesStart, jadwal, DaysMonthly);
                    break;
            }

            System.Console.WriteLine("Message Received: " + message);
        }

        /*----------------------------------------Function Task-----------------------------------------------------*/

        // Make New Task Function
        static TaskDefinition NewTaskService(String idRegister,String mac_send, String command_send, String durationcommand_send, String jadwal_send)
        {
            TaskDefinition td = TaskService.Instance.NewTask();
            td.RegistrationInfo.Description = "Schedule Pertanian IoT";
            td.Principal.LogonType = TaskLogonType.InteractiveToken;
      
            td.Actions.Add(new ExecAction("\"C:\\Program Files\\nodejs\\node.exe\"", "\"C:\\Users\\Nurman\\Documents\\GitHub\\homeauto_v2\\Worker\\WorkerMQTT\\app.js\"" + " --mac " + mac_send + " --command " + command_send + " --duration " + durationcommand_send + " --jadwal " + jadwal_send, null));

            return td;
        }

        // Register Task To Task Scheduler
        static void RegisterTask(string idRegister, TaskDefinition td)
        {
            // Folder Scheduller
            TaskFolder tf = TaskService.Instance.RootFolder;
            TaskFolder tfs = tf.SubFolders["Pertanian_Scheduler"];

            // Register the task in the root folder
            tfs.RegisterTaskDefinition(idRegister, td);
        }


        /*----------------------------------------Function Schedule-----------------------------------------------------*/
        /*
            RuleFunction
            1. Calling Function NewTaskService
  
            2. Add Trigger Function (TimeTrigger, Daily, Weekly, Monthly)
  
            3. Register Task Function
             
        */

        // Send Schedule To Task Scheduller By One Time
        static void TaskOneTime(string taskIdName, string sendToMac, string settingCommand,string durationCommand, short repeatDaily, int repeatDuration, int repeatInterval, double hoursStart, double minutesStart,string jadwal)
        {
            // Call Function NewTaskService
            TaskDefinition td = NewTaskService(taskIdName, sendToMac, settingCommand, durationCommand,jadwal);

            // Add a trigger that will fire the task at this time
            TimeTrigger dt = (TimeTrigger)td.Triggers.Add(new TimeTrigger()
            {
                StartBoundary = DateTime.Today + TimeSpan.FromHours(hoursStart) + TimeSpan.FromMinutes(minutesStart)
            });

            dt.Repetition.Duration = TimeSpan.FromHours(repeatDuration);
            dt.Repetition.Interval = TimeSpan.FromHours(repeatInterval);

            // Register Daily Task
            RegisterTask(taskIdName, td);
        }


        // Send Schedule To Task Scheduller By Daily Task
        static void TaskDaily(string taskIdName, string sendToMac, string settingCommand,string durationCommand, short repeatDaily, int repeatDuration, int repeatInterval,double hoursStart ,double minutesStart, string jadwal)
        {
            TaskDefinition td = NewTaskService(taskIdName, sendToMac, settingCommand, durationCommand, jadwal);
            DailyTrigger dt = (DailyTrigger)td.Triggers.Add(new DailyTrigger(repeatDaily)
            {
                StartBoundary = DateTime.Today + TimeSpan.FromHours(hoursStart) + TimeSpan.FromMinutes(minutesStart)
            });

            dt.Repetition.Duration = TimeSpan.FromHours(repeatDuration);
            dt.Repetition.Interval = TimeSpan.FromHours(repeatInterval);

            RegisterTask(taskIdName, td);
        }


        // Send Schedule Task To Scheduller By Weekly Task
        static void TaskWeekly(string taskIdName, string sendToMac, string settingCommand,string durationCommand, short repeatDaily, int repeatDuration, int repeatInterval, double hoursStart, double minutesStart, string arrayDays, string jadwal)
        {
            // Contoh data harian
            // String[] weekDays = new string[2] { "Monday", "Tuesday" };
                   
            List<String> weekDays = convertString(arrayDays);
            String[] days = weekDays.ToArray();

            TaskDefinition td = NewTaskService(taskIdName, sendToMac, settingCommand, durationCommand,  jadwal);

            WeeklyTrigger dt = (WeeklyTrigger)td.Triggers.Add(new WeeklyTrigger
            {
                StartBoundary = DateTime.Today + TimeSpan.FromHours(hoursStart) + TimeSpan.FromMinutes(minutesStart),
                DaysOfWeek = (DaysOfTheWeek) weeklyDateConverter(days)
            });

            dt.Repetition.Duration = TimeSpan.FromHours(repeatDuration);
            dt.Repetition.Interval = TimeSpan.FromHours(repeatInterval);

            RegisterTask(taskIdName, td);
        }


        // Send Schedule Task To Scheduller By Monthly
        static void TaskMonthly(string taskIdName, string sendToMac, string settingCommand, string durationCommand, short repeatDaily, int repeatDuration, int repeatInterval, double hoursStart, double minutesStart, string arrayDays, string jadwal)
        {
            TaskDefinition td = NewTaskService(taskIdName, sendToMac, settingCommand, durationCommand,  jadwal);

            List<int> weekDays = convertInt(arrayDays);
            int[] days = weekDays.ToArray();

            // td.Triggers.Add(new MonthlyTrigger { StartBoundary = Convert.ToDateTime(date_and_time), ExecutionTimeLimit = time, MonthsOfYear = MonthsOfTheYear.AllMonths, RunOnLastDayOfMonth = true });
            int[] daysTriggerMonth = new int[] { 1, 5, 7, 8, 20 };
            
            MonthlyTrigger dt = (MonthlyTrigger)td.Triggers.Add(new MonthlyTrigger
            {
                StartBoundary = DateTime.Today + TimeSpan.FromHours(hoursStart) + TimeSpan.FromMinutes(minutesStart),
                DaysOfMonth = days,
                MonthsOfYear = MonthsOfTheYear.AllMonths,
                RunOnLastDayOfMonth = true
            }) ;

            dt.Repetition.Duration = TimeSpan.FromHours(repeatDuration);
            dt.Repetition.Interval = TimeSpan.FromHours(repeatInterval);

            RegisterTask(taskIdName, td);
        }

        // Setting Days For Weekly And Monthly
        static DaysOfTheWeek? weeklyDateConverter(String[] dateArray)
        {
            String[] days = dateArray;

            DaysOfTheWeek? dow = null;

            foreach (var day in days)
            {
                DaysOfTheWeek dayValue;
                if (Enum.TryParse(day, true, out dayValue))
                {
                    dow = dow == null ? dayValue : dow | dayValue;
                }
            }
            return dow;
        }

        static List<String> convertString(string data)
        {
            string[] days = data.Split(',');

            List<string> day = new List<String>();

            foreach (string rawDays in days)
            {
                day.Add(rawDays);
            }

            return day;
        }

        static List<int> convertInt(string data)
        {
            string[] days = data.Split(',');

            List<int> day = new List<int>();

            foreach (string word in days)
            {
                int convertToInt = int.Parse(word);
                day.Add(convertToInt);
            }

            return day;
        }
    }
}