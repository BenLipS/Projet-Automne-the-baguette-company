#pragma once
#include "objetMesh.h"
#include <string>
#include "PxPhysicsAPI.h"
#include <DirectXMath.h>
#include "tools.h"

using namespace DirectX;
using namespace physx;

namespace PM3D
{

	//  Classe : Objet3DPhysique
	//
	//  BUT : 	Classe de base de tous nos objets 3D avec une physique
	//
	class Objet3DPhysic :
		public PM3D::CObjetMesh
	{
	protected:
		PxRigidActor * body_; //Corps physique
		Scene* scene_; //Scene actuelle

		Objet3DPhysic(Scene* _scene, PxRigidActor* _body) : scene_(_scene), body_(_body) {
			scene_->scene_->addActor(*body_);
		};
	public:
		~Objet3DPhysic();

		virtual void Anime(float) = 0;
		bool isPhysic() { return true; }
		PxRigidActor* getBody() { return body_; }

		Objet3DPhysic() = delete;
	};

} // namespace PM3D