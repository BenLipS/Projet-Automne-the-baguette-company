#pragma once
#include "BlocStatic.h"

namespace PM3D {
    class TerrainStatic : public BlocStatic
    {
    public:
        TerrainStatic(Scene* _scene, PxTransform _position, const float dx, const float dz,
            CDispositifD3D11* _pDispositif, Light_Manager LM_ = {

			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f)
			});
        PxTransform getTerrainNormale();
    };
}; //namespace PM3D