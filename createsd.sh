#!/bin/sh
truncate -s 1G sdcard.sdimg
/usr/local/Cellar/dosfstools/4.2/sbin/mkfs.vfat sdcard.sdimg
mmd -i sdcard.sdimg osd  
mmd -i sdcard.sdimg osd/fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Sans/IBMPlexSans-Regular.ttf ::osd/fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Serif/IBMPlexSerif-Regular.ttf ::osd/fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Mono/IBMPlexMono-Regular.ttf ::osd/fonts
mcopy -i sdcard.sdimg ./fa-light-300.ttf ::osd/fonts

