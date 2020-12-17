#include "stdafx.h"
#include "Objet3Dvisuel.h"

using namespace DirectX;

namespace PM3D
{

	void Objet3Dvisuel::Anime(float) noexcept
	{
		float x = pose.q.x;
		float y = pose.q.y;
		float z = pose.q.z;
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(pose.q.x, pose.q.y, pose.q.z, pose.q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(pose.p.x, pose.p.y, pose.p.z, 1)); //Position
	}
}