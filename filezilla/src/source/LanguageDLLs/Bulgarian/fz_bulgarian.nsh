;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by <Asparouh Kalyandjiev (acnapyx@computers.bg)>

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_BULGARIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Bulgarian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Стандартна"
LangString InstTypeFullName ${CURLANG} "Пълна"
LangString SecFileZillaName ${CURLANG} "FileZilla (задължителен)"
LangString SecDocumentationName ${CURLANG} "Документация"
LangString SecEnglishDocumentationName ${CURLANG} "English Documentation"
LangString SecFrenchDocumentationName ${CURLANG} "French Documentation"
LangString SecLangFilesName ${CURLANG} "Допълнителни езикови файлове"
LangString SecSourceCodeName ${CURLANG} "Изходен код"
LangString SecStartMenuName ${CURLANG} "Препратки в Start менюто"
LangString SecDesktopIconName ${CURLANG} "Икона на десктопа"
LangString SecDebugName ${CURLANG} "Debug files"

LangString SecFileZillaDesc ${CURLANG} "Инсталиране на FileZilla.exe и другите задължителни файлове"
LangString SecDocumentationDesc ${CURLANG} "Копиране на файловете с документацията в целевата папка"
LangString SecEnglishDocumentationDesc ${CURLANG} "Install the English documentation"
LangString SecFrenchDocumentationDesc ${CURLANG} "Download and install the French documentation"
LangString SecLangFilesDesc ${CURLANG} "Копиране на езиковите файлове на FileZilla в целевата папка"
LangString SecSourceCodeDesc ${CURLANG} "Изтегляне и копиране на изходния код в целевата папка"
LangString SecStartMenuDesc ${CURLANG} "Създаване на препратки в Start менюто"
LangString SecDesktopIconDesc ${CURLANG} "Добавяне на икона на десктопа за бърз достъп до FileZilla"
LangString SecDebugDesc ${CURLANG} "Copy debug files which help to analyze crashes"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "General settings"
LangString PageSettingsSubTitle ${CURLANG} "Configure secure mode and settings storage"
LangString PageDownloadTitle ${CURLANG} "Опции за изтегляне"
LangString PageDownloadSubTitle ${CURLANG} "Някои компоненти тлябва да бъдат изтеглени от Интернет"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Желаете ли да стартирате ${MUI_PRODUCT} в сигурен режим?\nВ сигурен режим, FileZilla не записва никакви пароли.\nПрепоръчително е да използвате тази опция, ако няколко души имат достъп до това PC."
LangString UseSecureMode ${CURLANG} "Use secure mode"
LangString DontUseSecureMode ${CURLANG} "Do not use secure mode"
LangString StorageQuestion ${CURLANG} "FileZilla can store its settings in two different places: In the registry and in an XML file.\r\nIn most cases it is recommended to use the XML.\r\nIf you are in a multi-user environment where each user has its own profile, you should use the registry as storage location so that each user has it's own settings for FileZilla as well."
LangString UseXML ${CURLANG} "Use &XML file"
LangString UseRegistry ${CURLANG} "use &registry"
LangString ForceUseRegistry ${CURLANG} "&Force registry even if XML file exists"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Следните компоненти не са включени в инсталацията, за да се намали размерът й:"
LangString DownloadDialog3 ${CURLANG} "За да инсталирате тези компоненти, инсталационната програма може да ги изтегли от Интернет. Ако не желаете да ги изтегляте сега, можете да стартирате инсталацията по-късно или да ги изтеглите ръчно от http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Изтегляне и инсталация от Интернет"
LangString DownloadDialog5 ${CURLANG} "Пропусни тези компоненти"
LangString DownloadDialog6 ${CURLANG} "Използвай локална папка (въведете я по-долу):"
LangString DownloadDialog8 ${CURLANG} "Ако използвате локална папка, инсталацията ще запише всички изтеглени файлове в тази папка. Също така тя ще изтегли само файловете, които вече не присъстват в тази папка."

;Download strings
LangString DownloadDownloading ${CURLANG} "Изтегляне на $1"
LangString DownloadDownloadFailed ${CURLANG} "  Изтеглянето пропадна: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Изтеглянето прекъснато" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Пропадна изтеглянето на $0. Причина: $R1$\nТози компонент няма да бъде инсталиран.$\nСтартирайте инсталатора по-късно, за да опитате отново да изтеглите този компонент.$\nАко грешката се появява постоянно, вероятно инсталацията няма достъп до Интернет. Можете да изтеглите компонентите и ръчно от http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Изтеглянето завърши успешно" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Пропадна извличането на съдържанието на $0. Причина: $R1$\nТози компонент няма да бъде инсталиран."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Да бъдат ли изтрити всички ключове от регистъра, създадени от FileZilla (включитело съдържанието на Мениджъра на сайтове), ако сте избрали регистъра като място за съхранение на настройките?"

!verbose 4