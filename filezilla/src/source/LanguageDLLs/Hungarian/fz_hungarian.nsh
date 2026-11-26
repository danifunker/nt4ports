;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Bata György

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_HUNGARIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Hungarian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Szokásos"
LangString InstTypeFullName ${CURLANG} "Teljes"
LangString SecFileZillaName ${CURLANG} "FileZilla (szükséges)"
LangString SecDocumentationName ${CURLANG} "Dokumentáció"
LangString SecEnglishDocumentationName ${CURLANG} "Angol dokumentáció"
LangString SecFrenchDocumentationName ${CURLANG} "Francia dokumentáció"
LangString SecLangFilesName ${CURLANG} "Nyelvi fájlok"
LangString SecSourceCodeName ${CURLANG} "Forráskód"
LangString SecStartMenuName ${CURLANG} "Start menü parancsikonok"
LangString SecDesktopIconName ${CURLANG} "Asztali ikon"
LangString SecDebugName ${CURLANG} "Hibakeresési fájlok"

LangString SecFileZillaDesc ${CURLANG} "A FileZilla.exe és más szükséges fájlok telepítése"
LangString SecDocumentationDesc ${CURLANG} "A dokumentációs fájlok telepítése"
LangString SecEnglishDocumentationDesc ${CURLANG} "Az angol dokumentáció telepítése"
LangString SecFrenchDocumentationDesc ${CURLANG} "A francia dokumentáció letöltése és telepítése"
LangString SecLangFilesDesc ${CURLANG} "A FileZilla nyelvi fájljainak telepítése"
LangString SecSourceCodeDesc ${CURLANG} "A forráskód letöltése és telepítése"
LangString SecStartMenuDesc ${CURLANG} "Parancsikonok létrehozása a Start menüben"
LangString SecDesktopIconDesc ${CURLANG} "Parancsikon létrehozása az asztalon"
LangString SecDebugDesc ${CURLANG} "A hibakeresési fájlok telepítése, melyek az összeomlás elemzésében segíthetnek"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Általános beállítások"
LangString PageSettingsSubTitle ${CURLANG} "A biztonsági mód és a beállítások tárolásának konfigurálása"
LangString PageDownloadTitle ${CURLANG} "Letöltés beállításai"
LangString PageDownloadSubTitle ${CURLANG} "Néhány összetevõ letöltésére van szükség."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "A ${MUI_PRODUCT}-t biztonságos módban szeretné futtatni?\nA biztonságos módban a FileZilla nem jegyzi meg a jelszavakat.\nA biztonságos mód választása ajánlott, ha többen használják ezt a számítógépet."
LangString UseSecureMode ${CURLANG} "Biztonságos mód használata"
LangString DontUseSecureMode ${CURLANG} "Ne használja a biztonságos módot"
LangString StorageQuestion ${CURLANG} "A FileZilla a beállításait kétféle helyen tárolhatja: a regisztrációs adatbázisban vagy egy XML fájlban. A legtöbb esetben az XML fájl használata az ajánlott. Azonban ha egy többfelhasználós számítógépen dolgozik, amelyen a felhasználók saját felülettel rendelkeznek, akkor a regisztrációs adatbázist célszerû használni a beállítások tárolására, így a FileZilla beállításai is egyénileg testre szabhatók lesznek."
LangString UseXML ${CURLANG} "&XML fájl használata"
LangString UseRegistry ${CURLANG} "A &regisztrációs adatbázis használata"
LangString ForceUseRegistry ${CURLANG} "&Abban az esetben is, ha az XML fájl már létezik"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "A telepítõ a következõ összetevõket nem tartalmazza a kis fájlméret megtartása érdekében:"
LangString DownloadDialog3 ${CURLANG} "Ezen összetevõk telepítéséhez a szükséges fájlokat a telepítõ le tudja tölteni. Ha nem szeretné, hogy a telepítõ most letöltse a fájlokat, bármikor újból futtathatja a telepítõt, vagy saját maga kézzel letöltheti a fájlokat a http://filezilla.sourceforge.net címrõl."
LangString DownloadDialog4 ${CURLANG} "Letöltés és telepítés az Internetrõl"
LangString DownloadDialog5 ${CURLANG} "Ezeknek az összetevõknek a kihagyása"
LangString DownloadDialog6 ${CURLANG} "Helyi letöltési könyvtár használata (adja meg alább):"
LangString DownloadDialog8 ${CURLANG} "Ha a helyi letöltési könyvtár használatát engedélyezi, a telepítõ az összes letöltött fájlt ebbe a mappába menti el. Másrészt csak azokat a fájlokat tölti le, melyek még nincsenek jelen a letöltési könyvtárban."

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 letöltése"
LangString DownloadDownloadFailed ${CURLANG} "  $0 letöltése nem sikerült." ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  A letöltés megszakadt." ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "$0 letöltése nem sikerült. Oka: $R1$\nEz az összetevõ nem fog telepítõdni.$\nKésõbb futtassa újra a telepítõt az összetevõ letöltésének megismétléséhez.$\nHa ez a hibaüzenet megmarad, akkor lehet, hogy a telepítõ nem tud az internethez csatlakozni. Az összetevõket bármikor kézzel is letöltheti a http://filezilla.sourceforge.net címrõl."
LangString DownloadDownloadSuccessful ${CURLANG} "  A letöltés sikerült." ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0 kicsomagolása nem sikerült. Oka: $R1$\nEz az összetevõ nem fog telepítõdni."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Töröljem a FileZilla által létrehozott összes regisztrációs bejegyzést (beleértve a Kedvenc Helyek állomásait is, amennyiben a regisztrációs adatbázist választotta a beállítások tárolásához)?"

!verbose 4

