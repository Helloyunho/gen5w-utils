#!/system/bin/sh

# Redirecting output to append to the file
LOG_FILE="/storage/usb0/bash_log.txt"
exec >>"$LOG_FILE" 2>&1

echo "d_audio: $(/system/bin/d_audio)"

echo "Going to try to copy recovery resources again, but first here are my ids"
echo "id: $(/system/bin/id -u)"
echo "group: $(/system/bin/id -g)"

echo "/system/etc $(/system/bin/ls -asl /system/etc/)"

echo "copying recovery resources: $(/system/bin/cp /system/etc/recovery-resource.dat /storage/usb0/recovery-resource.dat)"
