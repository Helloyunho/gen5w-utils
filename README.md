# Gen5W Utils

Useful utilities for Hyundai/KIA/Genesis Gen5W navigation firmwares.

## Files

- `odex_to_dex.py`: Converts `.odex` file to `.dex` using [smali](https://github.com/JesusFreke/smali).
- `tarlist_decrypt.py`: Decrypts `TarList.txt_encrypted` file in Gen5W firmware. This can also be used to encrypt `TarList.txt` file.
- `unpack_apk.py`: Automatically extracts apk using [Apktool](https://github.com/iBotPeaches/Apktool) and converts `<APK_NAME>.odex` to `classes.dex` file.
- `parse_ver.py`: Reads `.ver` file in Gen5W firmware and prints it in human-friendly form.
- `decrypt_navi.py`: Decrypts encrypted libraries in Gen5W firmware 'sw_backup' folder.
- `encrypt_navi.py`: Encrypts decrypted libraries in Gen5W firmware 'sw_backup' folder.
- `sha224_gen.py`: Generates Hyundai's specialized SHA224 hash for Gen5W firmware.
- `parse_tarlist.py`: Parses `TarList.txt` file in Gen5W firmware and prints it in human-friendly form.
- `make_patched_firmware.py`: Makes custom patched firmware with `sw_backup.tar`, `TarList.txt_encrypted` and `.ver` files.
- `hook.c`: Actual hook code for `libExSLAndroid_JNI.so` library. This uses no any default libraries or standard library functions. The build arguments are `-mthumb -fPIC -shared -fomit-frame-pointer -nostdlib -nodefaultlibs -o hook.o`.

## Dependencies

- Python 3 (tested on `3.10.10`)
- Everything in `requirements.txt`
- libtar (only required when creating custom firmware, tested on `1.2.20`)
- smali (only required when converting `.odex` to `.dex`, tested on `2.5.2-dev`)
- Apktool (only required when unpacking apk, tested on `2.7.0`)
- Android 4.4.2 system dump (only required when unpacking apk)
- Android NDK (only required when building hook, tested on `25.2.9519653`)

Use at your own risk.
