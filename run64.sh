#!/bin/sh
/Users/daryl/GitHub/qemu-patched/build/qemu-system-aarch64 \
    -global bcm2835-fb.xres=1920 -global bcm2835-fb.yres=1080 \
    -M raspi3b -d guest_errors,unimp \
    -kernel kernel8.img \
    -usb -device usb-kbd -device usb-mouse \
    -rtc clock=host -smp 4 -sd sdcard.sdimg
#                        -semihosting -monitor telnet:localhost:1235,server,nowait \
#                        -S -gdb tcp::1234 -serial stdio 
