## Pre-requirements

- Python 3<sup>[[install]](https://command-not-found.com/python3)</sup>(tested on `3.10.10`)
- Build tools<sup>[[install clang]](https://command-not-found.com/clang)</sup>(LLVM, GCC, MSVC, or anything you would like to use)
- Git<sup>[[install]](https://command-not-found.com/git)</sup>(I mean come on, even this repository is on GitHub)
- Make<sup>[[install]](https://command-not-found.com/make)</sup>
- [libtar](https://github.com/Parrot-Developers/libtar)(tested on `1.20.20`, build instruction below)
- [Android NDK](https://developer.android.com/ndk)(tested on `25.2.9519653`, you can install any version that supports Android 4.4.2)
  - Probably the easiest way to install this is using Android Studio
  - You can use `sdkmanager`<sup>[[install]](https://command-not-found.com/android)</sup> to install NDK aswell.
- Two USBs(Only need one if you have an old firmware installed)

- And of course, the stock firmware downloaded from Hyundai/Kia/Genesis.
  - It should have files like this
    ```
    /Volumes/a/
    ├── CAR_MODEL.ver
    ├── TarList.txt_encrypted (or NEW_TarList...)
    ├── TarList.txt_encrypted.rsa (or NEW_TarList...)
    ├── extra_package_something.tar (or new_extra_package... or extra_pkg...)
    ├── navi_backup.tar1 (1 to many files)
    ├── sw_backup.tar
    └── system_package_something.tar (or new_system_package... or system_pkg...)
    ```

Though I never tested this on Windows, it should not work. It's best to just use [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) on Windows.

You should be good to go if you're using macOS or Linux. I'm using macOS for this guide.

## Building libtar

1. Clone libtar to your computer and go to the cloned directory

```sh
git clone https://github.com/Parrot-Developers/libtar.git
cd libtar
```

2. Build and install libtar

```sh
autoreconf --force --install
./configure
make
make install
cd ..
```

## Clone gen5w-utils and go to the cloned directory

```sh
git clone https://github.com/Helloyunho/gen5w-utils.git
cd gen5w-utils
```

## Building hook.c

1. Set the `ANDROID_NDK_ROOT` variable

[Read this](https://developer.android.com/studio/intro/update#sdk-manager) to find your SDK location.

```sh
export ANDROID_NDK_ROOT="[SDK_PATH_HERE]/ndk/[NDK_VERSION_HERE]"
```

Replace `[..._HERE]` correspondingly.

2. Build hook.c

```sh
./build_hook.sh

# If you get a permission error run below and try the above command again
chmod +x ./build_hook.sh
```

## Patch the firmware

1. Duplicate the stock firmware directory
  - Since the latest 2023 update, Hyundai Mobis decided to validate a checksum for Navigation files as well, essentially blocking our exploit. We're going to bypass this using two USBs, one with stock firmware, and the other with patched firmware.

2. Run a single simple command

```sh
python3 make_patched_firmware.py [STOCK_FIRMWARE_DIR_HERE]
```

Replace `[..._HERE]` correspondingly.

## Conclusion

That's it! Time to install the patched firmware.
