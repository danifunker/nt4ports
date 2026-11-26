;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Malin (fastestbrowser@szm.sk)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_SLOVAK}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Slovak"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Štandardná"
LangString InstTypeFullName ${CURLANG} "Plná"
LangString SecFileZillaName ${CURLANG} "FileZilla (poadované)"
LangString SecDocumentationName ${CURLANG} "Dokumentácia"
LangString SecEnglishDocumentationName ${CURLANG} "Anglická dokumentácia"
LangString SecFrenchDocumentationName ${CURLANG} "Francúzska dokumentácia"
LangString SecLangFilesName ${CURLANG} "Ïalšie jazykové súbory"
LangString SecSourceCodeName ${CURLANG} "Zdrojvı kód"
LangString SecStartMenuName ${CURLANG} "Odkazy v ponuke Štart"
LangString SecDesktopIconName ${CURLANG} "Ikona na ploche"
LangString SecDebugName ${CURLANG} "Ladiace súbory"

LangString SecFileZillaDesc ${CURLANG} "Nainštaluje FileZilla.exe a iné potrebné súbory"
LangString SecDocumentationDesc ${CURLANG} "Skopíruje súbory dokumentácie do cie¾ového adresára"
LangString SecEnglishDocumentationDesc ${CURLANG} "Inštaluje Anglickú dokumentáciu"
LangString SecFrenchDocumentationDesc ${CURLANG} "Stiahne a inštaluje Francúzsku dokumentáciu"
LangString SecLangFilesDesc ${CURLANG} "Skopíruje jazykové súbory FileZilly do cie¾ového adresára"
LangString SecSourceCodeDesc ${CURLANG} "Stiahne a skopíruje zdrojovı kód do cie¾ového adresára."
LangString SecStartMenuDesc ${CURLANG} "Vytvorí odkazy v ponuke Štart"
LangString SecDesktopIconDesc ${CURLANG} "Pridá ikonu na Plochu na rıchly prístup k FileZille"
LangString SecDebugDesc ${CURLANG} "Kopírova ladiace súbory pomáhajúce analyzova vzniknuté problémy"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Všeobecné nastavenia"
LangString PageSettingsSubTitle ${CURLANG} "Konfigurova bezpeènostnı mód a nastavi adresár na uloenie"
LangString PageDownloadTitle ${CURLANG} "Nastavenia downloadu"
LangString PageDownloadSubTitle ${CURLANG} "Niektoré súèasti treba stiahnu"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Spusti ${MUI_PRODUCT} v bezpeènom móde?\r\nV bezpeènom móde, FileZilla si nepamätá heslá.\r\nOdporúèa sa poui bezpeènı mód ak má viac ¾udí prístup k tomuto PC."
LangString UseSecureMode ${CURLANG} "&Poui bezpeènı mód"
LangString DontUseSecureMode ${CURLANG} "&Nepoui bezpeènı mód"
LangString StorageQuestion ${CURLANG} "FileZilla vie uloi svoje nastavenia na dvoch miestach: V registroch a XML súbore.\r\nVo väèšine prípadov sa odporúèa XML súbor.\r\nAk pracuješ vo viacpouívate¾skom prostredí, kde má kadı pouívate¾ svoj profil, mal by si poui registre, aby mal kadı pouívate¾ vlastné nastavenia."
LangString UseXML ${CURLANG} "Poui &XML súbor"
LangString UseRegistry ${CURLANG} "Poui &registre"
LangString ForceUseRegistry ${CURLANG} "&Do registrov aj keï existuje XML súbor"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Nasledovné komponenty nie sú v inštalaènom balíku, z dôvodu jeho ve¾kosti:"
LangString DownloadDialog3 ${CURLANG} "Na ich inštaláciu, vie inštalátor stiahnu poadované súbory. Ak nechceš sahova súbory teraz, môeš spusti inštalátor neskôr alebo stiahnu súbory ruène z http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Stiahnu a Inštalova z internetu"
LangString DownloadDialog5 ${CURLANG} "Preskoèi komponenty"
LangString DownloadDialog6 ${CURLANG} "Poui lokálny adresár balíèkov (zadaj nišie):"
LangString DownloadDialog8 ${CURLANG} "Pri pouití adresára balíèkov, inštalátor uloí všetky stiahnuté súbory do tohoto adresára. Stiahne tie iba súbory, ktoré adresár balíèkov neobsahuje."

;Download strings
LangString DownloadDownloading ${CURLANG} "Sahujem $1"
LangString DownloadDownloadFailed ${CURLANG} "  Zlyahl download: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Download zrušenı" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Zlyhalo sahovanie $0. Dôvod: $R1$\nTento komponent nebude inštalovanı.$\nSpusti inštalátora neskôr na opakovanie sahovania komponentu.$\nAk chyba pretrváva, inštalátor nemôe pristúpi na internet. Tieto súbory môeš stiahnu ruène z http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Úspešnı download" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Zlyhala extrakcia obsahu $0. Dôvod: $R1$\nTento komponent nebude inštalovanı."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Zmaza všetky k¾úèe v registroch vytvorené FileZillou (vrátane záznamov Manaéra miest ak si si vybral na ukladanie nastavení registre)?"

!verbose 4