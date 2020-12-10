#pragma once
#include "BlocStatic.h"

namespace PM3D {
    class TerrainStatic : public BlocStatic
    {
    public:
        TerrainStatic(Scene* _scene, PxTransform _position, const float dx, const float dz,
            CDispositifD3D11* _pDispositif);
        PxVec3 getTerrainNormale();
    };
}; //namespace PM3D