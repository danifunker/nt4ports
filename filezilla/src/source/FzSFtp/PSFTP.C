/*
 * psftp.c: front end for PSFTP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#define PUTTY_DO_GLOBALS
#include "putty.h"
#include "psftp.h"
#include "storage.h"
#include "ssh.h"
#include "sftp.h"
#include "int64.h"

#include "FzSFtpIpc.h"

/*
 * Since SFTP is a request-response oriented protocol, it requires
 * no buffer management: when we send data, we stop and wait for an
 * acknowledgement _anyway_, and so we can't possibly overfill our
 * send buffer.
 */

static int psftp_connect(char *userhost, char *user, int portnumber, int use_compression, int protocol);
static int do_sftp_init(void);
void do_sftp_cleanup();

/* ----------------------------------------------------------------------
 * sftp client state.
 */

char *pwd, *homedir;
static Backend *back;
static void *backhandle;
static Config cfg;

/* ----------------------------------------------------------------------
 * Higher-level helper functions used in commands.
 */

/*
 * Attempt to canonify a pathname starting from the pwd. If
 * canonification fails, at least fall back to returning a _valid_
 * pathname (though it may be ugly, eg /home/simon/../foobar).
 */
char *canonify(char *name)
{
    char *fullname, *canonname;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;

    if (name[0] == '/') {
	fullname = dupstr(name);
    } else {
	char *slash;
	if (pwd[strlen(pwd) - 1] == '/')
	    slash = "";
	else
	    slash = "/";
	fullname = dupcat(pwd, slash, name, NULL);
    }

    sftp_register(req = fxp_realpath_send(fullname));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    canonname = fxp_realpath_recv(pktin, rreq);

    if (canonname) {
	sfree(fullname);
	return canonname;
    } else {
	/*
	 * Attempt number 2. Some FXP_REALPATH implementations
	 * (glibc-based ones, in particular) require the _whole_
	 * path to point to something that exists, whereas others
	 * (BSD-based) only require all but the last component to
	 * exist. So if the first call failed, we should strip off
	 * everything from the last slash onwards and try again,
	 * then put the final component back on.
	 * 
	 * Special cases:
	 * 
	 *  - if the last component is "/." or "/..", then we don't
	 *    bother trying this because there's no way it can work.
	 * 
	 *  - if the thing actually ends with a "/", we remove it
	 *    before we start. Except if the string is "/" itself
	 *    (although I can't see why we'd have got here if so,
	 *    because surely "/" would have worked the first
	 *    time?), in which case we don't bother.
	 * 
	 *  - if there's no slash in the string at all, give up in
	 *    confusion (we expect at least one because of the way
	 *    we constructed the string).
	 */

	int i;
	char *returnname;

	i = strlen(fullname);
	if (i > 2 && fullname[i - 1] == '/')
	    fullname[--i] = '\0';      /* strip trailing / unless at pos 0 */
	while (i > 0 && fullname[--i] != '/');

	/*
	 * Give up on special cases.
	 */
	if (fullname[i] != '/' ||      /* no slash at all */
	    !strcmp(fullname + i, "/.") ||	/* ends in /. */
	    !strcmp(fullname + i, "/..") ||	/* ends in /.. */
	    !strcmp(fullname, "/")) {
	    return fullname;
	}

	/*
	 * Now i points at the slash. Deal with the final special
	 * case i==0 (ie the whole path was "/nonexistentfile").
	 */
	fullname[i] = '\0';	       /* separate the string */
	if (i == 0) {
	    sftp_register(req = fxp_realpath_send("/"));
	} else {
	    sftp_register(req = fxp_realpath_send(fullname));
	}
	rreq = sftp_find_request(pktin = sftp_recv());
	assert(rreq == req);
	canonname = fxp_realpath_recv(pktin, rreq);

	if (!canonname)
	    return fullname;	       /* even that failed; give up */

	/*
	 * We have a canonical name for all but the last path
	 * component. Concatenate the last component and return.
	 */
	returnname = dupcat(canonname,
			    canonname[strlen(canonname) - 1] ==
			    '/' ? "" : "/", fullname + i + 1, NULL);
	sfree(fullname);
	sfree(canonname);
	return returnname;
    }
}

/*
 * Return a pointer to the portion of str that comes after the last
 * slash (or backslash or colon, if `local' is TRUE).
 */
static char *stripslashes(char *str, int local)
{
    char *p;

    if (local) {
        p = strchr(str, ':');
        if (p) str = p+1;
    }

    p = strrchr(str, '/');
    if (p) str = p+1;

    if (local) {
	p = strrchr(str, '\\');
	if (p) str = p+1;
    }

    return str;
}

/* ----------------------------------------------------------------------
 * Actual sftp commands.
 */
struct sftp_command {
    char **words;
    int nwords, wordssize;
    int (*obey) (struct sftp_command *);	/* returns <0 to quit */
};

/*
 * List a directory. If no arguments are given, list pwd; otherwise
 * list the directory given in words[1].
 */
