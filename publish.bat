@ECHO OFF

if exist "target" rmdir /s /q "target"

if "%~1"=="" (set BUILD=debug) ELSE (set BUILD=%1)
set EXEPATH=.\build\project\%BUILD%\project.exe


mkdir "target"
mkdir target\source
mkdir target\source\classes

xcopy /y /i ".\LICENSE" ".\target"

xcopy /y /i ".\project\resources\*.dll" ".\target"
xcopy /y /i ".\project\main.cpp" ".\target\source"
xcopy /y /i ".\project\mainwindow.cpp" ".\target\source"
xcopy /y /i ".\project\mainwindow.h" ".\target\source"
xcopy /y /i ".\project\mainwindow.ui" ".\target\source"
xcopy /y /i ".\project\form.cpp" ".\target\source"
xcopy /y /i ".\project\form.h" ".\target\source"
xcopy /y /i ".\project\form.ui" ".\target\source"
xcopy /y /i ".\project\project.pro" ".\target\source"
xcopy /y /i ".\project\project.pro.user" ".\target\source"
xcopy /y /i ".\project\classes" ".\target\source\classes"
xcopy /y /i ".\project\version_recognition" ".\target\source\version_recognition"
xcopy /y /i ".\project\updater" ".\target\source\updater"
xcopy /y /i ".\project\installer" ".\target\source\installer"
xcopy /y /i %EXEPATH% ".\target"
xcopy /y /i ".\publish.bat" ".\target"
