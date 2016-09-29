using System;
using System.IO;
using OfficeOpenXml;
using MediaStreamer.AutomationTests.Helpers;

namespace MediaStreamer.AutomationTests.Logger
{
    public class ExcelLogger
    {
        private static string logDirectory;
        static string fileName = null;
        static int excelRowCount = 1;


        private static void CreateReportsDirectory()
        {
            if (!Directory.Exists(logDirectory))
            {
                Directory.CreateDirectory(logDirectory);
            }
            Console.WriteLine("Report will be written to: {0}", logDirectory);
        }

        public static void InitExcelLoger(string logName)
        {
            logDirectory = Settings.Default.LogFolder;
            CreateReportsDirectory();
            fileName = logName + " " + String.Format("{0:d.M.yyyy HH-mm}", DateTime.Now) + ".xlsx";

            DirectoryInfo outputDir = new DirectoryInfo(Settings.Default.LogFolder);
            FileInfo newFile = new FileInfo(Path.Combine(outputDir.FullName, fileName));

            using (ExcelPackage package = new ExcelPackage(newFile))
            {
                var ws = package.Workbook.Worksheets.Add("Performance Test");
                ws.Cells["A" + excelRowCount].Value = "Timestamp";
                ws.Cells["B" + excelRowCount].Value = "RAM";
                ws.Cells["C" + excelRowCount].Value = "Handles";
                ws.Cells["D" + excelRowCount].Value = "GDI";
                ws.Cells["E" + excelRowCount].Value = "Threads";
                ws.Cells["F" + excelRowCount].Value = "SOW RAM";
                ws.Cells["G" + excelRowCount].Value = "SOW Handles";
                ws.Cells["H" + excelRowCount].Value = "SOW GDI";
                ws.Cells["I" + excelRowCount].Value = "SOW Threads";
                ws.Cells.AutoFitColumns();
                package.Compression = CompressionLevel.BestSpeed;
                package.Save();
            }
        }

        public static void WriteCurrentStatistics(string message)
        {
            DirectoryInfo outputDir = new DirectoryInfo(Settings.Default.LogFolder);
            FileInfo newFile = new FileInfo(Path.Combine(outputDir.FullName, fileName));

            using (ExcelPackage package = new ExcelPackage(newFile))
            {
                excelRowCount++;

                var ws = package.Workbook.Worksheets["Performance Test"];
                ws.Cells["A" + excelRowCount].Style.Numberformat.Format = "hh: mm: ss";
                ws.Cells["B" + excelRowCount].Style.Numberformat.Format = "0.###";
                ws.Cells["C" + excelRowCount + ":I" + excelRowCount].Style.Numberformat.Format = "0";
                ws.Cells["A" + excelRowCount].Value = string.Format("{0:HH:mm:ss}", DateTime.Now);
                ws.Cells["B" + excelRowCount].Value = double.Parse(ResourceCounter.logProcessInfo("Working Set") / 1000000 + "");
                ws.Cells["C" + excelRowCount].Value = double.Parse(ResourceCounter.logProcessInfo("Handle Count") + "");
                ws.Cells["D" + excelRowCount].Value = double.Parse(ResourceCounter.GetGuiResourcesGDICount() + "");
                ws.Cells["E" + excelRowCount].Value = double.Parse(ResourceCounter.logProcessInfo("Thread Count") + "");
                ws.Cells["F" + excelRowCount].Value = double.Parse("400");
                ws.Cells["G" + excelRowCount].Value = double.Parse("3000");
                ws.Cells["H" + excelRowCount].Value = double.Parse("500");
                ws.Cells["I" + excelRowCount].Value = double.Parse("150");
               // ws.Cells["B" + excelRowCount + ":I" + excelRowCount].Style.Numberformat.Format = "#,###0";
                ws.Cells.AutoFitColumns();
                package.Compression = CompressionLevel.BestSpeed;
                package.Save();
            }

            AutHelper.logLog(message + ResourceCounter.getCpuAndRamUsage());
        }


    }
}
