;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Eirik Hodne (mailto:hodne@stud.ntnu.no)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_NORWEGIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Norwegian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Full"
LangString SecFileZillaName ${CURLANG} "FileZilla (krevd)"
LangString SecDocumentationName ${CURLANG} "Dokumentasjon"
LangString SecEnglishDocumentationName ${CURLANG} "Engelsk dokumentasjon"
LangString SecFrenchDocumentationName ${CURLANG} "Fransk dokumentasjon"
LangString SecLangFilesName ${CURLANG} "Ekstra språkfiler"
LangString SecSourceCodeName ${CURLANG} "Kildekode"
LangString SecStartMenuName ${CURLANG} "Snarvei i startmenyen"
LangString SecDesktopIconName ${CURLANG} "Ikon på skrivebordet"
LangString SecDebugName ${CURLANG} "Debuggings filer"

LangString SecFileZillaDesc ${CURLANG} "Innstallere FileZilla.exe og andre nødvendige filer"
LangString SecDocumentationDesc ${CURLANG} "Kopier dokumentasjonsfilene til innstallasjonsmappen"
LangString SecEnglishDocumentationDesc ${CURLANG} "Innstaller engelsk dokumentasjon"
LangString SecFrenchDocumentationDesc ${CURLANG} "Last ned og innstaller fransk dokumentasjon"
LangString SecLangFilesDesc ${CURLANG} "Kopier FileZilla's språkfiler til innstallasjonsmappen"
LangString SecSourceCodeDesc ${CURLANG} "Last ned og kopier kildekoden til innstallasjonsmappen."
LangString SecStartMenuDesc ${CURLANG} "Lag snarveier i startmenyen"
LangString SecDesktopIconDesc ${CURLANG} "Lag et skrivebordsikon for hurtig tilgang til FileZilla"
LangString SecDebugDesc ${CURLANG} "Kopier debuggingsfiler hvilket hjelper å analysere feil"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Generelle innstillinger"
LangString PageSettingsSubTitle ${CURLANG} "Konfigurer sikkerhetstype og hvor FileZilla skal lagre innstillinger"
LangString PageDownloadTitle ${CURLANG} "Nedlastings innstillinger"
LangString PageDownloadSubTitle ${CURLANG} "Noen komponenter må lastes ned"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Kjør ${MUI_PRODUCT} i sikker modus?\r\nI sikker modus, husker FileZilla ingen passord for deg .\r\nDet er anbefalt å bruke sikker modus hvis flere personer har tilgang til denne pc'en."
LangString UseSecureMode ${CURLANG} "&Bruk sikker modus"
LangString DontUseSecureMode ${CURLANG} "&Ikke bruk sikker modus"
LangString StorageQuestion ${CURLANG} "FileZilla kan lagre sine innstillinger på to forskjellige plasser: I registeret eller i en XML fil.\r\nI de fleste tilfeller er XML-basert lagring å anbefale.\r\nHvis du er i et fler-bruker miljø hvor hver bruker har sin egen profil, kan du bruke registeret som lagringsplass slik at hver bruker også får sine egne innstillinger."
LangString UseXML ${CURLANG} "Bruk &XML fil"
LangString UseRegistry ${CURLANG} "Bruk &registeret"
LangString ForceUseRegistry ${CURLANG} "&Tving bruk av registeret selv om XML filen eksisterer"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Følgende komponenter er ikke inkludert i innstallasjonsfilen for å redusere filstørrelsen:"
LangString DownloadDialog3 ${CURLANG} "For å innstallere disse komponentene må vi laste dem ned nå. Hvis du ikke vil laste ned nå, kan du kjøre innstallasjonen på ny ved en senere anledning eller laste ned filene manuelt fra http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Last ned og installer fra internett"
LangString DownloadDialog5 ${CURLANG} "Hopp over disse komponentene"
LangString DownloadDialog6 ${CURLANG} "Bruk lokal lagringsmappe(skriv inn under):"
LangString DownloadDialog8 ${CURLANG} "Hvis du bruker lagringsmappe vil installasjonen lagre alle nedlastede filer her. Den vil også bare laste ned filer som ikke allerede finnes i denne mappen."

;Download strings
LangString DownloadDownloading ${CURLANG} "Laster ned $1"
LangString DownloadDownloadFailed ${CURLANG} "  Nedlasting feilet:: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Nedlasting avbrutt" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Feilet nedlasting av $0. Grunn: $R1$\nDenne komponenten vil ikke bli innstallert.$\nKjør innstallasjonen på nytt for å prøve å laste ned på ny.$\nHvis feilen gjentar seg kan det være at innstallasjonen ikke får kontakt med internett. Uansett kan du laste ned komponentene manuelt fra http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Nedlasting fullført" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Feil ved utpakking av innholdet i $0. Grunn: $R1$\nDenne komponenten vil ikke bli innstallert."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Fjerne alle registernøkler laget av FileZilla (inkludert servere i Site Manager hvis du valgte å lagre innstillingene i registeret)?"

!verbose 4
