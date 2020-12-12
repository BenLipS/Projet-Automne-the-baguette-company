#pragma once

#include "Objet3Dphysic.h"

using namespace DirectX;

namespace PM3D {

    class Objet3DStatic :
        public Objet3DPhysic
    {
	protected:
		Objet3DStatic(Scene* _scene, PxRigidStatic* _body) : Objet3DPhysic(_scene, _body) {};
	public:
		
		virtual ~Objet3DStatic() = default;

		virtual void Anime(float) {}
		virtual void Draw() = 0;
    };
} // namespace PM3D