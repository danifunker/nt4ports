;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Yuan Chia Fu (mailto:ycfunet@gmail.com)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_TRADCHINESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Traditional Chinese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "標準安裝"
LangString InstTypeFullName ${CURLANG} "完整安裝"
LangString SecFileZillaName ${CURLANG} "FileZilla (基本組件)"
LangString SecDocumentationName ${CURLANG} "說明文件"
LangString SecEnglishDocumentationName ${CURLANG} "英文說明文件"
LangString SecFrenchDocumentationName ${CURLANG} "法文說明文件"
LangString SecLangFilesName ${CURLANG} "額外語言包"
LangString SecSourceCodeName ${CURLANG} "程式原始碼"
LangString SecStartMenuName ${CURLANG} "開始程式集選單"
LangString SecDesktopIconName ${CURLANG} "桌面圖示"
LangString SecDebugName ${CURLANG} "除錯程式"

LangString SecFileZillaDesc ${CURLANG} "安裝 FileZilla.exe 和其他必要的檔案"
LangString SecDocumentationDesc ${CURLANG} "複製說明文件到程式目錄"
LangString SecEnglishDocumentationDesc ${CURLANG} "安裝英文說明文件"
LangString SecFrenchDocumentationDesc ${CURLANG} "下載並安裝英文說明文件"
LangString SecLangFilesDesc ${CURLANG} "複製 FileZilla 的額外語言包到程式目錄"
LangString SecSourceCodeDesc ${CURLANG} "下載並複製程式原始碼到程式目錄"
LangString SecStartMenuDesc ${CURLANG} "建立選單到開始程式集"
LangString SecDesktopIconDesc ${CURLANG} "在桌面上新增一個 FileZilla 的捷徑"
LangString SecDebugDesc ${CURLANG} "複製除錯程式，這些除錯程式能在 FileZilla 發生 crash 時對程式進行解析與幫助"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "基本設定"
LangString PageSettingsSubTitle ${CURLANG} "設定安全模式以及設定檔儲存位置"
LangString PageDownloadTitle ${CURLANG} "下載選項"
LangString PageDownloadSubTitle ${CURLANG} "需要下載一些元件"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "你希望 ${MUI_PRODUCT} 運作在安全模式？\n在安全模式之中，FileZilla 不會自動記憶任何的密碼。\n如果這台電腦有很多人在使用，建議你選擇 'Yes'。"
LangString UseSecureMode ${CURLANG} "使用安全模式"
LangString DontUseSecureMode ${CURLANG} "不使用安全模式"
LangString StorageQuestion ${CURLANG} "FileZilla 能夠把設定儲存在 registry 或 XML 檔案中。\r\n如果你是在多人環境中使用，建議你以 registry 來儲存設定。\r\n否則，我們建議大部分的使用者選擇使用 XML 檔案來儲存設定。"
LangString UseXML ${CURLANG} "使用 XML 檔案來儲存設定(&X)"
LangString UseRegistry ${CURLANG} "使用 registry 來儲存設定(&r)"
LangString ForceUseRegistry ${CURLANG} "不論 XML 檔案是否存在都強迫以 registry 來儲存設定(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "為了讓安裝程式更精簡，下列元件將沒有包含在安裝程式中:"
LangString DownloadDialog3 ${CURLANG} "為了安裝這些元件，安裝程式將會下載這些必要的檔案。如果你不願意現在下載這些檔案，你可以以後再執行安裝程式，或者自己到 http://filezilla.sourceforge.net 網站中下載。"
LangString DownloadDialog4 ${CURLANG} "從 Internet 進行下載和安裝"
LangString DownloadDialog5 ${CURLANG} "跳過這些元件"
LangString DownloadDialog6 ${CURLANG} "使用本機的套件目錄 (請輸入在下面):"
LangString DownloadDialog8 ${CURLANG} "當使用這個套件目錄時，安裝程式會把所有下載回來的檔案存入這個目錄中。而安裝程式也會檢查套件目錄，如果需要的檔案沒有在套件目錄之中，才會下載這些沒有的檔案。"

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 下載中..."
LangString DownloadDownloadFailed ${CURLANG} "  下載失敗: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  下載取消" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "下載 $0 失敗。原因: $R1$\n這個元件將不會被安裝。$\n你可以稍後再執行一次安裝程式，重新下載這個元件。$\n如果這個錯誤依然存在，也許是因為安裝程式無法連上 Internet。不管是任何原因，你都可以從 http://filezilla.sourceforge.net 網站中下載這些元件。"
LangString DownloadDownloadSuccessful ${CURLANG} "  下載成功" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "將 $0 的內容解壓縮失敗。原因: $R1$\n這個元件將不會被安裝。"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "刪除所有由 FileZilla 建立的 Registry 設定 (如果你是以 Registry 來儲存設定，那麼「站台管理員」內的紀錄也會一起被刪除)?"

!verbose 4