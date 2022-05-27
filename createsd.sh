#!/bin/sh
truncate -s 1G sdcard.sdimg
/usr/local/Cellar/dosfstools/4.2/sbin/mkfs.vfat sdcard.sdimg
mmd -i sdcard.sdimg osd  
mmd -i sdcard.sdimg osd/System
mmd -i sdcard.sdimg osd/System/Fonts
mmd -i sdcard.sdimg osd/Welcome
mcopy -i sdcard.sdimg /Users/daryl/Dev/osd/fonts/IBM*.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg /Users/daryl/Dev/osd/fonts/fa-light-300.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg Applications/* ::osd/Welcome

