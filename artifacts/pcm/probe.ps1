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
& "$PcmCommand" 0.5 -i=80 -csv='pcm_results.csv'

Write-Host 'Probing done'
