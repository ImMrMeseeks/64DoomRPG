# 64DoomRPG
N64 Source port of DoomRPG

This is a source port of the reverse engineered DoomRPG Brew version for N64. There is no AI Use within this project. 
This was created using a modified toolchain of libdragon conker64, the only external dependency is libmikmod for XM playback.
If you just want to build the ROM skip to the buildarom section, please be advised this ROM is best run on real hardware instead of emulator.

The port is designed to provide features which would be comparable to a standard N64 release. Testing has been performed on a NTSC-J N64 with expansion pak. A full playthrough has been performed using with floors and ceiling turned off.  

N64 Port Features:  
Saving to Controller Pak  
Save manager to copy & Delete saves (Hold start whilst powering on the console)  
Rumble Pak support  
Accessory checks for rumble and saving  
Deblur option for RDP Rendered graphics  
FPS counter available  
RDP used to draw:  
-Wall spans  
-Floor and Ceiling when Floor/Ceil is turned off (improved performance) otherwise textured written directly to the framebuffer  
-HUD  
-Text  
-menus  
-filled rectangles  
-XM audio playback (midi files need to be converted, used OpenMPT to convert the files)  
-SDL wrapper for N64  

Known Issues:  
Saving currently checks for existing game save blocks with the same ID as toolchain to overwrite and Delete  
Audio noise when nothing played  
Music Load times for music takes a few seconds  
Bug with infinite loop on player or monster seq, when time in sequence is near the max returned tick value  
	-Avoid by turning off floors and ceilings  

Oppotunities for new features:  
Fix infinite loop bug with time check once ticks overflow  
Use modern toolchain  
Draw floors and ceiling using polygons to improve performance  
New algorithm to store sprite spans in memory for the RDP  
Draw sprite spans using RDP  
Improve text rendering  
Music playback via midi  
Improve Music loading time  
Fix audio noise  
Better source code management for N64.c  


Controls:  
DPad Movement/Strafing  
C-Left/Right turn  
Stick Left/Right turn  
Start Esc/Menu  
A Select/Action/Shoot  
B Next Weapon  
L/Z Pass turn  
R AutoMap  



Build a ROM:  
To build a rom, you will need:  
-BREW version of DoomRPG  
-BarToZip  
-OpenMPT installed to convert midi files to XM for music playback (optional)  

1. Acquire BREW version of DoomRPG, this contains the assets for the game which cannot be provided by this repository  
2. use BarToZip to convert the assets into a compressed zip  
3. extract the zipped contents into "build a rom/filesystem"  
4. use Open MPT to convert the 3 midi files (.Mid) 5039, 5040 & 5043 to an XM file format  
	-file->open choose midi file  
	-view->song properties  
	-change the type drop down to fast tracker XM  
	-file->save as into filesystem directory  
5. run Buildrom batch file  

No contributions will be taken for this repository, but please take a copy source code and make a better version than what has been produced.  

Credit and special thanks:  
-DoomRPG reverse engineering and BarToZip by Erick194, BSzili & Jakubito https://github.com/Erick194/DoomRPG-RE   
-libdragon conker64 https://github.com/conker64/libdragon  
-libdragon community https://github.com/DragonMinded/libdragon  
-N64brew community  
