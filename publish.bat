@ECHO OFF

if exist "target" rmdir /s /q "target"

if "%~1"=="" (set BUILD=debug) ELSE (set BUILD=%1)
set PROJECT=UpdateArc
set EXEPATH=.\build\%PROJECT%\%BUILD%


mkdir target
mkdir target\source

robocopy "." ".\target" LICENSE > robocopy.txt
robocopy ".\%PROJECT%" ".\target\source" /s /mt >> robocopy.txt

robocopy ".\%PROJECT%\resources" ".\target" *.dll >> robocopy.txt
robocopy %EXEPATH% ".\target" %PROJECT%.exe >> robocopy.txt
robocopy "." ".\target" publish.bat >> robocopy.txt

echo Moving of files to target directory complete. View robocopy.txt if any issues arise.

exit 0
