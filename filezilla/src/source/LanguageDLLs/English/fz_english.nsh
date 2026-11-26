;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by <insert your name here>

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "English"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Full"
LangString SecFileZillaName ${CURLANG} "FileZilla (required)"
LangString SecDocumentationName ${CURLANG} "Documentation"
LangString SecEnglishDocumentationName ${CURLANG} "English Documentation"
LangString SecFrenchDocumentationName ${CURLANG} "French Documentation"
LangString SecLangFilesName ${CURLANG} "Additional language files"
LangString SecSourceCodeName ${CURLANG} "Source Code"
LangString SecStartMenuName ${CURLANG} "Start Menu Shortcuts"
LangString SecDesktopIconName ${CURLANG} "Desktop Icon"
LangString SecDebugName ${CURLANG} "Debug files"

LangString SecFileZillaDesc ${CURLANG} "Install FileZilla.exe and other required files"
LangString SecDocumentationDesc ${CURLANG} "Copy the documentation files to the target location"
LangString SecEnglishDocumentationDesc ${CURLANG} "Install the English documentation"
LangString SecFrenchDocumentationDesc ${CURLANG} "Download and install the French documentation"
LangString SecLangFilesDesc ${CURLANG} "Copy the language files of FileZilla to the target location"
LangString SecSourceCodeDesc ${CURLANG} "Download and copy the source code to the target location."
LangString SecStartMenuDesc ${CURLANG} "Create shortcuts in the start menu"
LangString SecDesktopIconDesc ${CURLANG} "Add a desktop icon for quick access to FileZilla"
LangString SecDebugDesc ${CURLANG} "Copy debug files which help to analyze crashes"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "General settings"
LangString PageSettingsSubTitle ${CURLANG} "Configure secure mode and settings storage location"
LangString PageDownloadTitle ${CURLANG} "Download options"
LangString PageDownloadSubTitle ${CURLANG} "Some components have to be downloaded"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Run ${MUI_PRODUCT} in secure mode?\r\nIn secure mode, FileZilla does not remember any passwords.\r\nIt is recommended to use secure mode if several people have access to this PC."
LangString UseSecureMode ${CURLANG} "&Use secure mode"
LangString DontUseSecureMode ${CURLANG} "Do &not use secure mode"
LangString StorageQuestion ${CURLANG} "FileZilla can store its settings in two different places: In the registry and in an XML file.\r\nIn most cases it is recommended to use the XML.\r\nIf you are in a multi-user environment where each user has its own profile, you should use the registry as storage location so that each user has its own settings for FileZilla as well."
LangString UseXML ${CURLANG} "Use &XML file"
LangString UseRegistry ${CURLANG} "use &registry"
LangString ForceUseRegistry ${CURLANG} "&Force registry even if XML file exists"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "The following components are not included in the installer to reduce its size:"
LangString DownloadDialog3 ${CURLANG} "To install these components, the installer can download the required files. If you do not want to download the files now, you can run the installer again later, or you can download the files manually from http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Download and Install from the internet"
LangString DownloadDialog5 ${CURLANG} "Skip these components"
LangString DownloadDialog6 ${CURLANG} "Use local package directory (enter below):"
LangString DownloadDialog8 ${CURLANG} "When using the package directory, the installer will save all downloaded files in this directory. It will also only download the files which aren't already present in the package directory."

;Download strings
LangString DownloadDownloading ${CURLANG} "Downloading $1"
LangString DownloadDownloadFailed ${CURLANG} "  Download failed: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Download cancelled" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Failed to download $0. Reason: $R1$\nThis component will not be installed.$\nRun the installer again later to retry downloading this component.$\nIf this error stays, the installer may not able to access the internet. In any case you can download the components manually from http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Download successful" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Failed to extract contents of $0. Reason: $R1$\nThis component will not be installed."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Delete all Registry keys created by FileZilla (including Site Manager entries if you did choose the registry as storage location)?"

!verbose 4