#include "StdAfx.h"
#include "BlocDynamic.h"
#include "util.h"
#include "DispositifD3D11.h"

using namespace physx;
using namespace DirectX;

namespace PM3D
{

	BlocDynamic::BlocDynamic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
		CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DDynamic(_scene, createRigidBody(_scene, _position, dx/2, dy/2, dz/2), _pDispositif, chargeurs)
		, dx_(dx)
		, dy_(dy)
		, dz_(dz)
	{
		typeTag = "Bloc";
	}

	void BlocDynamic::Anime(float)
	{
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position
	}

	BlocDynamic::~BlocDynamic()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	PxRigidDynamic* BlocDynamic::createRigidBody(Scene* _scene, PxTransform _position, const float _dx, const float _dy, const float _dz)
	{
		PxRigidDynamic* bodyDynamic = PxCreateDynamic(*(_scene->physic_), _position, PxBoxGeometry(_dx, _dy, _dz), *(_scene->material_), 10.0f);
		return bodyDynamic;
	}

} // namespace PM3D
