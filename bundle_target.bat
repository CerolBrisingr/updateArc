@ECHO OFF

:: Clear previous bundle if there is one
if exist "target" rmdir /s /q "target"

:: Read command line parameter if available
if "%~1"=="" (set BUILD=debug) ELSE (set BUILD=%1)
set PROJECT=UpdateArc
set EXEPATH=.\build\%PROJECT%\%BUILD%

:: Create target folders
mkdir target
mkdir target\source

:: Copy source data
robocopy ".\%PROJECT%" ".\target\source" /s /mt >> robocopy.txt
robocopy "." ".\target" bundle_target.bat >> robocopy.txt
robocopy "." ".\target" prepare_release.bat >> robocopy.txt

:: Copy program data
robocopy "." ".\target" LICENSE > robocopy.txt
robocopy ".\%PROJECT%\resources" ".\target" *.dll >> robocopy.txt
robocopy %EXEPATH% ".\target" %PROJECT%.exe >> robocopy.txt

echo Moving of files to target directory complete. View robocopy.txt if any issues arise.

exit 0
