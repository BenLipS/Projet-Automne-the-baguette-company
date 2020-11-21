#pragma once
#pragma once
#include "objet3D.h"
#include "Bloc.h"
#include "d3dx11effect.h"

namespace PM3D
{

	class CDispositifD3D11;

	//  Classe : CBlocEffect1
	//
	//  BUT : 	Classe de bloc
	//
	class CBlocEffect1 : public CObjet3D
	{
	public:
		CBlocEffect1(const float dx, const float dy, const float dz,
			CDispositifD3D11* pDispositif);

		// Destructeur
		virtual ~CBlocEffect1();

		void Anime(float tempsEcoule) noexcept override;
		void Draw() override;

	private:
		CDispositifD3D11* pDispositif;
		void InitShaders();
		void InitEffect();

		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		// D�finitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;
	};

} // namespace PM3D