static int sftp_ls_compare(const void *av, const void *bv)
{
    const struct fxp_name *const *a = (const struct fxp_name *const *) av;
    const struct fxp_name *const *b = (const struct fxp_name *const *) bv;
    return strcmp((*a)->filename, (*b)->filename);
}
int sftp_cmd_ls()
{
    struct fxp_handle *dirh;
    struct fxp_names *names;
    struct fxp_name **ournames;
    int nnames, namesize;
    char *dir, *cdir;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    int i;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    dir = ".";
    
    cdir = canonify(dir);
    if (!cdir) {
	FzSFtpIpc_Error2("%s: %s", dir, fxp_error());
	return 0;
    }

    printf("Listing directory %s\n", cdir);

    sftp_register(req = fxp_opendir_send(cdir));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    dirh = fxp_opendir_recv(pktin, rreq);

    if (dirh == NULL) {
	FzSFtpIpc_Error2("Unable to open %s: %s", dir, fxp_error());
    } else {
	int num = 0;
	nnames = namesize = 0;
	ournames = NULL;

	while (1) {

	    sftp_register(req = fxp_readdir_send(dirh));
	    rreq = sftp_find_request(pktin = sftp_recv());
	    assert(rreq == req);
	    names = fxp_readdir_recv(pktin, rreq);

	    if (names == NULL) {
		if (fxp_error_type() == SSH_FX_EOF)
		    break;
		FzSFtpIpc_Error2("Reading directory %s: %s", dir, fxp_error());
		break;
	    }
	    if (names->nnames == 0) {
		fxp_free_names(names);
		break;
	    }

	    if (nnames + names->nnames >= namesize) {
		namesize += names->nnames + 128;
		ournames = sresize(ournames, namesize, struct fxp_name *);
	    }

	    for (i = 0; i < names->nnames; i++)
		ournames[nnames++] = fxp_dup_name(&names->names[i]);

	    fxp_free_names(names);
	}
	sftp_register(req = fxp_close_send(dirh));
	rreq = sftp_find_request(pktin = sftp_recv());
	assert(rreq == req);
	fxp_close_recv(pktin, rreq);

	/*
	 * Now we have our filenames. Sort them by actual file
	 * name, and then output the longname parts.
	 */
	qsort(ournames, nnames, sizeof(*ournames), sftp_ls_compare);

	/*
	 * And print them.
	 */
	for (i = 0; i < nnames; i++) {
	    if (strcmp(ournames[i]->filename, ".") && strcmp(ournames[i]->filename, "..") ) {
		num++;
		FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_LIST, strlen(ournames[i]->longname)+1, ournames[i]->longname);
	    }
	    fxp_free_name(ournames[i]);
	}
	sfree(ournames);
	
	FzSFtpIpc_Status1("Successfully received %d items", num);
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_LIST, 0, 0);
    }

    sfree(cdir);

    return 1;
}

/*
 * Change directories. We do this by canonifying the new name, then
 * trying to OPENDIR it. Only if that succeeds do we set the new pwd.
 */
int sftp_cmd_cd(char *cd)
{
    struct fxp_handle *dirh;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    char *dir;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    dir = canonify(cd);

    if (!dir) {
	FzSFtpIpc_Error2("%s: %s", cd, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_opendir_send(dir));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    dirh = fxp_opendir_recv(pktin, rreq);

    if (!dirh) {
	FzSFtpIpc_Error2("Directory %s: %s", dir, fxp_error());
	sfree(dir);
	return 0;
    }

    sftp_register(req = fxp_close_send(dirh));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    fxp_close_recv(pktin, rreq);

    sfree(pwd);
    pwd = dir;
    FzSFtpIpc_Status1("Remote working directory is now %s", pwd)
    FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_CD, strlen(pwd)+1, pwd);

    return 1;
}

/*
 * Print current directory. Easy as pie.
 */
int sftp_cmd_pwd(struct sftp_command *cmd)
{
    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    FzSFtpIpc_Status1("Remote directory is %s", pwd);
    FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_PWD, strlen(pwd)+1, pwd);

    return 1;
}

/*
 * Get a file and save it at the local end. We have two very
 * similar commands here: `get' and `reget', which differ in that
 * `reget' checks for the existence of the destination file and
 * starts from where a previous aborted transfer left off.
 */
