#pragma once
#include "Objet3DPhysic.h"

using namespace DirectX;

namespace PM3D {
    class Objet3DDynamic :
        public Objet3DPhysic
    {
	protected:
		Objet3DDynamic(Scene* _scene, physx::PxRigidDynamic* _body, CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DPhysic(_scene, _body, _pDispositif, chargeurs) {};
	public:

		~Objet3DDynamic() = default;

		virtual void Anime(float) = 0;
    };
} //namespace PM3D