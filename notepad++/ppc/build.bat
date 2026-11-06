@echo off
rem Build Notepad++ 3.3 for NT 4.0 PowerPC with Visual C++ 4.x, on the NT machine.
rem See README.md in this folder. Run it from anywhere:
rem   build <source root> <NT 4.0 headers>
rem <source root> is the patched source with compat\ and ppc\ copied into it.
rem Output: <source root>\so\SciLexer.dll and <source root>\no\notepad++.exe
if "%2"=="" goto usage
set ROOT=%1
set NT4INC=%2
set INCLUDE=%ROOT%\compat;%NT4INC%;%INCLUDE%
set LIB=%LIB%;%ROOT%
if not exist %ROOT%\so mkdir %ROOT%\so
if not exist %ROOT%\no mkdir %ROOT%\no
del /q %ROOT%\so\*.obj
del /q %ROOT%\no\*.obj
cd /d %ROOT%
lib /nologo /def:ppc\imm32.def /machine:ppc /out:imm32.lib
if errorlevel 1 goto failed

echo /nologo /c /TP /O1 /MT /GX /W1 /DSCI_LEXER /DNDEBUG /DWIN32 /D_WIN32_WINNT=0x0400 /FInpp_vc4.h /I..\..\compat /I..\include /I..\src /I%NT4INC% /Fo%ROOT%\so\ > %ROOT%\sci.rsp
echo /nologo /c /O2 /MT /GX /W1 /DWIN32 /DNDEBUG /D_WINDOWS /D_WIN32_WINNT=0x0400 /DNPP_NT4_HEADERS /FInpp_vc4.h /FInpp_nt4compat.h /I..\..\compat /IWinControls\AboutDlg /I..\..\scintilla\include /I..\include /IWinControls /IWinControls\ImageListSet /IWinControls\OpenSaveFileDialog /IWinControls\SplitterContainer /IWinControls\StaticDialog /IWinControls\TabBar /IWinControls\ToolBar /IMISC\Process /IScitillaComponent /IMISC /IMISC\SysMsg /IWinControls\StatusBar /I. /IWinControls\StaticDialog\RunDlg /ITinyXml /IWinControls\ColourPicker /IMISC\RegExt /IWinControls\TrayIcon /IWinControls\shortcut /IWinControls\Grid /IWinControls\ContextMenu /IMISC\PluginsManager /I%NT4INC% /Fo%ROOT%\no\ > %ROOT%\npp.rsp

cd /d %ROOT%\scintilla\win32
for %%f in (..\src\AutoComplete.cxx ..\src\CallTip.cxx ..\src\CellBuffer.cxx ..\src\ContractionState.cxx ..\src\Document.cxx ..\src\DocumentAccessor.cxx ..\src\Editor.cxx ..\src\ExternalLexer.cxx ..\src\Indicator.cxx ..\src\KeyMap.cxx ..\src\KeyWords.cxx ..\src\LineMarker.cxx ..\win32\PlatWin.cxx ..\src\RESearch.cxx ..\src\PropSet.cxx ..\src\ScintillaBase.cxx ..\win32\ScintillaWin.cxx ..\src\Style.cxx ..\src\StyleContext.cxx ..\src\UniConversion.cxx ..\src\ViewStyle.cxx ..\src\XPM.cxx ..\src\LexAda.cxx ..\src\LexAPDL.cxx ..\src\LexAsm.cxx ..\src\LexAsn1.cxx ..\src\LexAU3.cxx ..\src\LexAVE.cxx ..\src\LexBaan.cxx ..\src\LexBash.cxx ..\src\LexBasic.cxx ..\src\LexBullant.cxx ..\src\LexCaml.cxx ..\src\LexCLW.cxx ..\src\LexConf.cxx ..\src\LexCPP.cxx ..\src\LexCrontab.cxx ..\src\LexCsound.cxx ..\src\LexCSS.cxx ..\src\LexEiffel.cxx ..\src\LexErlang.cxx ..\src\LexEScript.cxx ..\src\LexFlagship.cxx ..\src\LexForth.cxx ..\src\LexFortran.cxx ..\src\LexGui4Cli.cxx ..\src\LexHaskell.cxx ..\src\LexHTML.cxx ..\src\LexKix.cxx ..\src\LexLisp.cxx ..\src\LexLout.cxx ..\src\LexLua.cxx ..\src\LexMatlab.cxx ..\src\LexMetapost.cxx ..\src\LexMMIXAL.cxx ..\src\LexMPT.cxx ..\src\LexMSSQL.cxx ..\src\LexNsis.cxx ..\src\LexOthers.cxx ..\src\LexPascal.cxx ..\src\LexPB.cxx ..\src\LexPerl.cxx ..\src\LexPOV.cxx ..\src\LexPS.cxx ..\src\LexPython.cxx ..\src\LexRebol.cxx ..\src\LexRuby.cxx ..\src\LexScriptol.cxx ..\src\LexSmalltalk.cxx ..\src\LexSpecman.cxx ..\src\LexSQL.cxx ..\src\LexTADS3.cxx ..\src\LexTeX.cxx ..\src\LexUser.cxx ..\src\LexVB.cxx ..\src\LexVerilog.cxx ..\src\LexVHDL.cxx ..\src\LexYAML.cxx) do call %ROOT%\ppc\cc.bat sci %%f
rc /fo%ROOT%\so\ScintRes.res ScintRes.rc
if errorlevel 1 goto failed
if exist %ROOT%\so\ScintillaWinS.obj del %ROOT%\so\ScintillaWinS.obj
link /nologo /DLL /DEF:Scintilla.def /OUT:%ROOT%\so\SciLexer.dll %ROOT%\so\*.obj %ROOT%\so\ScintRes.res kernel32.lib user32.lib gdi32.lib imm32.lib ole32.lib uuid.lib
if errorlevel 1 goto failed