int sftp_general_get(char *file, char *outfname, int restart)
{
    struct fxp_handle *fh;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    struct fxp_xfer *xfer;
    char *fname;
    uint64 offset;
    HANDLE fp;
    int ret;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    fname = canonify(file);
    if (!fname)
    {
	FzSFtpIpc_Error2("%s: %s", file, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_open_send(fname, SSH_FXF_READ));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    fh = fxp_open_recv(pktin, rreq);

    if (!fh) {
	FzSFtpIpc_Error2("%s: %s", fname, fxp_error());
	sfree(fname);
	return 0;
    }

    if (restart)
	fp = CreateFileA(outfname, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    else
	fp = CreateFileA(outfname, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

    if (fp==INVALID_HANDLE_VALUE)
    {
	FzSFtpIpc_Error1("Unable to open %s", outfname);
	sftp_register(req = fxp_close_send(fh));
	rreq = sftp_find_request(pktin = sftp_recv());
	assert(rreq == req);
	fxp_close_recv(pktin, rreq);

	sfree(fname);
	return 0;
    }
    
    if (restart) {
	LONG high=0;
	LONG pos=SetFilePointer(fp, 0, &high, FILE_END);
	char decbuf[30];
	offset = uint64_make(high, pos);
	uint64_decimal(offset, decbuf);
	FzSFtpIpc_Status3("Downloading %s to %s, restarting at file position %s", fname, outfname, decbuf);
    } else {
	offset = uint64_make(0, 0);
	FzSFtpIpc_Status2("Downloading %s to %s", fname, outfname);
    }

    /*
     * FIXME: we can use FXP_FSTAT here to get the file size, and
     * thus put up a progress bar.
     */
    ret = 1;
    xfer = xfer_download_init(fh, offset);
    while (!xfer_done(xfer)) {
	void *vbuf;
	int ret, len;
	int wpos, wlen;

	xfer_download_queue(xfer);
	pktin = sftp_recv();
	ret = xfer_download_gotpkt(xfer, pktin);

	if (ret < 0) {
            FzSFtpIpc_Error1("Error while reading: %s", fxp_error());
            ret = 0;
	}

	while (xfer_download_data(xfer, &vbuf, &len)) {
	    unsigned char *buf = (unsigned char *)vbuf;

	    wpos = 0;
	    while (wpos < len) {
		BOOL res = WriteFile(fp, buf + wpos, len - wpos, &wlen, NULL);
		if (!res || wlen <= 0) {
		    printf("error while writing local file\n");
		    ret = 0;
		    xfer_set_error(xfer);
		}
		wpos += wlen;
	    }
	    if (wpos < len) {	       /* we had an error */
		ret = 0;
		xfer_set_error(xfer);
	    }
	    else
		FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_TRANSFERSTATUS, 4, &len);

	    sfree(vbuf);
	}
    }

    xfer_cleanup(xfer);

    CloseHandle(fp);

    sftp_register(req = fxp_close_send(fh));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    fxp_close_recv(pktin, rreq);

    sfree(fname);

    if (ret>0)
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_GET, 0, 0);

    return ret;
}

/*
 * Send a file and store it at the remote end. We have two very
 * similar commands here: `put' and `reput', which differ in that
 * `reput' checks for the existence of the destination file and
 * starts from where a previous aborted transfer left off.
 */
int sftp_general_put(char *file, char *localfile, int restart)
{
    struct fxp_handle *fh;
    struct fxp_xfer *xfer;
    char *outfname;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    uint64 offset;
    HANDLE fp;
    int ret, err, eof;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    outfname = canonify(file);
    if (!outfname) {
	FzSFtpIpc_Error2("%s: %s", file, fxp_error());
	return 0;
    }

    fp = CreateFileA(localfile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fp == INVALID_HANDLE_VALUE)
    {
	FzSFtpIpc_Error1("Unable to open %s", localfile);
	sfree(outfname);
	return 0;
    }
    if (restart) {
	sftp_register(req = fxp_open_send(outfname, SSH_FXF_WRITE));
    } else {
	sftp_register(req = fxp_open_send(outfname, SSH_FXF_WRITE |
					  SSH_FXF_CREAT | SSH_FXF_TRUNC));
    }
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    fh = fxp_open_recv(pktin, rreq);

    if (!fh) {
	FzSFtpIpc_Error2("%s: %s", outfname, fxp_error());
	sfree(outfname);
	return 0;
    }

    if (restart) {
	LONG high, low;
	char decbuf[30];
	struct fxp_attrs attrs;
	int ret;

	sftp_register(req = fxp_fstat_send(fh));
	rreq = sftp_find_request(pktin = sftp_recv());
	assert(rreq == req);
	ret = fxp_fstat_recv(pktin, rreq, &attrs);

	if (!ret) {
	    CloseHandle(fp);
	    FzSFtpIpc_Error2("Read size of %s: %s", outfname, fxp_error());
	    sfree(outfname);
	    return 0;
	}
	if (!(attrs.flags & SSH_FILEXFER_ATTR_SIZE)) {
	    CloseHandle(fp);
	    FzSFtpIpc_Error1("Read size of %s: Size was not given", outfname);
	    sfree(outfname);
	    return 0;
	}
	offset = attrs.size;
	uint64_decimal(offset, decbuf);
	printf("reput: restarting at file position %s\n", decbuf);
	if (uint64_compare(offset, uint64_make(0, LONG_MAX)) > 0) {
	    CloseHandle(fp);
	    FzSFtpIpc_Error("reput: remote file is larger than we can deal with");
	    sfree(outfname);
	    return 0;
	}
	high = offset.hi;
	low = offset.lo;
	low = SetFilePointer(fp, low, &high, FILE_BEGIN);
	if (low == 0xFFFFFFFF && GetLastError())
	{
	    low = 0;
	    high = 0;
	    low = SetFilePointer(fp, low, &high, FILE_END);
	}
	FzSFtpIpc_Status3("Uploading %s to %s, restarting at file position %s", localfile, file, decbuf);
    } else {
	offset = uint64_make(0, 0);
	FzSFtpIpc_Status2("Uploading %s to %s", localfile, file);
    }

    /*
     * FIXME: we can use FXP_FSTAT here to get the file size, and
     * thus put up a progress bar.
     */
    ret = 1;
    xfer = xfer_upload_init(fh, offset);
    err = eof = 0;
    while ((!err && !eof) || !xfer_done(xfer)) {
	char buffer[4096 * 2];
	int len;
	while (xfer_upload_ready(xfer) && !err && !eof) {
	    BOOL res = ReadFile(fp, buffer, sizeof(buffer), &len, 0);
	    if (!res)
	    {
		FzSFtpIpc_Error("Error while reading local file");
		err = 1;
	    }
	    else if (len == 0)
	    {
		eof = 1;
	    }
	    else
	    {
		xfer_upload_data(xfer, buffer, len);
		FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_TRANSFERSTATUS, 4, &len);
	    }
	}

	if (!xfer_done(xfer))
	{
	    pktin = sftp_recv();
	    ret = xfer_upload_gotpkt(xfer, pktin);

	    if (!ret) {
	        FzSFtpIpc_Error1("Error while writing: %s", fxp_error());
	        err = 1;
	    }
	}
    }

    xfer_cleanup(xfer);

    sftp_register(req = fxp_close_send(fh));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    fxp_close_recv(pktin, rreq);

    CloseHandle(fp);
    sfree(outfname);

    if (ret>0)
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_PUT, 0, 0);

    return ret;
}

