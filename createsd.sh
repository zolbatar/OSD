#!/bin/sh
truncate -s 1G sdcard.sdimg
/usr/local/Cellar/dosfstools/4.2/sbin/mkfs.vfat sdcard.sdimg
mmd -i sdcard.sdimg osd  
mmd -i sdcard.sdimg osd/System
mmd -i sdcard.sdimg osd/System/Fonts
mmd -i sdcard.sdimg osd/Welcome
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Sans/*.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Sans_Condensed/*.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Serif/*.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg ~/Dropbox/Development/Resource/Fonts/Plex/IBM_Plex_Mono/*.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg ~/Downloads/fa-light-300.ttf ::osd/System/Fonts
mcopy -i sdcard.sdimg Applications/*.daric ::osd/Welcome

