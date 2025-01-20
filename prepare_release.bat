:: Create ZIP-File for GitHub
@powershell Compress-Archive ./* UpdateArc.zip

:: Read version information of UpdateArc.exe and dump it into a file
:: Reading it into a variable is not a thing windows shell is good at
@powershell (Get-Item -Path 'UpdateArc.exe').VersionInfo.FileVersion > Version.txt

:: Read version information from file
@set /p VER= < Version.txt

:: Print version information to console
@echo Compiled program contains version number %VER%

:: Rename the version number dump to hammer things home
@ren Version.txt Version%VER%.txt