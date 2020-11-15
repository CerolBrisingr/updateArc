@ECHO OFF

if exist "target" rmdir /s /q "target"

mkdir "target"
mkdir target\source
mkdir target\source\classes

xcopy /y /i ".\resources\*.dll" ".\target"
xcopy /y /i ".\main.cpp" ".\target\source"
xcopy /y /i ".\UpdateArc.pro" ".\target\source"
xcopy /y /i ".\UpdateArc.pro.user" ".\target\source"
xcopy /y /i ".\classes" ".\target\source\classes"
xcopy /y /i "..\build\release\UpdateArc.exe" ".\target"
xcopy /y /i ".\publish.bat" ".\target\source"
xcopy /y /i ".\resources\*.bat" ".\target"
xcopy /y /i ".\resources\*.ini" ".\target"