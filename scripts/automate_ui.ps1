# PowerShell UI Automation Script for MfcNews

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

Write-Output "Starting MfcNews.exe..."
$process = Start-Process -FilePath "build\Release\MfcNews.exe" -PassThru

# Wait for the app to initialize and fetch default BBC/NASA feeds
Write-Output "Waiting for initial feed updates (12s)..."
Start-Sleep -Seconds 12

# Bring application to the foreground
$wshell = New-Object -ComObject Wscript.Shell
$wshell.AppActivate($process.Id)
Start-Sleep -Seconds 2

# Screenshot Helper
function Take-Screenshot($fileName) {
    $screen = [System.Windows.Forms.Screen]::PrimaryScreen
    $bitmap = New-Object System.Drawing.Bitmap $screen.Bounds.Width, $screen.Bounds.Height
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.CopyFromScreen($screen.Bounds.X, $screen.Bounds.Y, 0, 0, $bitmap.Size)
    
    # Ensure screenshots directory exists
    $dir = "screenshots"
    if (!(Test-Path -Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    
    $bitmap.Save("$dir\$fileName", [System.Drawing.Imaging.ImageFormat]::Png)
    $graphics.Dispose()
    $bitmap.Dispose()
    Write-Output "Screenshot saved: $dir\$fileName"
}

# 1. Capture the initial state (showing feed items fetched by BBC and NASA)
Write-Output "Capturing initial state..."
Take-Screenshot "1_initial_load.png"

# 2. Select the first article to display details in the right pane
Write-Output "Selecting the first article..."
[System.Windows.Forms.SendKeys]::SendWait("{TAB}") # Focus list view
Start-Sleep -Milliseconds 200
[System.Windows.Forms.SendKeys]::SendWait("{DOWN}") # Move selection to first item
Start-Sleep -Seconds 2
Take-Screenshot "2_article_selected.png"

# 3. Open the Add Feed dialog using our new Ctrl+A shortcut
Write-Output "Opening Add Feed dialog..."
[System.Windows.Forms.SendKeys]::SendWait("^a") # Send Ctrl+A
Start-Sleep -Seconds 2
Take-Screenshot "3_add_feed_dialog.png"

# 4. Fill in a new feed title and URL, then press Enter to submit
Write-Output "Entering custom feed info..."
# Title: CNBC RSS, URL: https://search.cnbc.com/rs/search/all/view.rss
[System.Windows.Forms.SendKeys]::SendWait("CNBC RSS{TAB}https://search.cnbc.com/rs/search/all/view.rss{ENTER}")
Write-Output "Waiting for feed update and reload (10s)..."
Start-Sleep -Seconds 10

# 5. Capture the view after refreshing with the new feed items merged
Take-Screenshot "4_new_feed_merged.png"

# 6. Close the application gracefully
Write-Output "Closing application..."
$process.CloseMainWindow() | Out-Null
Start-Sleep -Seconds 2

# Force terminate if still running
Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue

Write-Output "Automation script finished."
