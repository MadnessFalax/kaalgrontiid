# PowerShell Script to delete content in .\test\output\ folder but keep the folder itself
# And also delete files with .hdr and .dat extensions in .\ directory

$folderPath = ".\test\output\"
$extensionsToDelete = @("*.hdr", "*.dat")

# Delete files with .hdr and .dat extensions in the current directory (.\)
foreach ($ext in $extensionsToDelete) {
    $filesToDelete = Get-ChildItem -Path . -Filter $ext -File
    foreach ($file in $filesToDelete) {
        Remove-Item -Path $file.FullName -Force
        Write-Host "Deleted file: $($file.FullName)"
    }
}
Write-Host "All .hdr and .dat files have been deleted."

# Check if the folder exists
if (Test-Path -Path $folderPath) {
    # Delete all files and subdirectories inside the folder, but keep the folder itself
    $filesInOutput = Get-ChildItem -Path $folderPath -Recurse -File
    foreach ($file in $filesInOutput) {
        Remove-Item -Path $file.FullName -Force
        Write-Host "Deleted file: $($file.FullName)"
    }
    Write-Host "All contents in '$folderPath' have been deleted."
} else {
    Write-Host "The folder '$folderPath' does not exist."
}
