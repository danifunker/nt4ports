@echo off
rem Build 7-Zip 9.20 for NT 4.0 PowerPC with Visual C++ 4.x, on the NT machine.
rem See ..\README.md. Usage: build <7z920 folder> <NT 4.0 headers>
rem The folder holds the patched C and CPP, with ppc\ copied in and rar5\ copied
rem to CPP\7zip\Archive\Rar5. Each result is in that component's PPC folder.
if "%2"=="" goto usage
set ROOT=%1
set INCLUDE=%ROOT%\ppc;%2;%INCLUDE%
for %%c in (Bundles\Alone Bundles\Format7zF UI\Console Archive\Rar5 UI\FileManager UI\GUI UI\Explorer Bundles\SFXCon Bundles\SFXWin) do call %ROOT%\ppc\one.bat %%c
echo BUILD FINISHED
goto end
:usage
echo usage: build 7z920folder nt4headers
:end
