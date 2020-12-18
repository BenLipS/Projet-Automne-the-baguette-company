#include "stdafx.h"
#include "Bonus.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
#include <iostream>
#include "tools.h"
#include "Filter.h"

using namespace physx;
using namespace DirectX;


namespace PM3D
{

	Bonus::Bonus(Scene* _scene, PxTransform _position, const float _radius, const float _height,
		CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DStatic(_scene, createRigidBody(_scene, _position, _radius, _height/2), _pDispositif, chargeurs)
		, radius_(_radius)
		, height_(_height)
	{
		typeTag = "bonus";

		// Filtre pour les collisions
		setupFiltering(body_, FILTER_TYPE::BONUS, FILTER_TYPE::VEHICULE);
	}

	void Bonus::Anime(float) noexcept
	{
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position
	}

	Bonus::~Bonus()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	PxRigidStatic* Bonus::createRigidBody(Scene* _scene, PxTransform _position, const float _radius, const float _Hheight)
	{
		PxShape* shape_bonus = _scene->physic_->createShape(PxCapsuleGeometry(_radius, _Hheight), *(_scene->material_), true);
		shape_bonus->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape_bonus->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		PxRigidStatic* bodyStatic = _scene->physic_->createRigidStatic(_position);
		bodyStatic->attachShape(*shape_bonus);
		return bodyStatic;
	}

} // namespace PM3D
