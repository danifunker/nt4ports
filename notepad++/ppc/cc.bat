@echo off
rem cc <sci|npp> <file>: compile one file, report a failure.
cl @%ROOT%\%1.rsp %2
if errorlevel 1 echo COMPILE FAILED %2
