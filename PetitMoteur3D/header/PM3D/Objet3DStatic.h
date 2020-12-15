#pragma once

#include "Objet3Dphysic.h"

using namespace DirectX;

namespace PM3D {

    class Objet3DStatic :
        public Objet3DPhysic
    {
	protected:
		Objet3DStatic(Scene* _scene, physx::PxRigidStatic* _body) : Objet3DPhysic(_scene, _body) {};
		Objet3DStatic(Scene* _scene, physx::PxRigidStatic* _body, CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DPhysic(_scene, _body, _pDispositif, chargeurs) {};
	public:
		
		~Objet3DStatic() = default;

		virtual void Anime(float) noexcept {}
    };
} // namespace PM3D