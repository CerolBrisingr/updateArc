# updateArc  
Updater for the DPS-Tracking tool arcdps (https://www.deltaconnected.com/arcdps/) which is as of now authorized for usage with the game GuildWars2(TM)
Has since then been expanded to:
* ArcDPS
* TacO
* Blish-HUD
* Tekkit  
  
With each element being optional  
Will also run Blish-HUD, Gw2 and/or TacO as set up by the user

## Motivation:  
I am using this tool myself and was interested in an updater. Also, I wanted to do something with c++. 
  
## Goals:  
* Updater should only read the md5 file on the server to decide whether a new version is available
* Updater should use https
* Updater should provide an easy way to remove arcdps since updates in GuildWars2 often lead to incompatibilities
* New: updater should allow to block incompatible version for future update calls
! Updater will overwrite any d3d9.dll used by other .. tools, like shader adjustments

## Install ArcUpdater:  
The UpdateArc.exe as well as the libraries
* Qt5Core.dll (Code is using the QT frameowrk)
* Qt5Network.dll (Code is using the QT framework to establish the https connection)
* libeay32.dll and ssleay32.dll (OpenSSL is necessary it's easier to give you those dll files than to write an install guide)  
* Since this has become a GUI, some more stuff..

need to be placed in a subfolder to the GuildWars2 game folder.

## Update arcdps (and more):  
Just run UpdateArc.exe, follow UI

## Remove arcdps (and more):  
Run UpdateArc.exe, watch out for the [-] boxes next to the updaters

## Remove ArcUpdater:  
Just delete the folder you placed the Updater in. If you want to also get rid of arcdps, I advise you to run removal first.
Blish-HUD places its settings and markers in Documents/Guild Wars 2/addons
