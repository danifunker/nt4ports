;FileZilla install script
;written by Tim Kosse (mailto:tim.kosse@gmx.de)
;Compatible with NSIS Modern UI version 1.67
;Based upon example scripts from Joost Verburg

;--------------------------------
;Include Modern UI
  
  !include "MUI.nsh"
  !include "Sections.nsh"
  !define MUI_LICENSEPAGE_BGCOLOR /grey

;--------------------------------
;Product information

  !define MUI_PRODUCT "FileZilla" ;Define your own software name here
  !define MUI_VERSION "2.2.32" ;Define your own software version here
  Name "${MUI_PRODUCT} ${MUI_VERSION}"

;--------------------------------
;Configuration

  SetCompressor /SOLID lzma

  ;General
  OutFile "../../FileZilla_setup.exe"

  SetOverwrite on
  AutoCloseWindow false
  ShowInstDetails show
  ShowUninstDetails show

  ;Folder selection page
  InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"

  InstallDirRegKey HKCU "Software\${MUI_PRODUCT}" "Install_Dir"

  ;Remember the Start Menu Folder
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${MUI_PRODUCT}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "FileZilla"

 ; !define MUI_LANGDLL_ALWAYSSHOW
  !define MUI_LANGDLL_REGISTRY_ROOT      "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY       "software\${MUI_PRODUCT}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  !define TEMP $R0
 
;--------------------------------
;Modern UI Configuration
  
  !define MUI_ICON "..\res\filezilla.ico"
  !define MUI_UNICON "uninstall.ico"

  !define MUI_ABORTWARNING

;--------------------------------
; Variables

  Var STARTMENU_FOLDER
  Var MUI_TEMP
  VAR IO_HWND
  
;--------------------------------
;Pages

  !define MUI_LICENSEPAGE_TEXT_BOTTOM "$(LicenseText)"
  !define MUI_LICENSEPAGE_BUTTON "$(LicenseNext)"
  !insertmacro MUI_PAGE_LICENSE "license.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  Page custom Settings SettingsEnd
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  ;English
  !insertmacro MUI_LANGUAGE "English"
  !include "..\LanguageDLLs\English\fz_english.nsh"

  ;Bulgarian
  !insertmacro MUI_LANGUAGE "Bulgarian"
  !include "..\LanguageDLLs\Bulgarian\fz_bulgarian.nsh"

  ;Catalan
  !insertmacro MUI_LANGUAGE "Catalan"
  !include "..\LanguageDLLs\Catalan\fz_catalan.nsh"

  ;Czech
  !insertmacro MUI_LANGUAGE "Czech"
  !include "..\LanguageDLLs\Czech\fz_czech.nsh"

  ;French
  !insertmacro MUI_LANGUAGE "French"
  !include "..\LanguageDLLs\French\fz_french.nsh"

  ;German
  !insertmacro MUI_LANGUAGE "German"
  !include "..\LanguageDLLs\German\fz_german.nsh"

  ;Hungarian
  !insertmacro MUI_LANGUAGE "Hungarian"
  !include "..\LanguageDLLs\Hungarian\fz_hungarian.nsh"

  ;Italian
  !insertmacro MUI_LANGUAGE "Italian"
  !include "..\LanguageDLLs\Italian\fz_italian.nsh"

  ;Japanese
  !insertmacro MUI_LANGUAGE "Japanese"
  !include "..\LanguageDLLs\Japanese\fz_japanese.nsh"
 
  ;Japanese
  !insertmacro MUI_LANGUAGE "Korean"
  !include "..\LanguageDLLs\Korean\fz_korean.nsh"
 
  ;Norwegian
  !insertmacro MUI_LANGUAGE "Norwegian"
  !include "..\LanguageDLLs\Norwegian\fz_norwegian.nsh"

  ;Polish
  !insertmacro MUI_LANGUAGE "Polish"
  !include "..\LanguageDLLs\Polish\fz_polish.nsh"

  ;Simplified Chinese
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !include "..\LanguageDLLs\SimplifiedChinese\fz_simplifiedchinese.nsh"

  ;Traditional Chinese
  !insertmacro MUI_LANGUAGE "TradChinese"
  !include "..\LanguageDLLs\TraditionalChinese\fz_traditionalchinese.nsh"
 
  ;Russian
  !insertmacro MUI_LANGUAGE "Russian"
  !include "..\LanguageDLLs\Russian\fz_russian.nsh"

  ;Slovak
  !insertmacro MUI_LANGUAGE "Slovak"
  !include "..\LanguageDLLs\Slovak\fz_slovak.nsh"
 
