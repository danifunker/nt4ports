/*
 * console.c: various interactive-prompt routines shared between
 * the console PuTTY tools
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "putty.h"
#include "storage.h"
#include "ssh.h"

#include "FzSFtpIpc.h"

/*
 * Clean up and exit.
 */
void cleanup_exit(int code)
{
    /*
     * Clean up.
     */
//    sk_cleanup();

    random_save_seed();
#ifdef MSCRYPTOAPI
    crypto_wrapup();
#endif

    exit(code);
}

void set_busy_status(void *frontend, int status)
{
}

void notify_remote_exit(void *frontend)
{
}

void timer_change_notify(long next)
{
}

int verify_ssh_host_key(void *frontend, char *host, int port, char *keytype,
			 char *keystr, char *fingerprint,
			 void (*callback)(void *ctx, int result), void *ctx)
{
    int ret;

    /*
     * Verify the key against the registry.
     */
    ret = verify_host_key(host, port, keytype, keystr);

    if (ret == 0)		       /* success - key matched OK */
	return 1;
    
    if (ret == 1) 
    {
	/* key was absent */
	if (!FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_NEWHOSTKEY, strlen(fingerprint)+1, fingerprint))
	    cleanup_exit(1);
    }
    else if (ret == 2)
	/*key was different */
	if (!FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_CHANGEDHOSTKEY, strlen(fingerprint)+1, fingerprint))
	    cleanup_exit(1);
	
    while (TRUE)
    {
	DWORD nID, nDataLength;
	char pData[20480];
	if (!FzSFtpIpc_ReceiveRequest(&nID, &nDataLength, pData))
	    cleanup_exit(1);
	if (nID==SFTP_DATAID_STC_HOSTKEYREPLY)
	{
	    if (nDataLength!=4)
	    {
		FzSFtpIpc_Trace("SFTP_DATAID_STC_NEWHOSTKEY->nDataLength!=4");
		cleanup_exit(1);
	    }
	    switch (*(int*)pData)
	    {
	    case -1:
		return 0;
		break;
	    case 0: //Just continue without storing the key
		break;
	    case 1:
		store_host_key(host, port, keytype, keystr);
		break;										
	    }
	    break;
	}
    }
    return 1;
}

void update_specials_menu(void *frontend)
{
}

/*
 * Ask whether the selected cipher is acceptable (since it was
 * below the configured 'warn' threshold).
 * cs: 0 = both ways, 1 = client->server, 2 = server->client
 */
void askcipher(void *frontend, char *ciphername, int cs)
{
    return;
    
/*  HANDLE hin;
    DWORD savemode, i;

    static const char msg[] =
	"The first %scipher supported by the server is\n"
	"%s, which is below the configured warning threshold.\n"
	"Continue with connection? (y/n) ";
    static const char msg_batch[] =
	"The first %scipher supported by the server is\n"
	"%s, which is below the configured warning threshold.\n"
	"Connection abandoned.\n";
    static const char abandoned[] = "Connection abandoned.\n";

    char line[32];

    fprintf(stderr, msg,
	    (cs == 0) ? "" :
	    (cs == 1) ? "client-to-server " : "server-to-client ",
	    ciphername);
    fflush(stderr);

    hin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hin, &savemode);
    SetConsoleMode(hin, (savemode | ENABLE_ECHO_INPUT |
			 ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT));
    ReadFile(hin, line, sizeof(line) - 1, &i, NULL);
    SetConsoleMode(hin, savemode);

    if (line[0] == 'y' || line[0] == 'Y') {
	return;
    } else {
	fprintf(stderr, abandoned);
	cleanup_exit(0);
    }*/
}

/*
 * Ask whether the selected algorithm is acceptable (since it was
 * below the configured 'warn' threshold).
 */
