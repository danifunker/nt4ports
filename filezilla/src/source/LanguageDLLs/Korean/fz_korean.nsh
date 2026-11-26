;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Kim Jinwook

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_KOREAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Korean"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "기본"
LangString InstTypeFullName ${CURLANG} "Full"
LangString SecFileZillaName ${CURLANG} "FileZilla (필수)"
LangString SecDocumentationName ${CURLANG} "문서"
LangString SecEnglishDocumentationName ${CURLANG} "영어 문서"
LangString SecFrenchDocumentationName ${CURLANG} "프랑스어 문서"
LangString SecLangFilesName ${CURLANG} "추가적인 언어 파일들"
LangString SecSourceCodeName ${CURLANG} "소스 코드"
LangString SecStartMenuName ${CURLANG} "시작 메뉴 바로가기"
LangString SecDesktopIconName ${CURLANG} "바탕화면 아이콘"
LangString SecDebugName ${CURLANG} "디버그 파일들"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exe 와 다른 필수 파일들을 설치합니다."
LangString SecDocumentationDesc ${CURLANG} "설치될 폴더에 문서들을 복사합니다."
LangString SecEnglishDocumentationDesc ${CURLANG} "영어 문서를 설치합니다"
LangString SecFrenchDocumentationDesc ${CURLANG} "영어 문서를 다운로드 후 설치합니다."
LangString SecLangFilesDesc ${CURLANG} "설치 폴더에 fileZilla 의 언어 파일들을 복사합니다."
LangString SecSourceCodeDesc ${CURLANG} "설치 폴더에 소스코드를 다운로드 후 복사합니다."
LangString SecStartMenuDesc ${CURLANG} "시작 메뉴에 바로가기를 생성합니다."
LangString SecDesktopIconDesc ${CURLANG} "FileZilla 의 바탕화면 바로가기를 생성합니다."
LangString SecDebugDesc ${CURLANG} "디버깅을 돕기 위한 디버그 파일들을 복사합니다."
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "일반 설정"
LangString PageSettingsSubTitle ${CURLANG} "보안 모드와 설정 저장위치 설정"
LangString PageDownloadTitle ${CURLANG} "다운로드 옵션들"
LangString PageDownloadSubTitle ${CURLANG} "특정 컴포넌트들은 다운로드 되어야 함"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "${MUI_PRODUCT} 를 보안 모드에서 실행하시겠습니까?\r\n보안 모드에서는 FileZilla 는 어떤 암호도 기억하지 않습니다.\r\n만약 여러명이 이 PC 를 쓴다면 보안 모드를 사용하기를 권장합니다."
LangString UseSecureMode ${CURLANG} "보안 모드 사용(&U)"
LangString DontUseSecureMode ${CURLANG} "보안 모드 사용 안함(&N)"
LangString StorageQuestion ${CURLANG} "FileZilla 는 설정을 XML 파일과 레지스트리 두 곳에 저장할 수 있습니다.\r\n대부분의 경우 XML 을 쓰는 것을 권장합니다.\r\n만약 각자의 프로파일을 이용하는 다중 사용자 환경이라면, 각자의 설정을 위한 저장장소로 레지스트리를 써야합니다."
LangString UseXML ${CURLANG} "XML 파일 사용(&X)"
LangString UseRegistry ${CURLANG} "레지스트리 사용(&R)"
LangString ForceUseRegistry ${CURLANG} "XML 파일이 이미 있더라도 레지스트리를 사용(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "아래의 컴포넌트들은 인스톨러의 크기를 줄이기 위해서 인스톨러에 포함되지 않았습니다:"
LangString DownloadDialog3 ${CURLANG} "이 컴포넌트들을 설치하기 위해서는 인스톨러가 필요한 파일들을 다운로드할 것입니다. 만약 지금 파일들을 다운로드하기를 원하지 않는다면, 나중에 인스톨러를 실행시킬 수 있습니다. 그렇지 않으면 http://filezilla.sourceforge.net 에서 수동으로 다운받을 수 있습니다."
LangString DownloadDialog4 ${CURLANG} "인터넷으로 다운받아 설치"
LangString DownloadDialog5 ${CURLANG} "이 컴포넌트들은 건너뜀"
LangString DownloadDialog6 ${CURLANG} "로컬 패키지 디렉토리 사용(아래에 입력):"
LangString DownloadDialog8 ${CURLANG} "패키지 디렉토리를 사용할 때, 인스톨러는 아래의 디렉토리에 모든 다운로드된 파일을 저장할 것입니다. 그리고 또한 패키지에 존재하지 않는 파일들을 다운로드할 것 입니다."

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 다운로드 중"
LangString DownloadDownloadFailed ${CURLANG} "  다운로드 실패: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  다운로드 취소됨" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "$0 을 다운로드 하는 데 실패함. 이유: $R1$\n이 컴포넌트는 설치되지 않을 것입니다.$\n이 컴포넌트를 다시 다운로드하기 위해서는 나중에 다시 인스톨러를 실행시키세요.$\n만약 이 에러가 계속 있다면, 인스톨러가 인터넷에 접근할 수 없는 경우 입니다. 어떤 경우든 http://filezilla.sourceforge.net 에서 직접 이 컴포넌트를 다운받을 수 있습니다."
LangString DownloadDownloadSuccessful ${CURLANG} "  다운로드 성공" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0 을 추출하는데 실패했습니다. 이유: $R1$\n이 컴포넌트는 설치되지 않을 것 입니다."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "FileZilla 가 생성한 모든 레지스트리 키를 삭제합니까(사이트 관리자 항목의 저장위치를 레지스트리로 했다면 사이트 관리자의 모든 항목도 포함됩니다)?"

!verbose 4