;--------------------------------
;Reserve Files

  ;Things that need to be extracted on first (keep these lines before any File command!)
  ;Only useful for BZIP2 compression
 
  !insertmacro MUI_RESERVEFILE_LANGDLL
 
  ReserveFile "settings.ini"
  !insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;--------------------------------
;Installer Sections

InstType $(InstTypeStandardName)
InstType $(InstTypeFullName)

Section !$(SecFileZillaName) SecFileZilla
SectionIn 1 2 RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "..\release\FileZilla.exe"
  
  ;Delete gssapi.dll if it belongs to an old FileZilla version
  IfFileExists "$INSTDIR\FileZilla.exe" "" +2
  Delete "$INSTDIR\GSSApi.dll"

  File "FzGSS.dll"
  File "dbghelp.dll"
  File "..\openssl\ssleay32.dll"
  File "..\openssl\libeay32.dll"
  File "..\FzSFtp\Release\FzSFtp.exe"
  File "..\..\readme.htm"
  File "..\..\GPL.html"
  File "..\..\puttylicense.html"
  File "..\..\legal.htm"
  ; Write the installation path into the registry
  WriteRegStr HKCU SOFTWARE\FileZilla "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM SOFTWARE\FileZilla "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FileZilla" "DisplayName" "FileZilla (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FileZilla" "UninstallString" '"$INSTDIR\uninstall.exe"'
 
  ;Create the uninstaller
  WriteUninstaller uninstall.exe
  
  ; Check for /secure parameter
  Call GetParameters
  Push "/secure"
  Call StrStr
  Pop $R1
  StrCpy $R1 $R1 7
  StrCmp $R1 "/secure" secure

  ; Skip "run in secure mode" question if running in silent mode
  
  IfSilent NoSecure

  !insertmacro MUI_INSTALLOPTIONS_READ $R1 settings.ini "Field 2" State
  strcmp $R1 "1" "" NoSecure
 secure:
  WriteRegStr HKLM "Software\FileZilla" "Run in Secure Mode" 1
  WriteRegStr HKCU "Software\FileZilla" "Run in Secure Mode" 1
  goto SecureDone
 NoSecure:
  WriteRegStr HKLM "Software\FileZilla" "Run in Secure Mode" 0
  WriteRegStr HKCU "Software\FileZilla" "Run in Secure Mode" 0
 SecureDone:

  ; Check for /registry parameter
  Call GetParameters
  Push "/registry"
  Call StrStr
  Pop $R1
  StrCpy $R1 $R1 9
  StrCmp $R1 "/registry" useregistry

  ; Check for /forceregistry parameter
  Call GetParameters
  Push "/forceregistry"
  Call StrStr
  Pop $R1
  StrCpy $R1 $R1 14
  StrCmp $R1 "/forceregistry" forceregistry

  ; Skip storage question if running in silent mode
  IfSilent usexml
  
  !insertmacro MUI_INSTALLOPTIONS_READ $R1 settings.ini "Field 5" State
  strcmp $R1 "1" usexml
  !insertmacro MUI_INSTALLOPTIONS_READ $R1 settings.ini "Field 7" State
  strcmp $R1 "1" forceregistry
 useregistry:
  WriteRegStr HKLM "Software\FileZilla" "Use Registry" 1
  WriteRegStr HKCU "Software\FileZilla" "Use Registry" 1
  goto storagedone
 forceregistry:
  WriteRegStr HKLM "Software\FileZilla" "Use Registry" 2
  WriteRegStr HKCU "Software\FileZilla" "Use Registry" 2
  goto storagedone
 usexml:
  WriteRegStr HKLM "Software\FileZilla" "Use Registry" 0
  WriteRegStr HKCU "Software\FileZilla" "Use Registry" 0
 storagedone:

SectionEnd

SubSection /e $(SecDocumentationName) SecDocumentation

  Section $(SecEnglishDocumentationName) SecEnglishDocumentation
    SectionIn 1 2
    SetOutPath $INSTDIR
    File "..\documentation\FileZilla.chm"
    StrCpy $1 "documentation"
  SectionEnd

SubSectionEnd

Section $(SecLangFilesName) SecLangFiles
SectionIn 1 2
  SetOutPath $INSTDIR
  File "..\LanguageDLLs\Bulgarian\release\FzResBu.dll"
  File "..\LanguageDLLs\Breton\release\FzResBr.dll"
  File "..\LanguageDLLs\Catalan\release\FzResCa.dll"
  File "..\LanguageDLLs\Czech\release\FzResCze.dll"
  File "..\LanguageDLLs\SimplifiedChinese\release\FzResChs.dll"
  File "..\LanguageDLLs\TraditionalChinese\release\FzResCh.dll"
  File "..\LanguageDLLs\German\release\FzResDe.dll"
  File "..\LanguageDLLs\Spanish\release\FzResEs.dll"
  File "..\LanguageDLLs\French\release\FzResFr.dll"
  File "..\LanguageDLLs\Hungarian\release\FzResHu.dll"
  File "..\LanguageDLLs\Italian\release\FzResIt.dll"
  File "..\LanguageDLLs\Japanese\release\FzResJp.dll"
  File "..\LanguageDLLs\Korean\release\FzResKr.dll"
  File "..\LanguageDLLs\Norwegian\release\FzResNor.dll"
  File "..\LanguageDLLs\Polish\release\FzResPo.dll"
  File "..\LanguageDLLs\Russian\release\FzResRu.dll"
  File "..\LanguageDLLs\Slovak\release\FzResSk.dll"

  ;Write language for FileZilla into the registry
  Call WriteLanguage

SectionEnd

Section $(SecStartMenuName) SecStartMenu
SectionIn 1 2

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\FileZilla.lnk" "$INSTDIR\FileZilla.exe" "" "$INSTDIR\FileZilla.exe" 0
  
    StrCmp $1 "documentation" "" +2
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\FileZilla Documentation.lnk" "$INSTDIR\FileZilla.chm" "" "$INSTDIR\FileZilla.chm" 0

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd


Section $(SecDesktopIconName) SecDesktopIcon
SectionIn 1 2
  CreateShortCut "$DESKTOP\FileZilla.lnk" "$INSTDIR\FileZilla.exe" "" "$INSTDIR\FileZilla.exe" 0
SectionEnd

Section $(SecDebugName) SecDebug
SectionIn 1 2
  SetOutPath $INSTDIR
  File ..\release\FileZilla.pdb
SectionEnd

;--------------------------------
;Installer Functions

Function .onInit

;Language selection

  !insertmacro MUI_LANGDLL_DISPLAY

  ;Init InstallOptions
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "settings.ini"

FunctionEnd

Function Settings

  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" settings NextButtonText $R0

  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 1" text $(QuestionRunInSecureMode)
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 2" text $(UseSecureMode)
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 3" text $(DontUseSecureMode)

  ReadRegStr $R0 HKCU Software\FileZilla "Run in Secure mode"
  StrCmp $R0 "1" preselectsecure
  ReadRegStr $R0 HKLM Software\FileZilla "Run in Secure mode"
  StrCmp $R0 "1" preselectsecure
  goto preselectsecure_end
 preselectsecure:
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 2" state 1
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 3" state 0
 preselectsecure_end:

  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 4" text $(StorageQuestion)
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 5" text $(UseXML)
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 6" text $(UseRegistry)
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 7" text $(ForceUseRegistry)

  ReadRegStr $R0 HKCU Software\FileZilla "Use Registry"
  StrCmp $R0 "2" preselectstorage_force
  StrCmp $R0 "1" preselectstorage
  ReadRegStr $R0 HKLM Software\FileZilla "Use Registry"
  StrCmp $R0 "2" preselectstorage_force
  StrCmp $R0 "1" preselectstorage

  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 7" flags "DISABLED"
  goto preselectstorage_end
 preselectstorage_force:
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 7" state 1
 preselectstorage:
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 6" state 1
  !insertmacro MUI_INSTALLOPTIONS_WRITE "settings.ini" "Field 5" state 0
 preselectstorage_end:

  !insertmacro MUI_HEADER_TEXT $(PageSettingsTitle) $(PageSettingsSubTitle)
  !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "settings.ini"
  Pop $IO_HWND
  !insertmacro MUI_INSTALLOPTIONS_SHOW

FunctionEnd

Function SettingsEnd

  ; At this point the user has either pressed Next or one of our custom buttons
  ; We find out which by reading from the INI file
  !insertmacro MUI_INSTALLOPTIONS_READ $R0 "settings.ini" "Settings" "State"
  StrCmp $R0 0 validate_settingsdialog ; Next button?
  StrCmp $R0 5 onchange_settingsdialog
  StrCmp $R0 6 onchange_settingsdialog
  Abort ; Return to the page

onchange_settingsdialog:
  ; Make the Force registry field depend on use registry radiobuttun
  !insertmacro MUI_INSTALLOPTIONS_READ $R0 "settings.ini" "Field 5" "State"
  GetDlgItem $R1 $IO_HWND 1206 ; Force registry checkbox

  StrCmp $R0 0 "" +3
  EnableWindow $R1 1
  goto +2
  EnableWindow $R1 0
  
  Abort ; Return to the page

validate_settingsdialog:
  ; Nothing to validate

FunctionEnd

!macro checklang RefLang LangToFind goto
  push "${Reflang}"
  push "${LangToFind}"
  call StrStr
  Pop $R1
  StrCmp $1 "" "" ${goto}
!macroend

Function WriteLanguage

  Push $R0
  Push $R1
  
  ;Checks current language set in the registry, if none is set (or one not included in the installer) set it to the current one
  StrCpy $R0 ""
 
  ;First check/set the value under HKCU
  ReadRegStr $R0 HKCU Software\FileZilla Language
  StrCmp $R0 "" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Bulgarian" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Catalan" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Czech" HKCU_SetLanguage
  !insertmacro checklang "$R0" "English" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Deutsch" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Français" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Italiano" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Hungarian" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Japanese" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Korean" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Norwegian" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Polski" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Russian" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Simplified Chinese" HKCU_SetLanguage
  !insertmacro checklang "$R0" "Traditional Chinese" HKCU_SetLanguage
  !Insertmacro checklang "$R0" "Slovak" HKCU_SetLanguage

  goto HKLM_CheckLanguage

 HKCU_SetLanguage:
  WriteRegStr HKCU Software\FileZilla Language $(FZLanguageName)
  
 HKLM_CheckLanguage:
  
  StrCpy $R0 ""
 
  ;Now check/set the value under HKLM
  ReadRegStr $R0 HKLM Software\FileZilla Language
  StrCmp $R0 "" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Bulgarian" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Catalan" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Czech" HKLM_SetLanguage
  !insertmacro checklang "$R0" "English" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Deutsch" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Français" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Italiano" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Hungarian" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Japanese" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Korean" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Norwegian" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Polski" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Russian" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Simplified Chinese" HKLM_SetLanguage
  !insertmacro checklang "$R0" "Traditional Chinese" HKLM_SetLanguage
  !Insertmacro checklang "$R0" "Slovak" HKLM_SetLanguage

  goto SetLanguageDone

 HKLM_SetLanguage:
  WriteRegStr HKLM Software\FileZilla Language $(FZLanguageName)
  
 SetLanguageDone:
  Pop $R1
  Pop $R0

FunctionEnd

; GetParameters
; input, none
; output, top of stack (replaces, with e.g. whatever)
; modifies no other variables.

Function GetParameters

  Push $R0
   Push $R1
   Push $R2
   Push $R3
   
   StrCpy $R2 1
   StrLen $R3 $CMDLINE
   
   ;Check for quote or space
   StrCpy $R0 $CMDLINE $R2
   StrCmp $R0 '"' 0 +3
     StrCpy $R1 '"'
     Goto loop
   StrCpy $R1 " "
   
   loop:
     IntOp $R2 $R2 + 1
     StrCpy $R0 $CMDLINE 1 $R2
     StrCmp $R0 $R1 get
     StrCmp $R2 $R3 get
     Goto loop
   
   get:
     IntOp $R2 $R2 + 1
     StrCpy $R0 $CMDLINE 1 $R2
     StrCmp $R0 " " get
     StrCpy $R0 $CMDLINE "" $R2
   
   Pop $R3
   Pop $R2
   Pop $R1
   Exch $R0

FunctionEnd

; StrStr
; input, top of stack = string to search for
;        top of stack-1 = string to search in
; output, top of stack (replaces with the portion of the string remaining)
; modifies no other variables.
;
; Usage:
;   Push "this is a long ass string"
;   Push "ass"
;   Call StrStr
;   Pop $R0
;  ($R0 at this point is "ass string")

Function StrStr
Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrCpy $R4 0
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len(needle)
  ; $R4=cnt
  ; $R5=tmp
 loop:
  StrCpy $R5 $R2 $R3 $R4
  StrCmp $R5 $R1 done
  StrCmp $R5 "" done
  IntOp $R4 $R4 + 1
  Goto loop
 done:
  StrCpy $R1 $R2 "" $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
FunctionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFileZilla} $(SecFileZillaDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDocumentation} $(SecDocumentationDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecEnglishDocumentation} $(SecEnglishDocumentationDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecLangFiles} $(SecLangFilesDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} $(SecStartMenuDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopIcon} $(SecDesktopIconDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDebug} $(SecDebugDesc)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller section

