#pragma once
#include "objetMesh.h"
#include "tools.h"
#include <d3d11.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace PM3D {
	//  Classe : Objet3Dvisuel
	//
	//  BUT : 	Affichage uniquement
	//
	class Objet3Dvisuel :
		public CObjetMesh
	{
	public:
		Objet3Dvisuel(IChargeur* chargeur, CDispositifD3D11* pDispositif, float angle, float x = 0.0f, float y = 0.0f, float z = 0.0f) : CObjetMesh(chargeur, pDispositif) { pose = PxTransform(PxVec3(x, y, z), PxQuat(angle, PxVec3(1.0f, 0.0f, 0.0f))); typeTag = "obj3dVisuel"; }

		virtual void Anime(float tempsEcoule) noexcept override;
	private:
		PxTransform pose;
	};
}