﻿using System;
using log4net.Config;
using log4net.Repository.Hierarchy;
using log4net.Layout;
using log4net.Appender;
using log4net.Core;
using System.Reflection;
using log4net.Repository;
using System.IO;

namespace BJ
{
    [AttributeUsage(AttributeTargets.Assembly)]
    public class Logger : ConfiguratorAttribute
    {      
   
            public Logger() : base(0)
            {
            }

            public override void Configure(Assembly sourceAssembly, ILoggerRepository targetRepository)
            {
                var hierarchy = (Hierarchy)targetRepository;
                var patternLayout = new PatternLayout();
                patternLayout.ConversionPattern = "%date{HH:mm:ss} [%thread] %-5level - %message%newline";
                patternLayout.ActivateOptions();

                var roller = new RollingFileAppender();
                roller.AppendToFile = false;
                roller.File = Path.Combine(Settings1.Default.LogFolder, "Log-"+ String.Format("{0:d.M.yyyy HH-mm}", DateTime.Now)+".log");
                roller.Layout = patternLayout;
                roller.MaxSizeRollBackups = 5;
                roller.MaximumFileSize = "1GB";
                roller.RollingStyle = RollingFileAppender.RollingMode.Size;
                roller.StaticLogFileName = true;
                roller.ActivateOptions();
                hierarchy.Root.AddAppender(roller);

                hierarchy.Root.Level = Level.Info;
                hierarchy.Configured = true;
            }
        }
    }
