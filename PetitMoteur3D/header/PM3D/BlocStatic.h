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
			CDispositifD3D11* _pDispositif);

		// Destructeur
		virtual ~BlocStatic();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;

	private:
		CDispositifD3D11* pDispositif;
		void InitShaders();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		//float rotation;
		static PxRigidStatic* createRigidBody(Scene* _scene, PxTransform _position,
			const float _dx, const float _dy, const float _dz);
    };

} //namespace PM3D