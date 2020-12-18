#pragma once
#include "Objet3DStatic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"

#include "PxPhysicsAPI.h"

namespace PM3D {

	class CDispositifD3D11;

	class Bonus :
		public Objet3DStatic
	{
	public:
		Bonus(Scene* _scene, physx::PxTransform _position, const float _radius, const float _height,
			CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs);

		// Destructeur
		virtual ~Bonus();

		virtual void Anime(float) noexcept override;

	private:

		float radius_, height_;

		static physx::PxRigidStatic* createRigidBody(Scene* _scene, physx::PxTransform _position,
			const float _radius, const float _Hheight);

	};

} //namespace PM3D
