#pragma once
#include "objet3d.h"
#include <string>
#include "PxPhysicsAPI.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace physx;

namespace PM3D
{
	//  Classe : Objet3DPhysique
	//
	//  BUT : 	Classe de base de tous nos objets 3D avec une physique
	//
	class Objet3DPhysic :
		public PM3D::CObjet3D
	{
	protected:
		PxRigidActor * body_; //Corps physique
		PxScene* scene_; //Scene actuelle

		Objet3DPhysic(PxScene* _scene, PxRigidActor* _body) : scene_(_scene), body_(_body) {
			scene_->addActor(*body_);
		};
	public:
		~Objet3DPhysic() { scene_->removeActor(*body_); }

		virtual void Anime(float) {};
		virtual void Draw() = 0;

		Objet3DPhysic() = delete;
	};

} // namespace PM3D