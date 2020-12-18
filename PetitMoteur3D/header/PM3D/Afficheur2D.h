#pragma once
#include "stdafx.h"
#include <string>
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "Texture.h"
#include <vector>

using namespace DirectX;

namespace PM3D
{

	struct CSommetSprite
	{
		CSommetSprite() = default;
		CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}

		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];
		XMFLOAT3 m_Position;
		XMFLOAT2 m_CoordTex;
	};

	struct CSprite
	{
		ID3D11ShaderResourceView* pTextureD3D;
		XMMATRIX matPosDim;
		bool bPanneau;
		std::string typeSprite;
		CSprite()
			: bPanneau(false)
			, pTextureD3D(nullptr)
		{
		}
	};

	class CAfficheur2D : public CObjet3D
	{

	public:
		CAfficheur2D(CDispositifD3D11* pDispositif);
		virtual ~CAfficheur2D();
		virtual void Anime(float tempsEcoule) {};
		virtual void Draw() = 0;

		bool is2D() { return true; }
		std::string typeSprite = "sprite";
	protected:
		static CSommetSprite sommets[6];
		ID3D11Buffer* pVertexBuffer;
		CDispositifD3D11* pDispositif;

		virtual void InitEffet();

		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;
		ID3D11SamplerState* pSampleState;

	};
} // namespace PM3D