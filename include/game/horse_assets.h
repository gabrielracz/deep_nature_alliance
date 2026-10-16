#ifndef HORSE_ASSETS_H_
#define HORSE_ASSETS_H_

#include "resource_manager.h"

// Procedurally generated meshes and textures for the desert horse, its tack and its rider.
// Everything is built from lofted cross sections so the horse needs no external art assets.
namespace HorseAssets {
    void Load(ResourceManager& resman);
}

#endif // HORSE_ASSETS_H_