int sftp_cmd_mkdir(char *cmd)
{
    char *dir;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    int result;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    dir = canonify(cmd);
    if (!dir) {
	FzSFtpIpc_Error2("%s: %s", dir, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_mkdir_send(dir));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_mkdir_recv(pktin, rreq);

    if (!result) {
	FzSFtpIpc_Error2("mkdir %s: %s", dir, fxp_error());
	sfree(dir);
	return 0;
    }

    {
	char msg[]="Directory created successfully";
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_MKD, strlen(msg)+1, msg);
    }

    sfree(dir);
    return 1;
}

int sftp_cmd_rmdir(char *cmd)
{
    char *dir;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    int result;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    dir = canonify(cmd);
    if (!dir) {
	FzSFtpIpc_Error2("%s: %s", cmd, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_rmdir_send(dir));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_rmdir_recv(pktin, rreq);

    if (!result) {
	FzSFtpIpc_Error2("rmdir %s: %s", dir, fxp_error());
	sfree(dir);
	return 0;
    }

    {
	char msg[]="Directory removed successfully";
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_RMD, strlen(msg)+1, msg);
    }

    sfree(dir);
    return 1;
}

int sftp_cmd_rm(char *cmd)
{
    char *fname;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    int result;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    fname = canonify(cmd);
    if (!fname) {
	FzSFtpIpc_Error2("%s: %s", cmd, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_remove_send(fname));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_remove_recv(pktin, rreq);

    if (!result) {
	FzSFtpIpc_Error2("rm %s: %s", fname, fxp_error());
	sfree(fname);
	return 0;
    }

    sfree(fname);

    {
	char msg[]="File deleted successfully";
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_DELE, strlen(msg)+1, msg);
    }

    return 1;
}

