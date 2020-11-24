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

	//  Classe : CBloc
	//
	//  BUT : 	Classe de bloc
	//
	class BlocRollerDynamic : public Objet3DDynamic
	{
	public:
		BlocRollerDynamic(Scene* _scene, PxTransform _position, const float _radius,
			CDispositifD3D11* _pDispositif);

		// Destructeur
		virtual ~BlocRollerDynamic();

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
		static PxRigidDynamic* createRigidBody(Scene* _scene, PxTransform _position, const float _radius);
	};

} // namespace PM3D
