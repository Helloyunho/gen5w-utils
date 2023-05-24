#!/system/bin/sh                                                                             


# Redirecting output to append to the file
LOG_FILE="/storage/usb0/bash_log.txt"
exec >> "$LOG_FILE" 2>&1

echo "d_audio: $(/system/bin/d_audio)"

echo "Going to try to copy the binary again, but first here are my ids"
echo "id: $(/system/bin/id -u)"
echo "group: $(/system/bin/id -g)"

echo "copying d_audio: $(/system/bin/cp /system/bin/d_audio /storage/usb0/d_audio)"


echo "/system/bin/: $(/system/bin/ls -asl /system/bin/)"
echo "/system/etc $(/system/bin/ls -asl /system/etc/)"

echo "Just some new sanity here"
