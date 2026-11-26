;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Vojtech Sazel (mailto:vsazel@atlas.cz)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_CZECH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Czech"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standartní"
LangString InstTypeFullName ${CURLANG} "Plná"
LangString SecFileZillaName ${CURLANG} "FileZilla (poadováno)"
LangString SecDocumentationName ${CURLANG} "Dokumentace"
LangString SecEnglishDocumentationName ${CURLANG} "Anglická dokumentace"
LangString SecFrenchDocumentationName ${CURLANG} "Francouzská dokumentace"
LangString SecLangFilesName ${CURLANG} "Dodateèné soubory s jazyky"
LangString SecSourceCodeName ${CURLANG} "Zdrojovı kód"
LangString SecStartMenuName ${CURLANG} "Vytvoøit poloky v nabídce Start"
LangString SecDesktopIconName ${CURLANG} "Ikona na ploše"
LangString SecDebugName ${CURLANG} "Ladící soubory"

LangString SecFileZillaDesc ${CURLANG} "Installovat FileZillu a ostatní poadované soubory"
LangString SecDocumentationDesc ${CURLANG} "Instalovat soubory dokumentace"
LangString SecEnglishDocumentationDesc ${CURLANG} "Instalovat anglickou dokumentaci"
LangString SecFrenchDocumentationDesc ${CURLANG} "Stáhnout a nainstalovat francouzskou dokumentaci"
LangString SecLangFilesDesc ${CURLANG} "Zkopírovat soubory s pøeklady FileZilly do rùznıch jazykù"
LangString SecSourceCodeDesc ${CURLANG} "Stáhnout a nainstalovat zdrojové soubory FileZilly"
LangString SecStartMenuDesc ${CURLANG} "Vytvoøit zástupce v nabídce Start"
LangString SecDesktopIconDesc ${CURLANG} "Vytvoøit zástupce na ploše"
LangString SecDebugDesc ${CURLANG} "Nainstalovat ladící soubory pro ladìní FileZilly"

;Page titles
LangString PageSettingsTitle ${CURLANG} "Obecná nastavení"
LangString PageSettingsSubTitle ${CURLANG} "Konfigurovat bezpeènı reim a umístìní instalace"
LangString PageDownloadTitle ${CURLANG} "Nastavení stahování"
LangString PageDownloadSubTitle ${CURLANG} "Nìkteré èásti nelze stáhnout"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Spustit ${MUI_PRODUCT} v bezpeèném reimu?\r\nV bezpeèném reimu si FileZilla nebude pamatovat ádná hesla.\r\nJe doporuèeno pouít bezpeènı reim, pokud tento poèítaè pouívá více osob."
LangString UseSecureMode ${CURLANG} "&Pouít bezpeènı reim"
LangString DontUseSecureMode ${CURLANG} "&Nepouívat bezpeènı reim"
LangString StorageQuestion ${CURLANG} "FileZilla mùe ukládat nastavení na dvì odlišná místa: do registrù nebo and do XML souboru.\r\nIn most cases it is recommended to use the XML.\r\nPokud tento poèítaè pouívá více uivatelù, pak je doporuèeno ukládat nastavení do registrù."
LangString UseXML ${CURLANG} "Pouít &XML soubor"
LangString UseRegistry ${CURLANG} "Pouít &registry"
LangString ForceUseRegistry ${CURLANG} "&Vnutit registry i kdy XML soubor existuje"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Tyto komponenty nejsou zahrnuty v instalátoru:"
LangString DownloadDialog3 ${CURLANG} "K nainstalování tìchto komponent musíte bıt pøipojeni k internetu. Instalátor mùe stáhnout poadované soubory. Pokud tyto soubory nechcete nyní stahovat, mùete instalátor spustit pozdìji nebo je stáhnout ruènì z http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Stáhnout a instalovat z internetu"
LangString DownloadDialog5 ${CURLANG} "Pøeskoèit tyto komponenty"
LangString DownloadDialog6 ${CURLANG} "Pouít místní adresáø pro balíèky (zadejte níe):"
LangString DownloadDialog8 ${CURLANG} "Pokud pouíváte tento adresáø pro balíèky, instalaèní program uloí všechny staené soubory do tohoto adresáøe. Stáhne také jen balíèky, které ještì nejsou v adresáøi uloené."

;Download strings
LangString DownloadDownloading ${CURLANG} "Stahuji $1"
LangString DownloadDownloadFailed ${CURLANG} "  Stahování selhalo: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Stahování pøerušeno" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Nelze stáhnout $0. Dùvod: $R1$\nTato komponenta nebude nainstalována.$\nSpuste instalátor pozdìji a zkuste opakovat stahování.$\nPokud tato chyba pøetrvává, instalátor moná nemùe pøistupovat do internetu. V tomto pøípadì mùete komponenty stáhnout ruène z http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Stahování úspìšné" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Nelze rozbalit soubor $0. Dùvod: $R1$\nTato komponenta nebude nainstalována."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Chcete smazat všechny poloky FileZilly v registrech (vèetnì poloek Správce míst, pokud jste zvolili registry jako místo jejich uloení)?"

!verbose 4