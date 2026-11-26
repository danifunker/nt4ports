# sdkrun.sh (sourced by build.sh and mfc/build-mfc.sh): run a command in an SDK container.
#
#   ntsdk_run CPU 'command' [docker run options...]     tenox7/ntsdk, nt40 CPU
#   pint_run 'command' [bind options...]                the PINT 4.0 SDK
#
# The command runs in sh with /fz the repository's filezilla folder; the options add the
# folders it needs (-v HOST:/path for Docker, "HOST:/path" pairs for pint_run).
# Files written in the container are given back to the user who runs the build.
#
# pint_run uses Docker's tenox7/pint40sdk with the msvc-arm32 volume (pint/README.md) when
# Docker has the image. Otherwise it runs an unpacked copy of the image in bubblewrap, as
# ruststdwinnt4's tools/pint-sdk does: PINT_SDK is the folder holding its rootfs, and
# PINT_MSVC the folder Microsoft C for ARM was installed in.

uid_gid=$(id -u):$(id -g)

ntsdk_run() {
    _cpu=$1; _cmd=$2; shift 2
    docker run --rm -v "$fz":/fz "$@" tenox7/ntsdk nt40 "$_cpu" sh -c "
        $_cmd; s=\$?; chown -R $uid_gid /out /mfc 2>/dev/null; exit \$s"
}

pint_run() {
    _cmd=$1; shift
    if docker image inspect tenox7/pint40sdk >/dev/null 2>&1; then
        _v=
        for b in "$@"; do _v="$_v -v $b"; done
        # shellcheck disable=SC2086
        docker run --rm --platform linux/amd64 -v msvc-arm32:/opt/msvc -v "$fz":/fz $_v \
            tenox7/pint40sdk sh -c "
            $_cmd; s=\$?; chown -R $uid_gid /out /mfc 2>/dev/null; exit \$s"
        return
    fi
    : "${PINT_SDK:?no tenox7/pint40sdk in Docker: set PINT_SDK to an unpacked copy (with rootfs/)}"
    : "${PINT_MSVC:?set PINT_MSVC to the folder of Microsoft C for ARM}"
    set -- --bind "$fz" /fz $(for b in "$@"; do printf -- '--bind %s %s ' "${b%%:*}" "${b#*:}"; done)
    bwrap --bind "$PINT_SDK/rootfs" / --dev /dev --proc /proc --dir /run/xdg --tmpfs /tmp \
        --bind "$PINT_MSVC" /opt/msvc "$@" \
        --unshare-user --uid 0 --gid 0 --die-with-parent --clearenv \
        --setenv PATH /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        --setenv WINEDEBUG -all --setenv WINEPREFIX /opt/wine --setenv XDG_RUNTIME_DIR /run/xdg \
        --setenv HOME /root --chdir /fz \
        /usr/local/bin/pint-entrypoint sh -c "ulimit -n \$(ulimit -Hn) 2>/dev/null; $_cmd"
}
