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
		Bonus(Scene* _scene, PxTransform _position, const float _radius, const float _height,
			CDispositifD3D11* _pDispositif, Light_Manager LM_ = {

			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f)
			});

		// Destructeur
		virtual ~Bonus();

		virtual void Anime(float) noexcept override;

	private:


		Light_Manager LM_;

		void InitShaders();

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;

		float radius_, height_;

		static PxRigidStatic* createRigidBody(Scene* _scene, PxTransform _position,
			const float _radius, const float _Hheight);
	};

} //namespace PM3D
