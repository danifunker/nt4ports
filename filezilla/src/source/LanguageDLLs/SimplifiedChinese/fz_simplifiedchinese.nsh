;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Thunder White (mailto:thunder@cumt.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_SIMPCHINESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Simplified Chinese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "标准安装"
LangString InstTypeFullName ${CURLANG} "完全安装"
LangString SecFileZillaName ${CURLANG} "FileZilla (必须)"
LangString SecDocumentationName ${CURLANG} "帮助文件"
LangString SecEnglishDocumentationName ${CURLANG} "英语帮助文件"
LangString SecFrenchDocumentationName ${CURLANG} "法语帮助文件"
LangString SecLangFilesName ${CURLANG} "其他语言包"
LangString SecSourceCodeName ${CURLANG} "源程序"
LangString SecStartMenuName ${CURLANG} "开始菜单快捷方式"
LangString SecDesktopIconName ${CURLANG} "桌面图标"
LangString SecDebugName ${CURLANG} "调试文件"

LangString SecFileZillaDesc ${CURLANG} "安装 FileZilla.exe 和其他必需的文件"
LangString SecDocumentationDesc ${CURLANG} "复制文档到指定的位置"
LangString SecEnglishDocumentationDesc ${CURLANG} "安装英文帮助文件"
LangString SecFrenchDocumentationDesc ${CURLANG} "下载并安装英文帮助文件"
LangString SecLangFilesDesc ${CURLANG} "复制 FileZilla 的语言包文件到指定的位置"
LangString SecSourceCodeDesc ${CURLANG} "下载源程序并复制到指定位置."
LangString SecStartMenuDesc ${CURLANG} "在开始菜单中创建快捷方式"
LangString SecDesktopIconDesc ${CURLANG} "在桌面为 FileZilla 建立快捷方式"
LangString SecDebugDesc ${CURLANG} "复制用来帮助分析程序错误的调试文件"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "一般设置"
LangString PageSettingsSubTitle ${CURLANG} "配置安全模式以及设置的保存位置"
LangString PageDownloadTitle ${CURLANG} "下载选项"
LangString PageDownloadSubTitle ${CURLANG} "需要下载一些组件"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "您希望以安全模式运行 ${MUI_PRODUCT} 么？\n在安全模式下，FileZilla 不会记住用户名和密码。\n如果有多人使用这台机器，建议您选择使用安全模式。"
LangString UseSecureMode ${CURLANG} "使用安全模式"
LangString DontUseSecureMode ${CURLANG} "不使用安全模式"
LangString StorageQuestion ${CURLANG} "FileZilla 能够把设置保存在两个不同的地方：注册表和 XML 文件。\r\n一般建议使用 XML 文件。\r\n如果你工作在每个用户都有自己的设置文件的多用户环境，你应该选择使用注册表以便让每个用户都有自己的 FileZilla 设置。"
LangString UseXML ${CURLANG} "使用 XML 文件(&X)"
LangString UseRegistry ${CURLANG} "使用注册表(&R)"
LangString ForceUseRegistry ${CURLANG} "即使 XML 文件存在也强制使用注册表(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "为了精简安装程序，本安装程序中没有包含以下组件："
LangString DownloadDialog3 ${CURLANG} "要安装这些组件，安装程序可以下载需要的的文件。如果你现在不想下载这些文件，你以后可以再次运行此安装程序，或者你也可以从 http://filezilla.sourceforge.net 手工下载这些文件。"
LangString DownloadDialog4 ${CURLANG} "从互联网下载并安装"
LangString DownloadDialog5 ${CURLANG} "跳过这些组件"
LangString DownloadDialog6 ${CURLANG} "使用本地的组件目录 (请在下面输入)："
LangString DownloadDialog8 ${CURLANG} "如果使用组件目录，安装程序将把下载的所有文件都放在这个目录里。它将会下载组件目录里没有的那些文件。"

;Download strings
LangString DownloadDownloading ${CURLANG} "正在下载 $1"
LangString DownloadDownloadFailed ${CURLANG} "  下载失败：$0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  下载取消" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "下载 $0 失败。原因：$R1$\n此组件将不会被安装。$\n要下在这个组件，请再次运行安装程序。$\n如果错误依然存在，可是能安装程序无法访问互联网，不管是什么原因，你都可以从 http://filezilla.sourceforge.net 手动下载这些文件。"
LangString DownloadDownloadSuccessful ${CURLANG} "  下载成功" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "解压缩 $0 失败。原因：$R1$\n此组件将不会被安装。"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "确实要删除 FileZilla 创建的所有注册表项么(如果你选择了保存在注册表，则站点管理器中的记录也将被一起删除)？"

!verbose 4