#pragma once
#include "Objet3DPhysic.h"

using namespace DirectX;

namespace PM3D {
    class Objet3DDynamic :
        public Objet3DPhysic
    {
	protected:
		Objet3DDynamic(Scene* _scene, PxRigidDynamic* _body) : Objet3DPhysic(_scene, _body) {};
	public:

		~Objet3DDynamic() = default;

		virtual void Anime(float) = 0;
    };
} //namespace PM3D