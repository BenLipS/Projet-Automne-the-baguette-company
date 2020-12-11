#include "stdafx.h"
#include "PlanStatic.h"
#include "..\..\header\PM3D\PlanStatic.h"

PM3D::PlanStatic::PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale, PxVec3 _direction) :
	Objet3DStatic(_scene->scene_, createRigidBody(_scene, _point, _normale))
	, normale_(_normale)
	, direction_(_direction)
{
	if (_normale.x == 0 )
		typeTag = "terrain";
}

PxRigidStatic* PM3D::PlanStatic::createRigidBody(Scene* _scene, PxVec3 _point, PxVec3 _normale)
{
	PxRigidStatic* bodyStatic = PxCreatePlane(*(_scene->physic_), PxPlane(_point, _normale), *(_scene->material_));
	//dynamic->setAngularDamping(0.5f);
	//dynamic->setLinearVelocity(velocity);
	return bodyStatic;
}

PxVec3 PM3D::PlanStatic::getDirection() {
	return direction_;
}

PxVec3 PM3D::PlanStatic::getNormale() {
	return normale_;
}


PxTransform PM3D::PlanStatic::getTerrainNormale()
{
	return body_->getGlobalPose();
}

