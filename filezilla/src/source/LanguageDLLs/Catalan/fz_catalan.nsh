;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Oscar Martinez (mailto:omllobet@telefonica.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_CATALAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Catalan"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Estàndard"
LangString InstTypeFullName ${CURLANG} "Completa"
LangString SecFileZillaName ${CURLANG} "FileZilla (requerit)"
LangString SecDocumentationName ${CURLANG} "Documentació"
LangString SecEnglishDocumentationName ${CURLANG} "Documentació en anglès"
LangString SecFrenchDocumentationName ${CURLANG} "Documentació en francès"
LangString SecLangFilesName ${CURLANG} "Fitxer d'idiomes adicicionals"
LangString SecSourceCodeName ${CURLANG} "Codi font"
LangString SecStartMenuName ${CURLANG} "Dreceres al menú Inicia"
LangString SecDesktopIconName ${CURLANG} "Icona a l'escriptori"
LangString SecDebugName ${CURLANG} "Fitxers de depuració"

LangString SecFileZillaDesc ${CURLANG} "Instal·la FileZilla.exe i altres fitxers requerits"
LangString SecDocumentationDesc ${CURLANG} "Copia els fitxers de documentació a la localització de destí"
LangString SecEnglishDocumentationDesc ${CURLANG} "Instal·la la documentació en anglès"
LangString SecFrenchDocumentationDesc ${CURLANG} "Descarrega i intal·la la documentació en francès"
LangString SecLangFilesDesc ${CURLANG} "Copia els fitxers d'idiomes a la locaclització de destí"
LangString SecSourceCodeDesc ${CURLANG} "Descarrega i copia el codi font a la localització de destí."
LangString SecStartMenuDesc ${CURLANG} "Crea dreceres al menu Inicia"
LangString SecDesktopIconDesc ${CURLANG} "Afegeix a l'escriptori una icona per accedir ràpidament a FileZilla"
LangString SecDebugDesc ${CURLANG} "Copia els fitxer de depuració qe ajuden a analitzar les fallades"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Configuració General"
LangString PageSettingsSubTitle ${CURLANG} "Configura el mode segur i el desament del fitxer de configuració"
LangString PageDownloadTitle ${CURLANG} "Opcions de descàrrega"
LangString PageDownloadSubTitle ${CURLANG} "Alguns components han de ser descarregats."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Executar ${MUI_PRODUCT} en mode segur?\r\nEn mode segur,FileZilla no memoritza cap contrasenya.\r\nÉs aconsellable utilitzar el mode segur si diferents persones tene accés a aquest equip."
LangString UseSecureMode ${CURLANG} "&Utilitza el mode segur"
LangString DontUseSecureMode ${CURLANG} "&No utilitzis el  mode segur"
LangString StorageQuestion ${CURLANG} "FileZilla pot desar les teves preferències en dues localitzacions: Al registre i en un fitxer XML.\r\nEn molts casos es recomana utilitzar el fitxer XML.\r\nSi ets en un entorn multi-usuari on cada usuari té el seu propi perfil, hauríes d'usar el registre com a localització per desar les preferències per a què cada usuari pugui desar les seves preferències pel Filezilla."
LangString UseXML ${CURLANG} "Utilitza un fitxer &XML"
LangString UseRegistry ${CURLANG} "utilitza el &registre"
LangString ForceUseRegistry ${CURLANG} "&Força el registre encara que existeixi un fitxer XML"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Aquests componets no estàn inclosos en l'instal·lador per reduir la seva grandària:"
LangString DownloadDialog3 ${CURLANG} "Per a instal·lar aquests components, l'instal·lador es pot descarregar els fitxers requerits. Si no vol descarregar aquests fitxers ara, pots executar l'intal·lador un altre cop més tard, o por descarregar els fitxers manualment de http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Descarrega i instal·la els fitxers des de internet"
LangString DownloadDialog5 ${CURLANG} "Omet aquess components"
LangString DownloadDialog6 ${CURLANG} "Utilitza el següent directori local de paquests (introdueixi a sota):"
LangString DownloadDialog8 ${CURLANG} "Quan s'utilitza el directori de paquets, l'intal·lador desarà tots els fitxers en aquest directories. Només descarregarà els fitxers que no estan presents en el directori de paquets."

;Download strings
LangString DownloadDownloading ${CURLANG} "Descarregant $1"
LangString DownloadDownloadFailed ${CURLANG} "  La descàrrega ha fallat: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Descàrrega cancel·lada" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "La descàrrega ha fallat $0. Causa: $R1$\nAquest componets no serà intal·lat.$\nExecuti l'instal·lador més tard i torna a provar a descarregar el component.$\nSi aquest error perdura, l'intal·lador pot no tenier accés a internet. En qualsevol cas pots descarregar els components manualment des de http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Descàrrega correcta" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "L'extracció de continguts de $0 ha fallat. Causa: $R1$\nAquest component no serà instal·lat."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Vol eliminar totes les claus de refistre creades per FileZilla (incloent el Gestor de llocs si vas triar el registre com a locaclització per desar les preferències)?"

!verbose 4