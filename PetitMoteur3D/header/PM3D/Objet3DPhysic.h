#pragma once
#include "objetMesh.h"
#include <string>
#include "PxPhysicsAPI.h"
#include <DirectXMath.h>
#include "tools.h"

using namespace DirectX;

namespace PM3D
{

	//  Classe : Objet3DPhysique
	//
	//  BUT : 	Classe de base de tous nos objets 3D avec une physique
	//
	class Objet3DPhysic :
		public CObjetMesh
	{
	protected:
		physx::PxRigidActor * body_; //Corps physique
		Scene* scene_; //Scene actuelle

		Objet3DPhysic(Scene* _scene, physx::PxRigidActor* _body) : scene_(_scene), body_(_body) {
			scene_->scene_->addActor(*body_);
		};

		Objet3DPhysic(Scene* _scene, physx::PxRigidActor* _body, CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : CObjetMesh(getChargeurLOD(chargeurs,_body),chargeurs, _pDispositif), scene_(_scene), body_(_body) {
			scene_->scene_->addActor(*body_);
		};
	public:
		~Objet3DPhysic();

		virtual void Anime(float) = 0;

		bool isPhysic() { return true; }
		physx::PxRigidActor* getBody() { return body_; }
		physx::PxVec3 getPosition() { return body_->getGlobalPose().p; }

		Objet3DPhysic() = delete;

		IChargeur* getChargeurLODMoteur(std::vector<IChargeur*> chargeurs, physx::PxRigidActor* body);
		
	private:
		static IChargeur* getChargeurLOD(std::vector<IChargeur*> chargeurs, physx::PxRigidActor* body);
	};

} // namespace PM3D