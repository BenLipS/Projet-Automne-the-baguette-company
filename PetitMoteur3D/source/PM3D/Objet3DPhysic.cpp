#include "stdafx.h"
#include "Objet3DPhysic.h"
#include "MoteurWindows.h"

namespace PM3D {

	Objet3DPhysic::~Objet3DPhysic() {
		scene_->scene_->removeActor(*body_);
	}

	IChargeur* Objet3DPhysic::getChargeurLOD(std::vector<IChargeur*> chargeurs, PxRigidActor* body) {
		XMVECTOR camPoseDX = CMoteurWindows::GetInstance().getCameraPosition();
		PxVec3 camPose{ XMVectorGetX(camPoseDX),XMVectorGetY(camPoseDX),XMVectorGetZ(camPoseDX) };
		PxVec3 objPose = body->getGlobalPose().p;
		float distance = (objPose - camPose).magnitude();
		if (distance < 50000.f) {
			return chargeurs.front();
		}
		/*else if (distance < 30000.f) {
			return chargeurs[1];
		}*/
		else {
			return chargeurs.back();
		}
	}

	IChargeur* Objet3DPhysic::getChargeurLODMoteur(std::vector<IChargeur*> chargeurs, PxRigidActor* body) {
		XMVECTOR camPoseDX = CMoteurWindows::GetInstance().getCameraPosition();
		PxVec3 camPose{ XMVectorGetX(camPoseDX),XMVectorGetY(camPoseDX),XMVectorGetZ(camPoseDX) };
		PxVec3 objPose = body->getGlobalPose().p;
		float distance = (objPose - camPose).magnitude();
		if (chargeurs.size() == 3) {
			if (distance < 50000.f) {
				return chargeurs.front();
			}
			else if (distance < 30000.f) {
				return chargeurs[1];
			}
			else {
				return chargeurs.back();
			}
		}
		return chargeurs.front();
	}

}