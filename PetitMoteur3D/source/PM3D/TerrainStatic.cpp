#include "stdafx.h"
#include "TerrainStatic.h"
#include "..\..\header\PM3D\TerrainStatic.h"

PM3D::TerrainStatic::TerrainStatic(Scene* _scene, PxTransform _position, const float dx, const float dz, CDispositifD3D11* _pDispositif) : BlocStatic(_scene, _position, dx, 0.1f, dz, _pDispositif)
{
	typeTag = "terrain";
}

PxVec3 PM3D::TerrainStatic::getTerrainNormale()
{
	return body_->getGlobalPose().q.getBasisVector1().getNormalized();
}
