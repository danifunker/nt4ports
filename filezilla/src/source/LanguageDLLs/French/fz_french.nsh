;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Fabien ILLIDE (mailto:fabienillide@users.sourceforge.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_FRENCH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Français"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"
LangString LicenseText ${CURLANG} "FileZilla is released under the GNU General Public License (GPL). Please carefully read the license. Click Next to continue."
LangString LicenseNext ${CURLAG} "&Next"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Complet"
LangString SecFileZillaName ${CURLANG} "FileZilla (requis)"
LangString SecDocumentationName ${CURLANG} "Documentation"
LangString SecEnglishDocumentationName ${CURLANG} "Documentation en anglais"
LangString SecFrenchDocumentationName ${CURLANG} "Documentation en français"
LangString SecLangFilesName ${CURLANG} "Fichiers de langues additionelles"
LangString SecSourceCodeName ${CURLANG} "Code source"
LangString SecStartMenuName ${CURLANG} "Raccourcis dans le menu Démarrer"
LangString SecDesktopIconName ${CURLANG} "Icône sur le bureau"
LangString SecDebugName ${CURLANG} "Fichiers de debogage"

LangString SecFileZillaDesc ${CURLANG} "Installe FileZilla.exe et d'autres fichiers requis"
LangString SecDocumentationDesc ${CURLANG} "Copie les fichiers de documentation vers le répertoire cible"
LangString SecEnglishDocumentationDesc ${CURLANG} "Installe la documentation en anglais"
LangString SecFrenchDocumentationDesc ${CURLANG} "Télécharge et installe la documentation en français"
LangString SecLangFilesDesc ${CURLANG} "Copie les fichiers de langues pour FileZilla vers le répertoire cible"
LangString SecSourceCodeDesc ${CURLANG} "Télécharge et copie le code source vers le répertoire cible."
LangString SecStartMenuDesc ${CURLANG} "Créé les raccourcis dans le menu Démarrer"
LangString SecDesktopIconDesc ${CURLANG} "Ajoute une icône sur le bureau pour un lancement rapide de FileZilla"
LangString SecDebugDesc ${CURLANG} "Copie les fichiers de debug pour aider l'analyse des crashes"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Paramètres généraux"
LangString PageSettingsSubTitle ${CURLANG} "Configurer le mode sécurisé et les paramètres de stockage"
LangString PageDownloadTitle ${CURLANG} "Options de téléchargement"
LangString PageDownloadSubTitle ${CURLANG} "Quelques éléments doivent être téléchargés."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Voulez-vous utiliser ${MUI_PRODUCT} en mode sécurisé ?\nEn mode sécurisé, FileZilla ne se souviendra d'aucun mot de passe.\nCe mode sécurisé est recommandé si plusieurs personnes ont accès à cet ordinateur."
LangString UseSecureMode ${CURLANG} "Utiliser le mode sécurisé"
LangString DontUseSecureMode ${CURLANG} "Ne pas utiliser le mode sécurisé"
LangString StorageQuestion ${CURLANG} "FileZilla peut enregistrer ses paramètres à deux endroits différents : Dans la base de registre ou dans un fichier XML.\r\nDans la plupart des cas, il est recommandé d'utiliser le fichier XML. Si vous êtes dans un environnement multi-utilisateurs où chaque utilisateur a son propre profil, utiliser la base de registre permets d'avoir une configuration distincte pour chaque utilisateur."
LangString UseXML ${CURLANG} "Utiliser le fichier &XML"
LangString UseRegistry ${CURLANG} "Utiliser la base de &registre"
LangString ForceUseRegistry ${CURLANG} "&Forcer l'utilisation de la base de registre même si un fichier XML existe"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Les éléments suivants ne sont pas inclus dans l'installateur pour réduire sa taille :"
LangString DownloadDialog3 ${CURLANG} "Pour installer ces éléments, l'installateur peut télécharger les fichiers requis. Si vous ne voulez pas télécharger ces fichiers maintenant, vous pourrez relancer l'installateur plus tard, ou vous pouvez télécharger les fichiers manuellement depuis http://filezilla.sourceforge.net/"
LangString DownloadDialog4 ${CURLANG} "Télécharger et installer depuis internet"
LangString DownloadDialog5 ${CURLANG} "Passer ces éléments"
LangString DownloadDialog6 ${CURLANG} "Utiliser le répertoire local de paquetage (entré ci-dessous) :"
LangString DownloadDialog8 ${CURLANG} "En utilisant le répertoire local de paquetage, l'installateur enregistrera tout les fichiers téléchargés dans ce répertoire. Il téléchargera uniquement les fichiers qui ne sont pas actuellement présents dans le répertoire local de paquetage."

;Download strings
LangString DownloadDownloading ${CURLANG} "Téléchargement de $1"
LangString DownloadDownloadFailed ${CURLANG} "  Téléchargement échoué : $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Téléchargement annulé" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Echec du téléchargement de $0. Raison : $R1$\nCet élément ne sera pas installé.$\nPlus tard, lancez à nouveau l'installateur pour réessayer de télécharger cet élément.$\nSi cette erreur persiste, l'installateur n'arrive peut-être pas à accéder à internet. Dans tout les cas vous pouvez télécharger manuellement les éléments depuis http://filezilla.sourceforge.net/"
LangString DownloadDownloadSuccessful ${CURLANG} "  Téléchargement réussi" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Impossible d'extraire le contenu de $0. Raison : $R1$\nCet élément ne sera pas installé."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Supprimer toutes les clés de la base de registre créées par FileZilla (incluant les entrées du Gestionnaire de Sites si vous avez choisis la base de registre comme emplacement des enregistrements) ?"

!verbose 4