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
		BlocStatic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
			CDispositifD3D11* _pDispositif, const IChargeur& chargeur, Light_Manager LM_ = {

			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f)
			});

		// Destructeur
		virtual ~BlocStatic();

		virtual void Anime(float) noexcept override;

	private:

		
		Light_Manager LM_;

		void InitShaders();

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;

		float dx_, dy_, dz_;

		static PxRigidStatic* createRigidBody(Scene* _scene, PxTransform _position,
			const float _dx, const float _dy, const float _dz);
    };

} //namespace PM3D