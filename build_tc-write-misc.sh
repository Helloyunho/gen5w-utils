if [ -z "$NDK_ROOT"]; then
    if [ -z "$ANDROID_NDK_ROOT" ]; then
        if [ -z "$ANDROID_NDK_HOME" ]; then
            echo "Please set ANDROID_NDK_ROOT or ANDROID_NDK_HOME"
            exit 1
        else
            NDK_ROOT=$ANDROID_NDK_HOME
        fi
    else
        NDK_ROOT=$ANDROID_NDK_ROOT
    fi
fi

HOST_OS="$(uname -s | tr '[:upper:]' '[:lower:]')"
HOST_ARCH="$(uname -m | tr '[:upper:]' '[:lower:]')"
if [[ "$HOST_ARCH" == "arm64" && "$HOST_OS" == "darwin" ]]; then
    HOST_ARCH="x86_64" # NDK r21+ has no prebuilt darwin-aarch64 toolchain
fi
HOST_TAG="$HOST_OS-$HOST_ARCH"

curl -L https://git.huconn.com/linux_ivi_mirror/yocto/meta-telechips-bsp/-/raw/tcc8050_linux_ivi_tost/recipes-bsp/tc-write-misc/tc-write-misc/tc-write-misc.c?inline=false -o tc-write-misc.c

"$NDK_ROOT/toolchains/llvm/prebuilt/$HOST_TAG/bin/armv7a-linux-androideabi19-clang" tc-write-misc.c -o tc-write-misc
