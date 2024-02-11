# NOTE: Currently, the navigation crashes when the USB is not plugged in.

## Pre-reqirements

- Two USBs(Only need one if you have an old firmware installed)
- (Optional but highly recommended) A laptop
- (Optional) A toothpick or a small pin
  - This is for resetting the head unit. You'll see why later.

## Install patched firmware

1. Plug the USB into your car

- You might see an update message right away. If you don't, go to `Settings` > `General` and press the `Update`(not `Check Update`) button below.
<details>
  <summary>If you're using the latest 2023 firmware or above</summary>

- Since the latest 2023 firmware, Hyundai Mobis decided to validate a checksum for Navigation files as well, essentially blocking our exploit.
- If you followed the previous steps, you should have two USBs, one with stock firmware, and the other with patched firmware.
1. Plug the USB with stock firmware into the car and wait for the update message.
2. Press the `Update` button and wait for the system to shut off(the screen goes completely black).
3. When the system shuts off, quickly unplug the USB and plug the USB with patched firmware.
  - If you see a message saying "Updating...", you're good to go.
  - If you see an error message, you probably missed the timing. Try again.  
    ... or you can just reset the head unit by pressing the reset button in a small hole.  
    For me, it was on the right side of the right knob.
    ![CleanShot 2024-02-12 at 03 43 29@2x](https://github.com/Helloyunho/gen5w-utils/assets/10192625/d283a61d-33d1-4f1f-8618-ec473d611379)
  </details>

2. Keep the USB plugged in, even after the update has been completed.

## Conclusion

If everything works correctly, the system will reboot again and you can use it like normal.

But now, in your USB, the firmware decryption keys should be there!

You can modify `run.sh` in your USB to run some Linux commands on your car.

## Commonly used commands
<details>
  <summary>Enabling ADB</summary>

```sh
service call com.hkmc.misc 6 i32 5
service call com.hkmc.misc 4 i32 4
```

</details>

<details>
  <summary>Disabling ADB</summary>

```sh
service call com.hkmc.misc 6 i32 6
service call com.hkmc.misc 4 i32 4
```

</details>

<details>
  <summary>Enter fastboot from next boot</summary>

**NOTE: You somehow have to write empty value(`''`) to the misc partition to go back to Android.**

1. Follow the previous guide until `Building hook.c -> 1.`

2. Build tc-write-misc

```sh
./build_tc-write-misc.sh

# If you get a permission error run below and try the above command again
chmod +x ./build_tc-write-misc.sh
```

3. Edit run.sh

```sh
/storage/usb0/tc-write-misc boot-bootloader
```

4. Press the reset button in a small hole   
  For me, it was on the right side of the right knob.
  ![CleanShot 2024-02-12 at 03 43 29@2x](https://github.com/Helloyunho/gen5w-utils/assets/10192625/d283a61d-33d1-4f1f-8618-ec473d611379)

</details>
