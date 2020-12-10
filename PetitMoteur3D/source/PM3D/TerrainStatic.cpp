#include "stdafx.h"
#include "TerrainStatic.h"
#include "..\..\header\PM3D\TerrainStatic.h"

PM3D::TerrainStatic::TerrainStatic(Scene* _scene, PxTransform _position, const float dx, const float dz, CDispositifD3D11* _pDispositif, Light_Manager LM_) : BlocStatic(_scene, _position, dx, 0.1f, dz, _pDispositif, LM_)
{
	typeTag = "terrain";
}

PxTransform PM3D::TerrainStatic::getTerrainNormale()
{
	return body_->getGlobalPose();
}
