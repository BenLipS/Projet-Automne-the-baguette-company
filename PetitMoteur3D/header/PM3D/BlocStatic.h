#pragma once
#include "Objet3DStatic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"

#include "PxPhysicsAPI.h"

namespace PM3D {

	class CDispositifD3D11;

    class BlocStatic :
        public Objet3DStatic
    {
	public:
		BlocStatic(Scene* _scene, physx::PxTransform _position, const float dx, const float dy, const float dz,
			CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs);

		// Destructeur
		virtual ~BlocStatic();

		virtual void Anime(float) noexcept override;

	private:

		float dx_, dy_, dz_;

		static physx::PxRigidStatic* createRigidBody(Scene* _scene, physx::PxTransform _position,
			const float _dx, const float _dy, const float _dz);

    };

} //namespace PM3D