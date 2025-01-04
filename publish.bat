@ECHO OFF

if exist "target" rmdir /s /q "target"

if "%~1"=="" (set BUILD=debug) ELSE (set BUILD=%1)
set PROJECT=UpdateArc
set EXEPATH=.\build\%PROJECT%\%BUILD%


mkdir target
mkdir target\source

robocopy "." ".\target" LICENSE
robocopy ".\%PROJECT%" ".\target\source" /s /mt

robocopy ".\%PROJECT%\resources" ".\target" *.dll
robocopy %EXEPATH% ".\target" %PROJECT%.exe
robocopy "." ".\target" publish.bat

exit 0
