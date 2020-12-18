#include "StdAfx.h"
#include "BlocStatic.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "MoteurWindows.h"
#include "tools.h"
#include "Filter.h"

using namespace physx;
using namespace DirectX;


namespace PM3D
{
	
	BlocStatic::BlocStatic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
		CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DStatic(_scene, createRigidBody(_scene, _position, dx / 2, dy / 2, dz / 2), _pDispositif, chargeurs)
		, dx_(dx)
		, dy_(dy)
		, dz_(dz)
	{
		typeTag = "Bloc";

		// Filtre pour les collisions
		setupFiltering(body_, FILTER_TYPE::OBSTACLE, FILTER_TYPE::VEHICULE);
	}

	void BlocStatic::Anime(float) noexcept
	{
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position
	}

	BlocStatic::~BlocStatic()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	PxRigidStatic* BlocStatic::createRigidBody(Scene* _scene, PxTransform _position, const float _dx, const float _dy, const float _dz)
	{
		PxRigidStatic* bodyStatic = PxCreateStatic(*(_scene->physic_), _position, PxBoxGeometry(_dx, _dy, _dz), *(_scene->material_));
		return bodyStatic;
	}

} // namespace PM3D
