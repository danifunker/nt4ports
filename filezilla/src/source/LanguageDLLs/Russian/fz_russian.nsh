;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by <insert your name here>

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_RUSSIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Russian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Стандартная"
LangString InstTypeFullName ${CURLANG} "Полная"
LangString SecFileZillaName ${CURLANG} "FileZilla (обязательно)"
LangString SecDocumentationName ${CURLANG} "Документация"
LangString SecEnglishDocumentationName ${CURLANG} "Документация на английском"
LangString SecFrenchDocumentationName ${CURLANG} "Документация на французском"
LangString SecLangFilesName ${CURLANG} "Дополнительные языковые файлы"
LangString SecSourceCodeName ${CURLANG} "Исходные коды программ"
LangString SecStartMenuName ${CURLANG} "Ярлыки в Главном Меню"
LangString SecDesktopIconName ${CURLANG} "Ярлык на Рабочем Столе"
LangString SecDebugName ${CURLANG} "Отладочные файлы"

LangString SecFileZillaDesc ${CURLANG} "Установка FileZilla.exe и других необходимых файлов"
LangString SecDocumentationDesc ${CURLANG} "Копирование файлов документации в выбранную папку"
LangString SecEnglishDocumentationDesc ${CURLANG} "Установить документацию на английском языке"
LangString SecFrenchDocumentationDesc ${CURLANG} "Загрузить и установить документацию на английском языке"
LangString SecLangFilesDesc ${CURLANG} "Копировать языковые файлы FileZilla в выбранную папку"
LangString SecSourceCodeDesc ${CURLANG} "Загрузить и копировать исходные коды программ в выбранную папку"
LangString SecStartMenuDesc ${CURLANG} "Создать ярлыки в Главном Меню"
LangString SecDesktopIconDesc ${CURLANG} "Добавить ярлык для запуска FileZilla на Рабочий Стол"
LangString SecDebugDesc ${CURLANG} "Копировать отладочные файлы, которые помогут избежать ошибок"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Общие настройки"
LangString PageSettingsSubTitle ${CURLANG} "Конфигурирование защищенного режима и настройки расположения программы"
LangString PageDownloadTitle ${CURLANG} "Настройки загрузки"
LangString PageDownloadSubTitle ${CURLANG} "Некоторые компоненты должны быть дополнительно загружены"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Запускать ${MUI_PRODUCT} в защищенном режиме?\r\nВ защищенном режиме, FileZilla не запоминает пароли.\r\nЭтот режим рекомендуется если за компьютером работает несколько пользователей."
LangString UseSecureMode ${CURLANG} "&Использовать защищенный режим"
LangString DontUseSecureMode ${CURLANG} "&Не использовать защищенный режим"
LangString StorageQuestion ${CURLANG} "FileZilla может хранить настройки двумя способами: В реестре и XML файле.\r\nДля одного пользователя рекомендуется XML.\r\nЕсли на компьютере работает не один пользователь, то рекомендуется хранить настройки в реестре, т.к. в XML не может хранится более одной настройки."
LangString UseXML ${CURLANG} "Использовать &XML файл"
LangString UseRegistry ${CURLANG} "Использовать &реестр"
LangString ForceUseRegistry ${CURLANG} "&Перенести настройки из XML файла в реестр."

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Выбранные компоненты требуют для установки объем свободного дискового пространства:"
LangString DownloadDialog3 ${CURLANG} "Для установки этих компонентов, программа установки должна загрузить их. Если Вы не хотите загружать их сейчас, Вы можете запустить программу установки позже, или можете загрузить эти файлы вручную с http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Загрузить и установить из Интернет"
LangString DownloadDialog5 ${CURLANG} "Пропустить эти компоненты"
LangString DownloadDialog6 ${CURLANG} "Использовать локальную папку (введите здесь):"
LangString DownloadDialog8 ${CURLANG} "Когда используется установочная папка, программа установки загружает файлы в нее. Это необходимо использовать тогда, когда необходимые файлы отсутствуют в установочной папке."

;Download strings
LangString DownloadDownloading ${CURLANG} "Загружено $1"
LangString DownloadDownloadFailed ${CURLANG} "  Загрузка невозможна: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Загрузка отменена" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Ошибка при загрузке $0. Причина: $R1$\nЭтот компонент не может быть установлен.$\nЗапустите программу установки снова после загрузки необходимых файлов.$\nЕсли ошибка повторяется, вероятно нет подключения к Интерент. Вы можете загрузить этот компонетр вручную с сайта http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Загрузка завершена" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Ошибка извлечения файлов $0. Причина: $R1$\nЭтот компонент не может быть установлен."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Удалить все ключи реестра сгенерированные FileZilla (включая ключи Менеджера Сайтов если Вы использовали реестр для хранения настроек)?"

!verbose 4
