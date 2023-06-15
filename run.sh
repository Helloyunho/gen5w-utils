#!/system/bin/sh

# Redirecting output to append to the file
LOG_FILE="/storage/usb0/bash_log.txt"
exec 1>"$LOG_FILE" 2>&1

echo "Copying recovery resources..."
/system/bin/cp /system/etc/recovery-resource.dat /storage/usb0/recovery-resource.zip