int sftp_cmd_mv(char *src, char *dst)
{
    char *srcfname, *dstfname;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;
    int result;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    srcfname = canonify(src);
    if (!srcfname) {
	FzSFtpIpc_Error2("%s: %s", src, fxp_error());
	return 0;
    }

    dstfname = canonify(dst);
    if (!dstfname) {
	FzSFtpIpc_Error2("%s: %s", dst, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_rename_send(srcfname, dstfname));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_rename_recv(pktin, rreq);

    if (!result) {
	char const *error = fxp_error();
	struct fxp_attrs attrs;

	/*
	 * The move might have failed because dstfname pointed at a
	 * directory. We check this possibility now: if dstfname
	 * _is_ a directory, we re-attempt the move by appending
	 * the basename of srcfname to dstfname.
	 */
	sftp_register(req = fxp_stat_send(dstfname));
	rreq = sftp_find_request(pktin = sftp_recv());
	assert(rreq == req);
	result = fxp_stat_recv(pktin, rreq, &attrs);

	if (result &&
	    (attrs.flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
	    (attrs.permissions & 0040000)) {
	    char *p;
	    char *newname, *newcanon;
	    printf("(destination %s is a directory)\n", dstfname);
	    p = srcfname + strlen(srcfname);
	    while (p > srcfname && p[-1] != '/') p--;
	    newname = dupcat(dstfname, "/", p, NULL);
	    newcanon = canonify(newname);
	    sfree(newname);
	    if (newcanon) {
		sfree(dstfname);
		dstfname = newcanon;

		sftp_register(req = fxp_rename_send(srcfname, dstfname));
		rreq = sftp_find_request(pktin = sftp_recv());
		assert(rreq == req);
		result = fxp_rename_recv(pktin, rreq);

		error = result ? NULL : fxp_error();
	    }
	}
	if (error) {
	    FzSFtpIpc_Error3("Move/Rename %s %s: %s", srcfname, dstfname, error);
	    sfree(srcfname);
	    sfree(dstfname);
	    return 0;
	}
    }
    {
        char str[]="File moved / renamed successfully";
        FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_RENAME, strlen(str)+1, str);
    }

    sfree(srcfname);
    sfree(dstfname);
    return 1;
}

int sftp_cmd_chmod(char *value, char *filename)
{
    char *fname, *mode;
    int result;
    struct fxp_attrs attrs;
    unsigned attrs_clr, attrs_xor, oldperms, newperms;
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;

    if (back == NULL) {
	FzSFtpIpc_Error("Not connected to a host");
	return 0;
    }

    /*
     * Attempt to parse the mode specifier in cmd->words[1]. We
     * don't support the full horror of Unix chmod; instead we
     * support a much simpler syntax in which the user can either
     * specify an octal number, or a comma-separated sequence of
     * [ugoa]*[-+=][rwxst]+. (The initial [ugoa] sequence may
     * _only_ be omitted if the only attribute mentioned is t,
     * since all others require a user/group/other specification.
     * Additionally, the s attribute may not be specified for any
     * [ugoa] specifications other than exactly u or exactly g.
     */
    attrs_clr = attrs_xor = 0;
    mode = value;
    if (mode[0] >= '0' && mode[0] <= '9') {
	if (mode[strspn(mode, "01234567")]) {
	    FzSFtpIpc_Error("chmod: numeric file modes should contain digits 0-7 only");
	    return 0;
	}
	attrs_clr = 07777;
	sscanf(mode, "%o", &attrs_xor);
	attrs_xor &= attrs_clr;
    } else {
	while (*mode) {
	    char *modebegin = mode;
	    unsigned subset, perms;
	    int action;

	    subset = 0;
	    while (*mode && *mode != ',' &&
		   *mode != '+' && *mode != '-' && *mode != '=') {
		switch (*mode) {
		  case 'u': subset |= 04700; break; /* setuid, user perms */
		  case 'g': subset |= 02070; break; /* setgid, group perms */
		  case 'o': subset |= 00007; break; /* just other perms */
		  case 'a': subset |= 06777; break; /* all of the above */
		  default:
		    FzSFtpIpc_Error3("chmod: file mode '%.*s' contains unrecognised user/group/other specifier '%c'",
				      strcspn(modebegin, ","), modebegin, *mode);
			return 0;
		}
		mode++;
	    }
	    if (!*mode || *mode == ',') {
		FzSFtpIpc_Error2("chmod: file mode '%.*s' is incomplete",
		       strcspn(modebegin, ","), modebegin);
		return 0;
	    }
	    action = *mode++;
	    if (!*mode || *mode == ',') {
		printf("chmod: file mode '%.*s' is incomplete\n",
		       strcspn(modebegin, ","), modebegin);
		return 0;
	    }
	    perms = 0;
	    while (*mode && *mode != ',') {
		switch (*mode) {
		  case 'r': perms |= 00444; break;
		  case 'w': perms |= 00222; break;
		  case 'x': perms |= 00111; break;
		  case 't': perms |= 01000; subset |= 01000; break;
		  case 's':
		    if ((subset & 06777) != 04700 &&
			(subset & 06777) != 02070) {
			FzSFtpIpc_Error2("chmod: file mode '%.*s': set[ug]id bit should"
					 " be used with exactly one of u or g only",
					 strcspn(modebegin, ","), modebegin);
			return 0;
		    }
		    perms |= 06000;
		    break;
		  default:
		    FzSFtpIpc_Error3("chmod: file mode '%.*s' contains unrecognised"
				     " permission specifier '%c'",
				     strcspn(modebegin, ","), modebegin, *mode);
		    return 0;
		}
		mode++;
	    }
	    if (!(subset & 06777) && (perms &~ subset)) {
		FzSFtpIpc_Error2("chmod: file mode '%.*s' contains no user/group/other"
				 " specifier and permissions other than 't'",
				 strcspn(modebegin, ","), modebegin);
		return 0;
	    }
	    perms &= subset;
	    switch (action) {
	      case '+':
		attrs_clr |= perms;
		attrs_xor |= perms;
		break;
	      case '-':
		attrs_clr |= perms;
		attrs_xor &= ~perms;
		break;
	      case '=':
		attrs_clr |= subset;
		attrs_xor |= perms;
		break;
	    }
	    if (*mode) mode++;	       /* eat comma */
	}
    }

    fname = canonify(filename);
    if (!fname) {
	FzSFtpIpc_Error2("%s: %s", filename, fxp_error());
	return 0;
    }

    sftp_register(req = fxp_stat_send(fname));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_stat_recv(pktin, rreq, &attrs);

    if (!result || !(attrs.flags & SSH_FILEXFER_ATTR_PERMISSIONS)) {
	FzSFtpIpc_Error2("get attrs for %s: %s\n", fname,
			 result ? "file permissions not provided" : fxp_error());
	sfree(fname);
	return 0;
    }

    attrs.flags = SSH_FILEXFER_ATTR_PERMISSIONS;   /* perms _only_ */
    oldperms = attrs.permissions & 07777;
    attrs.permissions &= ~attrs_clr;
    attrs.permissions ^= attrs_xor;
    newperms = attrs.permissions & 07777;

    sftp_register(req = fxp_setstat_send(fname, attrs));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    result = fxp_setstat_recv(pktin, rreq);

    if (!result) {
	FzSFtpIpc_Error2("set attrs for %s: %s", fname, fxp_error());
	sfree(fname);
	return 0;
    }

    {
	char str[]="chmod successful";
	FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_CHMOD, strlen(str)+1, str);
    }

    sfree(fname);
    return 1;
}

void do_sftp_cleanup()
{
	char ch;
	if (back) {
		back->special(backhandle, TS_EOF);
		sftp_recvdata(&ch,	1);
		back->free(backhandle);
		sftp_cleanup_request();
	}
	if (pwd) {
		sfree(pwd);
		pwd = NULL;
	}
	if (homedir) {
		sfree(homedir);
		homedir = NULL;
	}
}

