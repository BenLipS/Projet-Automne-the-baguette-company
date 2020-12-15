#include "stdafx.h"
#include "PlanStatic.h"
#include "Filter.h"

using namespace physx;

namespace PM3D {

	PlanStatic::PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale, PxVec3 _direction) :
		Objet3DStatic(_scene, createRigidBody(_scene, _point, _normale))
		, normale_(_normale)
		, direction_(_direction)
	{

		if (_normale.x == 0) {
			typeTag = "terrain";
			setupFiltering(body_, FILTER_TYPE::TERRAIN, FILTER_TYPE::VEHICULE);

		}
		else {
			typeTag = "mur";
			setupFiltering(body_, FILTER_TYPE::MUR, FILTER_TYPE::VEHICULE);
		}
	}

	PxRigidStatic* PlanStatic::createRigidBody(Scene* _scene, PxVec3 _point, PxVec3 _normale)
	{
		PxRigidStatic* bodyStatic = PxCreatePlane(*(_scene->physic_), PxPlane(_point, _normale), *(_scene->material_));
		//dynamic->setAngularDamping(0.5f);
		//dynamic->setLinearVelocity(velocity);
		return bodyStatic;
	}

	PxVec3 PlanStatic::getDirection() {
		return direction_;
	}

	PxVec3 PlanStatic::getNormale() {
		return normale_;
	}


	PxTransform PlanStatic::getTerrainNormale()
	{
		return body_->getGlobalPose();
	}

} // namespace PM3D
