;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Satoshi Osuga (mailto:osuga@users.sourceforge.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_JAPANESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Japanese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "標準"
LangString InstTypeFullName ${CURLANG} "完全"
LangString SecFileZillaName ${CURLANG} "FileZilla (必須)"
LangString SecDocumentationName ${CURLANG} "ドキュメント"
LangString SecEnglishDocumentationName ${CURLANG} "英語ドキュメント"
LangString SecFrenchDocumentationName ${CURLANG} "フランス語ドキュメント"
LangString SecLangFilesName ${CURLANG} "追加の言語ファイル"
LangString SecSourceCodeName ${CURLANG} "ソースコード"
LangString SecStartMenuName ${CURLANG} "スタートメニューのショートカット"
LangString SecDesktopIconName ${CURLANG} "デスクトップアイコン"
LangString SecDebugName ${CURLANG} "デバッグ用ファイル"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exeと他の必要なファイルをインストール"
LangString SecDocumentationDesc ${CURLANG} "ドキュメントファイルを対象の場所にコピー"
LangString SecEnglishDocumentationDesc ${CURLANG} "英語ドキュメントのインストール"
LangString SecFrenchDocumentationDesc ${CURLANG} "フランス語ドキュメントのダウンロードとインストール"
LangString SecLangFilesDesc ${CURLANG} "FileZillaの言語ファイルを対象の場所にコピー"
LangString SecSourceCodeDesc ${CURLANG} "ソースコードのダウンロードと対象の場所へのコピー."
LangString SecStartMenuDesc ${CURLANG} "スタートメニューのショートカットを作成"
LangString SecDesktopIconDesc ${CURLANG} "FileZillaへ素早くアクセスするためのデスクトップアイコンを追加"
LangString SecDebugDesc ${CURLANG} "クラッシュを分析する助けになるデバッグ用ファイルのコピー"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "一般的な設定"
LangString PageSettingsSubTitle ${CURLANG} "セキュアモードの設定と保存場所の設定"
LangString PageDownloadTitle ${CURLANG} "ダウンロードオプション"
LangString PageDownloadSubTitle ${CURLANG} "幾つかのコンポーネントはダウンロードする必要があります"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "${MUI_PRODUCT}をセキュアモードで起動しますか?\r\nセキュアモードでは, FileZillaはパスワードを記憶しません.\r\n複数の人がこのPCにアクセスするのであればセキュアモードの使用が推奨されます."
LangString UseSecureMode ${CURLANG} "セキュアモードを使用する(&U)"
LangString DontUseSecureMode ${CURLANG} "セキュアモードを使用しない(&n)"
LangString StorageQuestion ${CURLANG} "FileZillaは設定を異なる2つの場所に保存することができます: レジストリとXMLファイルです.\r\nほとんどの場合はXMLの使用が推奨されます.\r\nそれぞれのユーザが各自のプロファイルを持つマルチユーザ環境であれば, 同様にそれぞれのユーザが各自のFileZillaの設定を持つことができるようにレジストリを保存場所として使用するべきです."
LangString UseXML ${CURLANG} "&XMLファイルを使用"
LangString UseRegistry ${CURLANG} "レジストリを使用(&r)"
LangString ForceUseRegistry ${CURLANG} "XMLファイルが存在しても強制的にレジストリを使用(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "以下のコンポーネントはインストーラのサイズを減らすため含まれていません:"
LangString DownloadDialog3 ${CURLANG} "これらのコンポーネントをインストールするために, インストーラは要求されたファイルをダウンロードできます. これらのファイルを今ダウンロードしたくないのであれば, インストーラを後で再び起動するか, 手作業でhttp://filezilla.sourceforge.netからダウンロードできます."
LangString DownloadDialog4 ${CURLANG} "インターネットからのダウンロードとインストール"
LangString DownloadDialog5 ${CURLANG} "これらのコンポーネントをスキップ"
LangString DownloadDialog6 ${CURLANG} "ローカルのパッケージディレクトリを使用(以下に入力):"
LangString DownloadDialog8 ${CURLANG} "パッケージディレクトリを使用する場合, インストーラはダウンロードしたファイル全てをこのディレクトリに保存します. その場合はパッケージディレクトリにまだ存在していないファイルをダウンロードするだけです."

;Download strings
LangString DownloadDownloading ${CURLANG} "$1のダウンロード中"
LangString DownloadDownloadFailed ${CURLANG} "  ダウンロード失敗: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  ダウンロードがキャンセルされました" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "ダウンロード失敗 $0. 理由: $R1$\nこのコンポーネントはインストールされません.$\n後でインストーラを再度起動してこのコンポーネントのダウンロードを再試行して下さい.$\nこのエラーが続くようであれば, インストーラはインターネットにアクセスできないかもしれません. どんな場合でもコンポーネントはhttp://filezilla.sourceforge.netから手動でダウンロードできます"
LangString DownloadDownloadSuccessful ${CURLANG} "  ダウンロード成功" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0 のコンテンツの抽出に失敗しました. 理由: $R1$\nこのコンポーネントはインストールされません."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "FileZillaによって作られた全てのレジストリキーを削除 (サイトマネージャのエントリの保存場所としてレジストリを選んだ場合, それも含まれます)?"

!verbose 4
