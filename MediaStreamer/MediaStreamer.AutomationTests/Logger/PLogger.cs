using MediaStreamer.AutomationTests.Helpers;
using System;
using System.IO;

namespace MediaStreamer.AutomationTests.Logger
{
    public static class PLogger
    {


        private static string logDirectory;



        private static StreamWriter csvOutFile;

        public static void InitPLogger(string logName)
        {
            logDirectory = Settings.Default.LogFolder;

            string csvFileName = null;

            csvFileName = logName + " " + String.Format("{0:d.M.yyyy HH-mm}", DateTime.Now) + ".csv";
            CreateReportsDirectory();
            csvOutFile = new StreamWriter(Path.Combine(logDirectory, csvFileName), true);
            
            WriteColumnCSV("Timestamp");
            WriteColumnCSV("RAM");
            WriteColumnCSV("Handles");
            WriteColumnCSV("GDI");
            WriteColumnCSV("Threads");
            WriteColumnCSV("SOW RAM");
            WriteColumnCSV("SOW Handles");
            WriteColumnCSV("SOW GDI");
            WriteColumnCSV("SOW Threads");

        }


        public static string LogDirectory
        {
            get { return logDirectory; }
        }

        private static void CreateReportsDirectory()
        {
            if (!Directory.Exists(logDirectory))
            {
                Directory.CreateDirectory(logDirectory);
            }
            Console.WriteLine($"Report will be written to: {logDirectory}");
        }


        public static void WriteCSV(string line)
        {
            csvOutFile.WriteLine(line);
            csvOutFile.Flush();
        }
        public static void WriteColumnCSV(string line)
        {
            csvOutFile.Write(line + ",");
            csvOutFile.Flush();
        }
        public static void Close()
        {
            csvOutFile.Close();
        }

        public static void WriteCurrentStatistics(string message)
        {
            WriteCSV("");
            WriteColumnCSV(string.Format("{0:HH:mm:ss}", DateTime.Now));
            WriteColumnCSV(ResourceCounter.logProcessInfo("Working Set") / 1000000 + "");
            WriteColumnCSV(ResourceCounter.logProcessInfo("Handle Count") + "");
            WriteColumnCSV(ResourceCounter.GetGuiResourcesGDICount() + "");
            WriteColumnCSV(ResourceCounter.logProcessInfo("Thread Count") + "");
            WriteColumnCSV("400");
            WriteColumnCSV("3000");
            WriteColumnCSV("500");
            WriteColumnCSV("150");

            AutHelper.logLog(message + ResourceCounter.getCpuAndRamUsage());
        }


    }
}
