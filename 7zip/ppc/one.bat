@echo off
rem one <component>: clean and build one component, then say whether it worked.
cd /d %ROOT%\CPP\7zip\%1
if exist PPC\*.obj del /q PPC\*.obj
nmake /nologo CPU=PPC MY_STATIC_LINK=1
if errorlevel 1 echo FAILED %1
if not errorlevel 1 echo BUILT %1
