#pragma once
#include "Objet3DStatic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dx11effect.h>
#include "tools.h"
#include "Texture.h"

#include "PxPhysicsAPI.h"

namespace PM3D {

	
	class CDispositifD3D11;

    class BlocStatic :
        public Objet3DStatic
    {
	public:
		BlocStatic(Scene* _scene, PxTransform _position, const float dx, const float dy, const float dz,
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
		virtual ~BlocStatic();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;
		void SetTexture(CTexture* pTexture);

	private:

		
		Light_Manager LM_;

		CDispositifD3D11* pDispositif;
		void InitShaders();
		// Initialisation de l’effet
		void InitEffet();


		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		ID3D11ShaderResourceView* pTextureD3D;
		ID3D11SamplerState* pSampleState;

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;

		

		// D�finitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		//float rotation;

		float dx_, dy_, dz_;

		static PxRigidStatic* createRigidBody(Scene* _scene, PxTransform _position,
			const float _dx, const float _dy, const float _dz);
    };

} //namespace PM3D