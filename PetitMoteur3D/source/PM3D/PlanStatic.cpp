#include "stdafx.h"
#include "PlanStatic.h"
#include "..\..\header\PM3D\PlanStatic.h"

PM3D::PlanStatic::PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale) : Objet3DStatic(_scene->scene_, createRigidBody(_scene, _point, _normale))
{
}

PxRigidStatic* PM3D::PlanStatic::createRigidBody(Scene* _scene, PxVec3 _point, PxVec3 _normale)
{
	PxRigidStatic* bodyStatic = PxCreatePlane(*(_scene->physic_), PxPlane(_point, _normale), *(_scene->material_));
	//dynamic->setAngularDamping(0.5f);
	//dynamic->setLinearVelocity(velocity);
	return bodyStatic;
}
