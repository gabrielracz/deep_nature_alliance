3D space exploration game with a story!

BUILDING

Visual Studio
mkdir build
cd build
cmake ..
./dna.sln
Set dna as startup project
Choose release build
run build and run

Linux Build
mkdir build
cd build
cmake ..
make

RIDING (desert level)
A saddled horse waits near the landing site.
F            mount / dismount (walk up to her first)
B            whistle - she comes to you from anywhere on the map
W            trot          SHIFT   gallop (uses stamina)
CTRL + W     walk          S       brake / back up
A / D        steer         SPACE   jump while moving, rear up when standing
Z            toggle third / first person camera, mouse orbits or looks around
F12          save a screenshot

The horse, rider, tack, dust and all horse sounds are generated procedurally at startup.

Developer: set DNA_AUTOTEST to a script file to drive the game with timed input, e.g.
  0.5 press 3
  2.0 hold W 0.45
  2.9 press F
  4.0 mouse 650 0
  4.5 shot C:\path\shot.bmp
  6.0 quit

Attributions:
Planetary textures taken from https://www.solarsystemscope.com/textures/ (CC BY 4.0)
Habitable worlds textures taken from https://www.texturesforplanets.com/texture-packs.shtml
Cactus meshes and textures taken from Cactus | Pack by YadroGames https://sketchfab.com/3d-models/cactus-pack-588596f1601d48e6ad4cb24b31c3f33c
Sand /desert texture taken from https://polyhaven.com/a/excavated_soil_wall
Towers taken from Pack of old towers in ruins by JB3D https://sketchfab.com/3d-models/pack-of-old-towers-in-ruins-f213359c6cbb4c29bf8880764faa0fb8
Monastery taken from Jvari Monastery by Nik https://sketchfab.com/3d-models/jvari-monastery-9b70c349769a40b5a053c890693626f5
Low Spec Retro Computer - Commodore PET by pistachio opengameart.org CC-BY 4.0 Modified texture.
