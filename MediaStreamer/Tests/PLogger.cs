using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BJ
{
    public static class PLogger
    {


        private static string logDirectory;



        private static StreamWriter csvOutFile;

        public static void InitPLogger(string logName)
        {
            logDirectory = Settings1.Default.LogFolder;

            string csvFileName = null;

            csvFileName = logName + " " + String.Format("{0:d.M.yyyy HH-mm}", DateTime.Now) + ".csv";
            CreateReportsDirectory();
            csvOutFile = new StreamWriter(Path.Combine(logDirectory, csvFileName), true);
            WriteCSV("MediaStreamer Testing");

            WriteColumnCSV("Timestamp");
            WriteColumnCSV("RAM");
            WriteColumnCSV("Handles");
            WriteColumnCSV("GDI");
            WriteColumnCSV("Threads");

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
            Console.WriteLine("Report will be written to: {0}", logDirectory);
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

        public static void WriteCurrentStatistics()
        {
            WriteCSV("");
            WriteColumnCSV(String.Format("{0:d/M/yyyy HH:mm}", DateTime.Now));
            WriteColumnCSV(ResourceCounter.logProcessInfo("Working Set") / 1000000 + "");
            WriteColumnCSV(ResourceCounter.logProcessInfo("Handle Count") + "");
            WriteColumnCSV(ResourceCounter.GetGuiResourcesGDICount() + "");
            WriteColumnCSV(ResourceCounter.logProcessInfo("Thread Count") + "");

        }


    }
}
