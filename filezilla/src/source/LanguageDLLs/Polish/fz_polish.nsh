;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Piotr Murawski & Rafa≥ Lampe; www.lomsel.com.pl

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_POLISH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Polski"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standardowa"
LangString InstTypeFullName ${CURLANG} "Pe≥na"
LangString SecFileZillaName ${CURLANG} "FileZilla (wymagane)"
LangString SecDocumentationName ${CURLANG} "Dokumentacja"
LangString SecEnglishDocumentationName ${CURLANG} "Angielska dokumentacja"
LangString SecFrenchDocumentationName ${CURLANG} "Francuska dokumentacja"
LangString SecLangFilesName ${CURLANG} "Dodatkowe pliki jÍzyka"
LangString SecSourceCodeName ${CURLANG} "èrÛd≥o kodu"
LangString SecStartMenuName ${CURLANG} "SkrÛty klawiszowe Menu Start"
LangString SecDesktopIconName ${CURLANG} "Ikona pulpitu"
LangString SecDebugName ${CURLANG} "Pliki Debug"

LangString SecFileZillaDesc ${CURLANG} "Instaluje FileZilla.exe oraz inne wymagane pliki"
LangString SecDocumentationDesc ${CURLANG} "Kopiuje pliki dokumentacji do miejsca docelowego"
LangString SecEnglishDocumentationDesc ${CURLANG} "Instaluj angielskπ dokumentacjÍ"
LangString SecFrenchDocumentationDesc ${CURLANG} "Pobierz i instaluj francuskπ dokumentacjÍ"
LangString SecLangFilesDesc ${CURLANG} "Kopiuje pliki jÍzyka do miejsca docelowego"
LangString SecSourceCodeDesc ${CURLANG} "Pobiera i kopiuje ürÛd≥o kodu do miejsca docelowego."
LangString SecStartMenuDesc ${CURLANG} "Tworzy klawisze skrÛtÛw w menu start"
LangString SecDesktopIconDesc ${CURLANG} "Dodaje ikonÍ pulpitu, aby uzyskaÊ szybszy dostÍp do programu FileZilla"
LangString SecDebugDesc ${CURLANG} "Kopiuj pliki debug, ktÛre pomogπ zanalizowaÊ uszkodzenia"

;Page titles
LangString PageSettingsTitle ${CURLANG} "OgÛlne ustawienia"
LangString PageSettingsSubTitle ${CURLANG} "Potwierdü tryb ochrony i zapis ustawieÒ"
LangString PageDownloadTitle ${CURLANG} "Opcje pobierania"
LangString PageDownloadSubTitle ${CURLANG} "NiektÛre komponenty muszπ byÊ pobrane"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Czy chcesz uruchomiÊ ${MUI_PRODUCT} w bezpiecznym trybie?\nW bezpiecznym trybie FileZilla nie pamiÍta øadnych hase≥.\nJest to zalecane jeøeli kilka osÛb ma dostÍp do komputera."
LangString UseSecureMode ${CURLANG} "Uøyj trybu ochrony"
LangString DontUseSecureMode ${CURLANG} "Nie uøywaj trybu ochrony"
LangString StorageQuestion ${CURLANG} "FileZilla moøe zapisaÊ ustawienia w dwÛch rÛønych miejscach: W rejestrze i w pliku XML.\r\nW wiÍkszoúci przypadkÛw zaleca siÍ zapis do pliku XML.\r\nJeøeli pracujesz w trybie wielu uøytkownikÛw, gdzie kaødy uøytkownik ma swÛj w≥asny profil, to naleøy uøyÊ zapisu do rejestru, øeby kaødy uøytkownik mia≥ swoje ustawienia dla FileZilla."
LangString UseXML ${CURLANG} "Uøyj pliku &XML"
LangString UseRegistry ${CURLANG} "Uøyj &rejestru"
LangString ForceUseRegistry ${CURLANG} "Wymuú zapis do rejestru nawet, gdy istnieje plik XML"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "NastÍpujπce komponenty nie sπ w≥πczone do instalatora po to by zredukowaÊ rozmiar:"
LangString DownloadDialog3 ${CURLANG} "Aby zainstalowaÊ te komponenty instalator moøe pobraÊ wymagane pliki. Jeøeli nie chcesz teraz pobraÊ plikÛw, to moøesz uruchomiÊ instalator pÛüniej lub pobraÊ pliki rÍcznie z http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Pobierz i instaluj z Internetu"
LangString DownloadDialog5 ${CURLANG} "PomiÒ te komponenty"
LangString DownloadDialog6 ${CURLANG} "Uøyj lokalnego katalogu (wpisz poniøej):"
LangString DownloadDialog8 ${CURLANG} "Kiedy uøywasz katalogu, to instalator zapisze wszystkie pobrane pliki do tego katalogu. Zostanπ pobrane tylko te pliki, ktÛre nie sπ obecne w tym katalogu."

;Download strings
LangString DownloadDownloading ${CURLANG} "Pobiera $1"
LangString DownloadDownloadFailed ${CURLANG} "  Pobieranie nie powiod≥o siÍ: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Pobieranie anulowano" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "B≥πd pobierania $0. Przyczyna: $R1$\nTen komponent nie zostanie zainstalowany.$\nUruchom pÛüniej jeszcze raz instalator, aby ponownie sprÛbowaÊ pobraÊ ten komponent.$\nJeøeli b≥πd wystÍpuje nadal, to moøe instalator nie ma dostÍpu do internetu. W kaødym przypadku moøna pobraÊ komponenty rÍcznie z http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Pobieranie powiod≥o siÍ" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "B≥πd rozpakowania zawartoúci $0. Przyczyna: $R1$\nTen komponent nie zostanie zainstalowany."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Czy usunπÊ wszystkie wpisy w rejestrze utworzone przez FileZilla (w≥πczajπc wpisy menedøera stron, gdy wybrano rejestr do zapisu ustawieÒ)?"

!verbose 4