Section "Uninstall"

  ;Add your stuff here

  ; remove registry keys
  MessageBox MB_ICONQUESTION|MB_YESNO $(un.QuestionDeleteRegistry) IDNO NoRegDelete
  DeleteRegKey HKCU "Software\FileZilla"
  DeleteRegKey HKLM "Software\FileZilla"
 NoRegDelete:
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FileZilla"

  ; remove files
  Delete $INSTDIR\FileZilla.exe
  Delete $INSTDIR\FileZilla.chm
  Delete $INSTDIR\dbghelp.dll
  Delete $INSTDIR\FzGSS.dll
  Delete $INSTDIR\FzResBu.dll
  Delete $INSTDIR\FzResBr.dll
  Delete $INSTDIR\FzResCa.dll
  Delete $INSTDIR\FzResCh.dll
  Delete $INSTDIR\FzResChs.dll
  Delete $INSTDIR\FzResCze.dll
  Delete $INSTDIR\FzResDe.dll
  Delete $INSTDIR\FzResEs.dll
  Delete $INSTDIR\FzResFr.dll
  Delete $INSTDIR\FzResHu.dll
  Delete $INSTDIR\FzResIt.dll
  Delete $INSTDIR\FzResJp.dll
  Delete $INSTDIR\FzResKr.dll
  Delete $INSTDIR\FzResNor.dll
  Delete $INSTDIR\FzResPo.dll
  Delete $INSTDIR\FzResRu.dll
  Delete $INSTDIR\FzResSk.dll
  Delete $INSTDIR\libeay32.dll
  Delete $INSTDIR\FzSFtp.exe"
  Delete $INSTDIR\ssleay32.dll
  Delete $INSTDIR\GPL.html
  Delete $INSTDIR\puttylicense.html
  Delete $INSTDIR\legal.htm
  Delete $INSTDIR\readme.htm

  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\uninstall.exe
  ; remove shortcuts, if any.
  Delete "$SMPROGRAMS\FileZilla\*.*"
  Delete $DESKTOP\FileZilla.lnk
  ; remove directories used.

  ; Remove Startmenu shortcuts
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  
  StrCmp $MUI_TEMP "" noshortcuts
  
    Delete "$SMPROGRAMS\$MUI_TEMP\FileZilla.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\FileZilla Documentation.lnk"
    RMDir "$SMPROGRAMS\Start Menu Folder" ;Only if empty, so it won't delete other shortcuts
    
  noshortcuts:

  ; Remove folders
  RMDir "$INSTDIR"

SectionEnd

;--------------------------------
;Uninstaller functions

Function un.onInit
 
  !insertmacro MUI_UNGETLANGUAGE

FunctionEnd
