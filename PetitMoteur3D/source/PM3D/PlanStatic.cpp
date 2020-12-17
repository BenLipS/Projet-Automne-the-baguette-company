#include "stdafx.h"
#include "PlanStatic.h"
#include "Filter.h"

using namespace physx;

namespace PM3D {

	PlanStatic::PlanStatic(Scene* _scene, PxVec3 _point, PxVec3 _normale) :
		Objet3DStatic(_scene, createRigidBody(_scene, _point, _normale))
	{

		if (_normale.x == 0) {
			typeTag = "pente";
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

		return bodyStatic;
	}

	PxVec3 PlanStatic::getDirection() {
		PxVec3 vector1 = body_->getGlobalPose().q.getBasisVector1();
		if (vector1.x < 0.0001 && vector1.x > -0.0001)
			vector1.x = 0;
		if (vector1.y < 0.0001 && vector1.y > -0.0001)
			vector1.y = 0;
		if (vector1.z < 0.0001 && vector1.z > -0.0001)
			vector1.z = 0;

		PxVec3 vector2 = body_->getGlobalPose().q.getBasisVector2();
		if (vector2.x < 0.0001 && vector2.x > -0.0001)
			vector2.x = 0;
		if (vector2.y < 0.0001 && vector2.y > -0.0001)
			vector2.y = 0;
		if (vector2.z < 0.0001 && vector2.z > -0.0001)
			vector2.z = 0;

		PxVec3 directionX = PxVec3(0.0f, -1.0f, 0.0f).dot(vector1) * vector1;
		PxVec3 directionY = PxVec3(0.0f, -1.0f, 0.0f).dot(vector2) * vector2;
		PxVec3 direction = (directionX + directionY);
		PxVec3 directionN = direction.getNormalized();
		return directionN;
	}

	PxVec3 PlanStatic::getNormale() {
		return body_->getGlobalPose().q.getBasisVector0();
	}

	PxVec3 PlanStatic::getPointPlan(physx::PxVec3 _point)
	{
		//_point.y = 0.0f;
		PxVec3 projeteX = _point.dot(body_->getGlobalPose().q.getBasisVector1()) * body_->getGlobalPose().q.getBasisVector1();
		PxVec3 projeteY = _point.dot(body_->getGlobalPose().q.getBasisVector2()) * body_->getGlobalPose().q.getBasisVector2();
		PxVec3 pointProjete = (projeteX + projeteY);
		PxVec3 vecteurProjete = pointProjete - _point;
		PxVec3 direction;
		if (pointProjete.y < _point.y) {
			direction = PxVec3(0.0f, -1.0f, 0.0f);
		}
		else {
			direction = PxVec3(0.0f, 1.0f, 0.0f);
		}
		float angle = acos(direction.dot(vecteurProjete.getNormalized()));
		float longueurResultat = vecteurProjete.magnitude() / cos(angle);
		PxVec3 projete = _point + direction * longueurResultat;
		return projete;
	}


	PxTransform PlanStatic::getTerrainNormale()
	{
		return body_->getGlobalPose();
	}

} // namespace PM3D