static int do_sftp_init(void)
{
    struct sftp_packet *pktin;
    struct sftp_request *req, *rreq;

    /*
     * Do protocol initialisation. 
     */
    if (!fxp_init()) {
	FzSFtpIpc_Error1("Fatal: unable to initialise SFTP: %s", fxp_error());
	return 1;		       /* failure */
    }

    /*
     * Find out where our home directory is.
     */
    sftp_register(req = fxp_realpath_send("."));
    rreq = sftp_find_request(pktin = sftp_recv());
    assert(rreq == req);
    homedir = fxp_realpath_recv(pktin, rreq);

    if (!homedir) {
	FzSFtpIpc_Status1("Warning: failed to resolve home directory: %s",
			   fxp_error());
	homedir = dupstr(".");
    } else {
	FzSFtpIpc_Status1("Remote working directory is %s", homedir);
    }
    pwd = dupstr(homedir);

    FzSFtpIpc_SendRequest(SFTP_DATAID_CTS_CONNECTED, 0, 0);

    return 0;
}

/* ----------------------------------------------------------------------
 * Dirty bits: integration with PuTTY.
 */

static int verbose = 0;

/*
 *  Print an error message and perform a fatal exit.
 */
void fatalbox(char *fmt, ...)
{
    char *str, *str2;
    va_list ap;
    va_start(ap, fmt);
    str = dupvprintf(fmt, ap);
    str2 = dupcat("Fatal: ", str, NULL);
    sfree(str);
    va_end(ap);
    FzSFtpIpc_FatalError(str2);
    sfree(str2);

    cleanup_exit(1);
}
void modalfatalbox(char *fmt, ...)
{
    char *str, *str2;
    va_list ap;
    va_start(ap, fmt);
    str = dupvprintf(fmt, ap);
    str2 = dupcat("Fatal: ", str, NULL);
    sfree(str);
    va_end(ap);
    FzSFtpIpc_FatalError(str2);
    sfree(str2);

    cleanup_exit(1);
}
void connection_fatal(void *frontend, char *fmt, ...)
{
    char *str, *str2;
    va_list ap;
    va_start(ap, fmt);
    str = dupvprintf(fmt, ap);
    str2 = dupcat("Fatal: ", str, NULL);
    sfree(str);
    va_end(ap);
    FzSFtpIpc_FatalError(str2);
    sfree(str2);

    cleanup_exit(1);
}

void ldisc_send(void *handle, char *buf, int len, int interactive)
{
    /*
     * This is only here because of the calls to ldisc_send(NULL,
     * 0) in ssh.c. Nothing in PSFTP actually needs to use the
     * ldisc as an ldisc. So if we get called with any real data, I
     * want to know about it.
     */
    assert(len == 0);
}

/*
 * In psftp, all agent requests should be synchronous, so this is a
 * never-called stub.
 */
void agent_schedule_callback(void (*callback)(void *, void *, int),
			     void *callback_ctx, void *data, int len)
{
    assert(!"We shouldn't be here");
}

/*
 * Receive a block of data from the SSH link. Block until all data
 * is available.
 *
 * To do this, we repeatedly call the SSH protocol module, with our
 * own trap in from_backend() to catch the data that comes back. We
 * do this until we have enough data.
 */

static unsigned char *outptr;	       /* where to put the data */
static unsigned outlen;		       /* how much data required */
static unsigned char *pending = NULL;  /* any spare data */
static unsigned pendlen = 0, pendsize = 0;	/* length and phys. size of buffer */
int from_backend(void *frontend, int is_stderr, const char *data, int datalen)
{
    unsigned char *p = (unsigned char *) data;
    unsigned len = (unsigned) datalen;

    /*
     * stderr data is just spouted to local stderr and otherwise
     * ignored.
     */
    if (is_stderr) {
	if (len > 0)
	    fwrite(data, 1, len, stderr);
	return 0;
    }

    /*
     * If this is before the real session begins, just return.
     */
    if (!outptr)
	return 0;

    if ((outlen > 0) && (len > 0)) {
	unsigned used = outlen;
	if (used > len)
	    used = len;
	memcpy(outptr, p, used);
	outptr += used;
	outlen -= used;
	p += used;
	len -= used;
    }

    if (len > 0) {
	if (pendsize < pendlen + len) {
	    pendsize = pendlen + len + 4096;
	    pending = sresize(pending, pendsize, unsigned char);
	}
	memcpy(pending + pendlen, p, len);
	pendlen += len;
    }

    return 0;
}
int sftp_recvdata(char *buf, int len)
{
    outptr = (unsigned char *) buf;
    outlen = len;

    /*
     * See if the pending-input block contains some of what we
     * need.
     */
    if (pendlen > 0) {
	unsigned pendused = pendlen;
	if (pendused > outlen)
	    pendused = outlen;
	memcpy(outptr, pending, pendused);
	memmove(pending, pending + pendused, pendlen - pendused);
	outptr += pendused;
	outlen -= pendused;
	pendlen -= pendused;
	if (pendlen == 0) {
	    pendsize = 0;
	    sfree(pending);
	    pending = NULL;
	}
	if (outlen == 0)
	    return 1;
    }

    while (outlen > 0) {
	if (ssh_sftp_loop_iteration() < 0)
	    return 0;		       /* doom */
    }

    return 1;
}
int sftp_senddata(char *buf, int len)
{
    back->send(backhandle, buf, len);
    return 1;
}


