#pragma once
#include "Objet3DDynamic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"

#include "PxPhysicsAPI.h"

using namespace DirectX;

namespace PM3D
{
	class CDispositifD3D11;

	class BlocDynamic : public Objet3DDynamic
	{
	public:
		BlocDynamic(Scene* _scene, physx::PxTransform _position, const float dx, const float dy, const float dz,
			CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeur);

		// Destructeur
		virtual ~BlocDynamic();

		virtual void Anime(float tempsEcoule);
		

	private:

		float dx_, dy_, dz_;

		static physx::PxRigidDynamic* createRigidBody(Scene* _scene, physx::PxTransform _position,
			const float _dx, const float _dy, const float _dz);
	
	};
} // namespace PM3D
