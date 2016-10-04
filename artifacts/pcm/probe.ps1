#The script launches PCM counters and stores probe results in CSV file.
#ver 1.0.1

Write-Host -NoNewLine 'Waiting for AUT '
$passedSec = 0;
$proc = $null

do {
    Start-Sleep -Seconds 2
    $proc = Get-Process MediaStreamer.UI -ErrorAction SilentlyContinue
    $passedSec ++;
    Write-Host -NoNewline '.'
    
    if($passedSec -eq 30) {
        Write-Host ' terminating!'
        Write-Host 'Was not able to find MediaStreamer.UI'
        exit
    }

}while (-Not $proc)

Write-Host ' found!'
Write-Host 'Probing started...'

cd $env:APPLICATION_PATH

$PcmCommand = '.\pcm.exe'

#will make samples each 0.5 secs during 80 iteration (40 sec in total)
& "$PcmCommand" 0.5 -i=8 -csv='pcm_results.csv'

Write-Host 'Probing done'
Write-Host 'Analyzing'

$ParserCommand = '.\pcmcsvparse.exe'
& "$ParserCommand" -f pcm_results.csv -p L2MISS 5.5  -p L3MISS 5.5

if ($?) {
    Write-Host 'Profiling succeeded!'
    $testsPassed = $true
    $mailSubject = "Profiling succeeded!"
} else {
    Write-Host 'Profiling failed!'
    $testsPassed = $false
    $mailSubject = "Profiling failed!"
}


$projectID = $env:APPVEYOR_PROJECT_ID
$projectName = $env:APPVEYOR_PROJECT_NAME
$accountName = $env:APPVEYOR_ACCOUNT_NAME
$buildID = $env:APPVEYOR_BUILD_ID
$buildNumber = $env:APPVEYOR_BUILD_NUMBER
$buildVersion = $env:APPVEYOR_BUILD_VERSION
$projectSlug = $env:APPVEYOR_PROJECT_SLUG
$jobID = $env:APPVEYOR_JOB_ID
$jobName = $env:APPVEYOR_JOB_NAME
$committer = $env:APPVEYOR_REPO_COMMIT_AUTHOR
$committerEmail = $env:APPVEYOR_REPO_COMMIT_AUTHOR_EMAIL
$commitID = $env:APPVEYOR_REPO_COMMIT
$commitMessage = $env:APPVEYOR_REPO_COMMIT_MESSAGE
$commitTimestamp = $env:APPVEYOR_REPO_COMMIT_TIMESTAMP
$repository = $env:APPVEYOR_REPO_NAME
$repository = $repository.Replace(".git", "")

$token = 'j3cq3nvgd82v4306pb8x'
$headers = @{
  "Authorization" = "Bearer $token"
  "Content-type" = "application/json"
}

$AppveyorHost = "https://ci.appveyor.com"
$APIdeploymentString = $AppveyorHost+"/api/projects/"+$accountName+"/"+$projectSlug+"/deployments"
$usersString = $AppveyorHost+"/api/users"
$collaboratorsString = $AppveyorHost+"/api/collaborators"
$deploymentString = $AppveyorHost+"/project/"+$accountName+"/"+$projectSlug+"/deployment/"

$users = Invoke-RestMethod -Uri $usersString -Headers $headers -Method Get
$collaborators = Invoke-RestMethod -Uri $collaboratorsString -Headers $headers -Method Get
$deployments = Invoke-RestMethod -Uri $APIdeploymentString -Headers $headers -Method Get

$deploymentID

foreach($deployment in $deployments.deployments)
{
    foreach($job in $deployment.deployment.jobs)
   {
        If ($job.jobId -eq $jobID)
        {
            $deploymentID = $deployment.deployment.deploymentId
            break
        }
    }
    break
}

$deploymentLink = $deploymentString+$deploymentID+"/job/"+$jobID

Write-Host 'Deployment link: ' $deploymentLink

#SMTP server name
$smtpAddress = “smtp.lohika.com”
$EmailFrom = "bluejeans_ci@lohika.com"
$EmailFromPass = "eRgaq7uUh2o4u63wDjN"
$EmailAlias = "CI Reporter"

#Creating a Mail object
$msg = new-object Net.Mail.MailMessage

#Creating SMTP server object
$smtp = new-object Net.Mail.SmtpClient($smtpAddress, 25)
$smtp.EnableSsl = $true


$smtp.Credentials = New-Object System.Net.NetworkCredential($EmailFrom.Split("@")[0], $EmailFromPass);

$bodyString
$finalCommitPath = $repository+"/commit/"+$commitID
$shortCommitId = $commitID.Substring(0,10)

If ($testsPassed -eq $true)
{
    $bodyString = "<span><a style='font-size:15pt;font-family:Segoe UI;color:#078DC7' href="+$deploymentLink+">Deployment "+$projectName + " "+$buildVersion+ " completed"+"</a>"+"</span><br/><br/>"
}
Else
{
    $bodyString = "<span><a style='font-size:15pt;font-family:Segoe UI;color:red' href="+$deploymentLink+">Deployment "+$projectName + " "+$buildVersion+ " completed"+"</a>"+"</span><br/><br/>"
}

If ($committerEmail -eq $null)
{
    $bodyString += "<span style='color:darkgray;font-size:10pt;font-family:Segoe UI'>Commit "+"<a href="+$finalCommitPath+">$shortCommitId</a> 
    by Unknown committer on "+$commitTimestamp+"</span><br/>
    <span style='font-size:11pt;font-family:Segoe UI;color:black'>Commit message: "+$commitMessage+"</span>"
}
Else
{
    $bodyString += "<span style='color:darkgray;font-size:11pt'>Commit "+"<a href="+$finalCommitPath+">$shortCommitId</a> 
    by <a href='mailto:$committerEmail'>$committer</a> on "+$commitTimestamp+"</span><br/>
    <span style='font-size:11pt;font-family:Segoe UI;color:black'>Commit message: "+$commitMessage+"</span>"
}

$bodyString+= "<br/><br/><a style='color:darkgray;font-size:8.5pt;font-family:Segoe UI;' href='https://ci.appveyor.com/notifications'>Configure your notification preferences</a>"

#Email structure 
$msg.From = New-Object mailaddress($EmailFrom, $EmailAlias)
$msg.Subject = $mailSubject
$msg.IsBodyHtml = $true
$msg.Body = $bodyString


function CollectUsers($usersCollection)
{
    foreach($user in $usersCollection)
    {
        If ($committerEmail -eq $null)
        {
            $msg.To.Add($user.email)
        }
        Else
        {
            If ($testsPassed -eq $false)
            {
                If ($user.failedDeploymentNotification -ne "none")
                {
                    $msg.To.Add($user.email)
                }
            }
            Else
            {
                If ($user.successfulDeploymentNotification -ne "none")
                {
                    $msg.To.Add($user.email)
                }
            }
        }
    }
}

CollectUsers $users
CollectUsers $collaborators

Write-Host “Sending Email”
#Sending email 
$smtp.Send($msg)

Start-Sleep -Seconds 10


