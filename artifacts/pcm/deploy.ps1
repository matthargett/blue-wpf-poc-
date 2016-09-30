#$env:APPLICATION_PATH = 'E:\Projects\Git\blue-wpf-poc\artifacts\pcm'
#$PcmCommand = '.\calc.exe'

#Write-Host 'Application path: ' $env:APPLICATION_PATH 

#If (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator"))
#{   Write-Host 'Evaluating privileges....'
#    $arguments = "& '" + $myinvocation.mycommand.definition + "'"
#    Start-Process powershell -Verb runAs -ArgumentList $arguments
#    Break
#}

cd $env:APPLICATION_PATH
Start-Process powershell -ArgumentList '.\probe.ps1'