/*
 * Connect to a host.
 */
static int psftp_connect(char *userhost, char *user, int portnumber, int use_compression, int protocol)
{
    char *host, *realhost;
    const char *err;
    
    /* Separate host and username */
    host = userhost;
    host = strrchr(host, '@');
    if (host == NULL) {
	host = userhost;
    } else {
	*host++ = '\0';
	if (user) {
	    printf("psftp: multiple usernames specified; using \"%s\"\n",
		   user);
	} else
	    user = userhost;
    }

    /* Try to load settings for this host */
    do_defaults(host, &cfg);
    if (cfg.host[0] == '\0') {
	/* No settings for this host; use defaults */
	do_defaults(NULL, &cfg);
	strncpy(cfg.host, host, sizeof(cfg.host) - 1);
	cfg.host[sizeof(cfg.host) - 1] = '\0';
    }

    /*
     * Force use of SSH. (If they got the protocol wrong we assume the
     * port is useless too.)
     */
    if (cfg.protocol != PROT_SSH) {
        cfg.protocol = PROT_SSH;
        cfg.port = 22;
    }

    /*
     * Enact command-line overrides.
     */
    cmdline_run_saved(&cfg);

    /*
     * Trim leading whitespace off the hostname if it's there.
     */
    {
	int space = strspn(cfg.host, " \t");
	memmove(cfg.host, cfg.host+space, 1+strlen(cfg.host)-space);
    }

    /* See if host is of the form user@host */
    if (cfg.host[0] != '\0') {
	char *atsign = strchr(cfg.host, '@');
	/* Make sure we're not overflowing the user field */
	if (atsign) {
	    if (atsign - cfg.host < sizeof cfg.username) {
		strncpy(cfg.username, cfg.host, atsign - cfg.host);
		cfg.username[atsign - cfg.host] = '\0';
	    }
	    memmove(cfg.host, atsign + 1, 1 + strlen(atsign + 1));
	}
    }

    /*
     * Trim a colon suffix off the hostname if it's there.
     */
    cfg.host[strcspn(cfg.host, ":")] = '\0';

    /*
     * Remove any remaining whitespace from the hostname.
     */
    {
	int p1 = 0, p2 = 0;
	while (cfg.host[p2] != '\0') {
	    if (cfg.host[p2] != ' ' && cfg.host[p2] != '\t') {
		cfg.host[p1] = cfg.host[p2];
		p1++;
	    }
	    p2++;
	}
	cfg.host[p1] = '\0';
    }

    /* Set username */
    if (user != NULL && user[0] != '\0') {
	strncpy(cfg.username, user, sizeof(cfg.username) - 1);
	cfg.username[sizeof(cfg.username) - 1] = '\0';
    }
    if (!cfg.username[0]) {
	printf("login as: ");
	fflush(stdout);
	if (!fgets(cfg.username, sizeof(cfg.username), stdin)) {
	    fprintf(stderr, "psftp: aborting\n");
	    cleanup_exit(1);
	} else {
	    int len = strlen(cfg.username);
	    if (cfg.username[len - 1] == '\n')
		cfg.username[len - 1] = '\0';
	}
    }

    if (portnumber)
	cfg.port = portnumber;

    /* SFTP uses SSH2 by default always */
    cfg.sshprot = 2;

    /* Override compression and protocol */
    if (use_compression == 1)
	cfg.compression = 1;
    else if (use_compression == 2)
	cfg.compression = 0;
    if (protocol == 1)
	cmdline_process_param("-1", 0, 0, &cfg);
    else if (protocol == 2)
	cmdline_process_param("-2", 0, 0, &cfg);
    
    /*
     * Disable scary things which shouldn't be enabled for simple
     * things like SCP and SFTP: agent forwarding, port forwarding,
     * X forwarding.
     */
    cfg.x11_forward = 0;
    cfg.agentfwd = 0;
    cfg.portfwd[0] = cfg.portfwd[1] = '\0';

    /* Set up subsystem name. */
    strcpy(cfg.remote_cmd, "sftp");
    cfg.ssh_subsys = TRUE;
    cfg.nopty = TRUE;

    /*
     * Set up fallback option, for SSH1 servers or servers with the
     * sftp subsystem not enabled but the server binary installed
     * in the usual place. We only support fallback on Unix
     * systems, and we use a kludgy piece of shellery which should
     * try to find sftp-server in various places (the obvious
     * systemwide spots /usr/lib and /usr/local/lib, and then the
     * user's PATH) and finally give up.
     * 
     *   test -x /usr/lib/sftp-server && exec /usr/lib/sftp-server
     *   test -x /usr/local/lib/sftp-server && exec /usr/local/lib/sftp-server
     *   exec sftp-server
     * 
     * the idea being that this will attempt to use either of the
     * obvious pathnames and then give up, and when it does give up
     * it will print the preferred pathname in the error messages.
     */
    cfg.remote_cmd_ptr2 =
	"test -x /usr/lib/sftp-server && exec /usr/lib/sftp-server\n"
	"test -x /usr/local/lib/sftp-server && exec /usr/local/lib/sftp-server\n"
	"exec sftp-server";
    cfg.ssh_subsys2 = FALSE;

    back = &ssh_backend;

    err = back->init(NULL, &backhandle, &cfg, cfg.host, cfg.port, &realhost,
						0, cfg.tcp_keepalives);
    if (err != NULL) {
	fprintf(stderr, "ssh_init: %s\n", err);
	return 1;
    }
    while (!back->sendok(backhandle)) {
	if (ssh_sftp_loop_iteration() < 0) {
	    fprintf(stderr, "ssh_init: error during SSH connection setup\n");
	    return 1;
	}
    }
    if (verbose && realhost != NULL)
	printf("Connected to %s\n", realhost);
    return 0;
}