int askalg(void *frontend, const char *algtype, const char *algname,
	   void (*callback)(void *ctx, int result), void *ctx)
{
    return 1;
    /*
    HANDLE hin;
    DWORD savemode, i;

    static const char msg[] =
	"The first %s supported by the server is\n"
	"%s, which is below the configured warning threshold.\n"
	"Continue with connection? (y/n) ";
    static const char msg_batch[] =
	"The first %s supported by the server is\n"
	"%s, which is below the configured warning threshold.\n"
	"Connection abandoned.\n";
    static const char abandoned[] = "Connection abandoned.\n";

    char line[32];

    if (console_batch_mode) {
	fprintf(stderr, msg_batch, algtype, algname);
	return 0;
    }

    fprintf(stderr, msg, algtype, algname);
    fflush(stderr);

    hin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hin, &savemode);
    SetConsoleMode(hin, (savemode | ENABLE_ECHO_INPUT |
			 ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT));
    ReadFile(hin, line, sizeof(line) - 1, &i, NULL);
    SetConsoleMode(hin, savemode);

    if (line[0] == 'y' || line[0] == 'Y') {
	return 1;
    } else {
	fprintf(stderr, abandoned);
	return 0;
    }*/
}


/*
 * Ask whether to wipe a session log file before writing to it.
 * Returns 2 for wipe, 1 for append, 0 for cancel (don't log).
 */
int askappend(void *frontend, Filename filename,
	      void (*callback)(void *ctx, int result), void *ctx)
{
/*    HANDLE hin;
    DWORD savemode, i;

    static const char msgtemplate[] =
	"The session log file \"%.*s\" already exists.\n"
	"You can overwrite it with a new session log,\n"
	"append your session log to the end of it,\n"
	"or disable session logging for this session.\n"
	"Enter \"y\" to wipe the file, \"n\" to append to it,\n"
	"or just press Return to disable logging.\n"
	"Wipe the log file? (y/n, Return cancels logging) ";

    static const char msgtemplate_batch[] =
	"The session log file \"%.*s\" already exists.\n"
	"Logging will not be enabled.\n";

    char line[32];

    if (console_batch_mode) {
	fprintf(stderr, msgtemplate_batch, FILENAME_MAX, filename.path);
	fflush(stderr);
	return 0;
    }
    fprintf(stderr, msgtemplate, FILENAME_MAX, filename.path);
    fflush(stderr);

    hin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hin, &savemode);
    SetConsoleMode(hin, (savemode | ENABLE_ECHO_INPUT |
			 ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT));
    ReadFile(hin, line, sizeof(line) - 1, &i, NULL);
    SetConsoleMode(hin, savemode);

    if (line[0] == 'y' || line[0] == 'Y')
	return 2;
    else if (line[0] == 'n' || line[0] == 'N')
	return 1;
    else*/
	return 0;
}

/*
 * Warn about the obsolescent key file format.
 * 
 * Uniquely among these functions, this one does _not_ expect a
 * frontend handle. This means that if PuTTY is ported to a
 * platform which requires frontend handles, this function will be
 * an anomaly. Fortunately, the problem it addresses will not have
 * been present on that platform, so it can plausibly be
 * implemented as an empty function.
 */
void old_keyfile_warning(void)
{
    static const char message[] =
	"You are loading an SSH 2 private key which has an\n"
	"old version of the file format. This means your key\n"
	"file is not fully tamperproof. Future versions of\n"
	"PuTTY may stop supporting this private key format,\n"
	"so we recommend you convert your key to the new\n"
	"format.\n"
	"\n"
	"Once the key is loaded into PuTTYgen, you can perform\n"
	"this conversion simply by saving it again.\n";

//    fputs(message, stderr);
}

void logeventc(char *string)
{
    FzSFtpIpc_Trace(string);
}

int console_get_line(const char *prompt, char *str,
			    int maxlen, int is_pw, int nType)
{
    return 0;
}

void frontend_keypress(void *handle)
{
    /*
     * This is nothing but a stub, in console code.
     */
    return;
}
