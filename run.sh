#!/system/bin/sh

# Redirecting output to append to the file
LOG_FILE="/storage/usb0/bash_log.txt"
exec 1>"$LOG_FILE" 2>&1

export PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin
export LD_LIBRARY_PATH=/vendor/lib:/system/lib
export ANDROID_BOOTLOGO=1
export ANDROID_ROOT="/system"
export ANDROID_ASSETS="/system/app"
export ANDROID_DATA="/data"
export ANDROID_STORAGE="/storage"
export ASEC_MOUNTPOINT="/mnt/asec"
export LOOP_MOUNTPOINT="/mnt/obb"
export BOOTCLASSPATH="/system/framework/core.jar:/system/framework/conscrypt.jar:/system/framework/okhttp.jar:/system/framework/core-junit.jar:/system/framework/bouncycastle.jar:/system/framework/ext.jar:/system/framework/framework.jar:/system/framework/framework2.jar:/system/framework/telephony-common.jar:/system/framework/voip-common.jar:/system/framework/mms-common.jar:/system/framework/android.policy.jar:/system/framework/services.jar:/system/framework/apache-xml.jar:/system/framework/webviewchromium.jar:/system/framework/automotive.jar"

echo "Copying recovery resources..."
/system/bin/cp /system/etc/recovery-resource.dat /storage/usb0/recovery-resource.zip

### Uncomment(remove sharp) below and build tc-write-misc to set the boot mode to fastboot.
### You need to manually reset your headunit to enter it.
# /storage/usb0/tc-write-misc boot-bootloader