void cmdline_error(char *p, ...)
{
    va_list ap;
    fprintf(stderr, "psftp: ");
    va_start(ap, p);
    vfprintf(stderr, p, ap);
    va_end(ap);
    fprintf(stderr, "\n       try typing \"psftp -h\" for help\n");
    exit(1);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bLTrace=TRUE;
	flags = FLAG_STDERR | FLAG_INTERACTIVE | FLAG_VERBOSE | FLAG_SYNCAGENT;
	cmdline_tooltype = TOOLTYPE_FILETRANSFER;
	ssh_get_line = &console_get_line;
	sk_init();

	if (!FzSFtpIpc_Init(lpCmdLine))
	{
		MessageBoxA(0, "Error: You can't start FzSFtp directly. It can only by started from FileZilla when connecting to an SFTP server.", "FzSFtp Error", MB_OK);
		return 1;
	}

	FzSFtpIpc_Trace("FzSFtp started and initialized.");
	while (TRUE)
	{
		char pMem[20480];
		DWORD nID, nDataLength;
		BOOL res=FzSFtpIpc_ReceiveRequest(&nID, &nDataLength, pMem);
		if (!res)
			break;
		switch(nID)
		{
		case SFTP_DATAID_STC_CONNECT:
			if (!nDataLength)
				cleanup_exit(1);
			{
			char *host;
			int port;
			char *user;
			char *pass;
			int use_compression;
			int protocol;

			host = pMem;
			port = *(int *)(pMem+strlen(host)+1);
			user = pMem+strlen(host)+1+4;
			pass = pMem+strlen(host)+1+4 + strlen(user) + 1;
			use_compression = *(pMem+strlen(host)+1+4 + strlen(user) + 1 + strlen(pass) + 1);
			protocol = *(pMem+strlen(host)+1+4 + strlen(user) + 1 + strlen(pass) + 2);
			cmdline_process_param("-pw", pass, 0, &cfg);
			if (psftp_connect (host, user, port, use_compression, protocol)<0)
				cleanup_exit(1);
			if (do_sftp_init())
				cleanup_exit(1);
			}
			break;
		case SFTP_DATAID_STC_PWD:
			if (sftp_cmd_pwd(0)<0)
				cleanup_exit(1);
			break;
		case SFTP_DATAID_STC_LIST:
			if (sftp_cmd_ls()<0)
				cleanup_exit(1);
			break;
		case SFTP_DATAID_STC_CD:
			if (!nDataLength)
				cleanup_exit(1);
			if (sftp_cmd_cd(pMem)<0)
				cleanup_exit(1);
			break;
		case SFTP_DATAID_STC_TRACE:
			if (nDataLength!=4)
				cleanup_exit(1);
			bLTrace=*(BOOL*)pMem;
			break;
		case SFTP_DATAID_STC_MKD:
			if (!nDataLength)
				cleanup_exit(1);
			sftp_cmd_mkdir(pMem);
			break;
		case SFTP_DATAID_STC_GET:
			if (!nDataLength)
				cleanup_exit(1);
			sftp_general_get(pMem, pMem+strlen(pMem)+1, *(int *)(pMem+strlen(pMem)+1+strlen(pMem+strlen(pMem)+1)+1) );
			break;
		case SFTP_DATAID_STC_PUT:
			if (!nDataLength)
				cleanup_exit(1);
			sftp_general_put(pMem, pMem+strlen(pMem)+1, *(int *)(pMem+strlen(pMem)+1+strlen(pMem+strlen(pMem)+1)+1) );
			break;
		case SFTP_DATAID_STC_RMD:
			if (!nDataLength)
				cleanup_exit(1);
			if (sftp_cmd_rmdir(pMem)<0)
				cleanup_exit(1);
			break;
		case SFTP_DATAID_STC_DELE:
			if (!nDataLength)
				cleanup_exit(1);
			if (sftp_cmd_rm(pMem)<0)
				cleanup_exit(1);
			break;
		case SFTP_DATAID_STC_RENAME:
			if (!nDataLength)
				cleanup_exit(1);
			sftp_cmd_mv(pMem, pMem+strlen(pMem)+1);
			break;
		case SFTP_DATAID_STC_CHMOD:
			if (!nDataLength)
				cleanup_exit(1);
			sftp_cmd_chmod(pMem, pMem+strlen(pMem)+1);
			break;
		case SFTP_DATAID_STC_FDREAD:
		case SFTP_DATAID_STC_FDWRITE:
			break;
		default:
			FzSFtpIpc_Trace("Unknown command, will be ignored");
			break;
		}
	}

        random_save_seed();

	return 0;
}