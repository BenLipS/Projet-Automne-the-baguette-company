#pragma once
#include "Objet3DPhysic.h"

using namespace DirectX;

namespace PM3D {
    class Objet3DDynamic :
        public Objet3DPhysic
    {
	protected:
		Objet3DDynamic(Scene* _scene, PxRigidDynamic* _body, CDispositifD3D11* _pDispositif, const IChargeur& chargeur) : Objet3DPhysic(_scene, _body, _pDispositif, chargeur) {};
	public:

		~Objet3DDynamic() = default;

		virtual void Anime(float) = 0;
    };
} //namespace PM3D