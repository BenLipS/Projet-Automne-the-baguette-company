#include "stdafx.h"
#include "SkyBox.h"
#include "MoteurWindows.h"

namespace PM3D {

	void SkyBox::Anime(float) noexcept
	{
		physx::PxQuat quat1{ XM_PI/3,physx::PxVec3(1.0f,0.0f,0.0f) };
		physx::PxQuat quat2{ -XM_PI/2,physx::PxVec3(0.0f,1.0f,0.0f) };
		physx::PxQuat quat = quat1 * quat2;
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(quat.x, quat.y, quat.z, quat.w));
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(XMVectorGetX(CMoteurWindows::GetInstance().getCameraPosition()), XMVectorGetY(CMoteurWindows::GetInstance().getCameraPosition()) - 2000.0f, XMVectorGetZ(CMoteurWindows::GetInstance().getCameraPosition()) + 8000.0f, 1.0f)); //Position
	}

}