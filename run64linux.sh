#!/bin/sh
~/GitHub/qemu/build/qemu-system-aarch64 \
    -global bcm2835-fb.xres=1920 -global bcm2835-fb.yres=1080 \
    -M raspi3b -d guest_errors,unimp \
    -kernel kernel8.img \
    -device usb-kbd -device usb-mouse \
    -rtc clock=host -smp 4
#    -sd noobs1gb.sdimg
#                        -semihosting -monitor telnet:localhost:1235,server,nowait \
#                        -S -gdb tcp::1234 -serial stdio 
