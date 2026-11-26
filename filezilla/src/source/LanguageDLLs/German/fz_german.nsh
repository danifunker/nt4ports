;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Tim Kosse (mailto:tim.kosse@gmx.de)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_GERMAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Deutsch"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla wird unter der GNU General Public License (GPL) veröffentlicht. Bitte lesen Sie die Lizenz sorgfältig durch. Klicken Sie auf Weiter um forzufahren."
LangString LicenseNext ${CURLANG} "&Weiter"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Voll"
LangString SecFileZillaName ${CURLANG} "FileZilla (benötigt)"
LangString SecDocumentationName ${CURLANG} "Dokumentation"
LangString SecEnglishDocumentationName ${CURLANG} "Englische Dokumentation"
LangString SecFrenchDocumentationName ${CURLANG} "Französische Dokumentation"
LangString SecLangFilesName ${CURLANG} "Weitere Sprachdateien"
LangString SecSourceCodeName ${CURLANG} "Quellcode"
LangString SecStartMenuName ${CURLANG} "Startmenü Verknüpfung"
LangString SecDesktopIconName ${CURLANG} "Desktop Symbol"
LangString SecDebugName ${CURLANG} "Debug files"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exe und andere benötigte Dateien installieren"
LangString SecDocumentationDesc ${CURLANG} "Die Dokumentation in den Zielordner kopieren"
LangString SecEnglishDocumentationDesc ${CURLANG} "Englische Dokumentation installieren"
LangString SecFrenchDocumentationDesc ${CURLANG} "Französische Dokumentation downloaden und installieren"
LangString SecLangFilesDesc ${CURLANG} "Weitere Sprachdateien in den Zielordner kopieren"
LangString SecSourceCodeDesc ${CURLANG} "Den Quellcode runterladen und in den Zielordner kopieren"
LangString SecStartMenuDesc ${CURLANG} "Verknüpfungen im Startmenü erstellen"
LangString SecDesktopIconDesc ${CURLANG} "Desktop Symbol für den schnellen Zugriff auf FileZilla hinzufügen"
LangString SecDebugDesc ${CURLANG} "Debug Dateien kopieren die bei der Crashanalyse helfen"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Allgemeine Einstellungen"
LangString PageSettingsSubTitle ${CURLANG} "Sicherer Modus und Speicherort für Einstellungen"
LangString PageDownloadTitle ${CURLANG} "Download-Optionen"
LangString PageDownloadSubTitle ${CURLANG} "Einige Komponenten müssen noch heruntergeladen werden."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Soll ${MUI_PRODUCT} im sicheren Modus ausgeführt werden?\nIm sicheren Modus speichert FileZilla keine Passwörter.\nDies wird empfohlen, wenn mehrere Personen Zugriff zu diesem PC haben."
LangString UseSecureMode ${CURLANG} "&Sicheren Modus benutzen"
LangString DontUseSecureMode ${CURLANG} "Sicheren Modus &nicht benutzen"
LangString StorageQuestion ${CURLANG} "FileZilla kann die Einstellungen auf zwei verschiedene Arten speichern: In der Registrierung oder in einer XML Datei. In den meisten Fällen wird die XML Datei empfohlen.\r\nWenn Sie sich in einer Mehrbenutzerumgebung befinden, in der jeder Benutzer sein eigenes Profil hat, sollten Sie die Registrierung als Speicherort auswählen, so dass jeder Benutzer seine eigenen Einstellungen in FileZilla hat."
LangString UseXML ${CURLANG} "&XML Datei benutzen"
LangString UseRegistry ${CURLANG} "Registrierung benutzen"
LangString ForceUseRegistry ${CURLANG} "&Registrierung benutzen selbst wenn eine XML Datei existiert"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Die folgenden Komponenten sind nicht im Setupprogramm enthalten, um Platz zu sparen:"
LangString DownloadDialog3 ${CURLANG} "Um diese Komponenten zu installieren, können diese jetzt automatisch runtergeladen werden. Wenn Sie diese Dateien jetzt nicht downloaden möchten, können Sie Setup später nochmal starten, oder Sie könnnen die Dateien selbst von http://filezilla.sourceforge.net runterladen."
LangString DownloadDialog4 ${CURLANG} "Aus dem Internet runterladen und installieren"
LangString DownloadDialog5 ${CURLANG} "Diese Komponenten überspringen"
LangString DownloadDialog6 ${CURLANG} "Lokales Paketverzeichnis benutzen:"
LangString DownloadDialog8 ${CURLANG} "Wenn das lokale Paketverzeichnis benutzt wird, speichert das Installationsprogramm alle runtergeladenen Dateien in diesem Ordner. Es werden auch nur die Dateien runterladen, die noch nicht in diesem Verzeichnis sind."

;Download strings
LangString DownloadDownloading ${CURLANG} "Download von $1"
LangString DownloadDownloadFailed ${CURLANG} "  Download fehlgeschlagen: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Download abgebrochen" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "$0 konnte nicht runtergeladen werden. Grund: $R1$\nDiese Komponente wird nicht installiert.$\nStarten Sie das Installationsprogramm später nochmal um es erneut zu versuchen.$\nWenn dieser Fehler immer angezeigt wird, ist das Installationsprogramm eventuell nicht in der Lage, auf das Internet zuzugreifen. Sie können die Komponenten jedoch jederzeit selbst von http://filezilla.sourceforge.net runterladen."
LangString DownloadDownloadSuccessful ${CURLANG} "  Download erfolgreich" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Der Inhalt von $0 konnte nicht extrahiert werden. Grund: $R1$\nDiese Komponente wird nicht installiert."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Alle Registrierungseinträge die von FileZilla erstellt wurden löschen? Dies schließt auch die Einträge der Seiten-Verwaltung mit ein, wenn die die Registrierung zur Speicherung der Einstellungen ausgewählt haben."

!verbose 4