cd /d %ROOT%\PowerEditor\src
for %%f in (WinControls\AboutDlg\AboutDlg.cpp WinControls\Grid\BabyGrid.cpp WinControls\Grid\BabyGridWrapper.cpp WinControls\ColourPicker\ColourPicker.cpp WinControls\ColourPicker\ColourPopup.cpp WinControls\TabBar\ControlsTab.cpp ScitillaComponent\DocTabView.cpp WinControls\OpenSaveFileDialog\FileDialog.cpp ScitillaComponent\FindReplaceDlg.cpp ScitillaComponent\GoToLineDlg.cpp WinControls\ImageListSet\ImageListSet.cpp Notepad_plus.cpp Parameters.cpp MISC\PluginsManager\PluginsManager.cpp ScitillaComponent\Printer.cpp MISC\Process\Process.cpp MISC\RegExt\regExtDlg.cpp WinControls\StaticDialog\RunDlg\RunDlg.cpp ScitillaComponent\ScintillaEditView.cpp WinControls\shortcut\shortcut.cpp WinControls\Grid\ShortcutMapper.cpp WinControls\SplitterContainer\Splitter.cpp WinControls\SplitterContainer\SplitterContainer.cpp WinControls\StaticDialog\StaticDialog.cpp WinControls\StatusBar\StatusBar.cpp MISC\SysMsg\SysMsg.cpp WinControls\TabBar\TabBar.cpp TinyXml\tinystr.cpp TinyXml\tinyxml.cpp TinyXml\tinyxmlerror.cpp TinyXml\tinyxmlparser.cpp WinControls\ToolBar\ToolBar.cpp WinControls\TrayIcon\trayIconControler.cpp WinControls\AboutDlg\URLCtrl.cpp ScitillaComponent\UserDefineDialog.cpp Utf8_16.cpp winmain.cpp WinControls\ColourPicker\WordStyleDlg.cpp ..\..\compat\npp_ministl.cpp) do call %ROOT%\ppc\cc.bat npp %%f
set P=%ROOT%\PowerEditor\src
set INCLUDE=%INCLUDE%;%P%;%P%\WinControls;%P%\WinControls\ColourPicker;%P%\ScitillaComponent;%P%\MISC\RegExt;%P%\WinControls\StaticDialog\RunDlg;%P%\WinControls\shortcut;%P%\WinControls\Grid;%P%\WinControls\StaticDialog;%P%\WinControls\TabBar;D:\dump\npp33\scintilla\include
rc /d __GNUC__ /d NDEBUG /fo%ROOT%\no\Notepad_plus.res npp_nt4.rc
if errorlevel 1 goto failed
link /nologo /subsystem:windows /OUT:%ROOT%\no\notepad++.exe %ROOT%\no\*.obj %ROOT%\no\Notepad_plus.res comctl32.lib shell32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib kernel32.lib
if errorlevel 1 goto failed
echo BUILD OK
goto end
:failed
echo BUILD FAILED
goto end
:usage
echo usage: build sourceroot nt4headers
:end
