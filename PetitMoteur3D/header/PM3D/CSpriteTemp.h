#pragma once
#include "stdafx.h"
#include <string>
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "Texture.h"


using namespace DirectX;
namespace PM3D
{

	class CSommetSprite
	{
	public:
		CSommetSprite() = default;
		CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}
	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];
		XMFLOAT3 m_Position;
		XMFLOAT2 m_CoordTex;
	};


	class CSpriteTemp : public CObjet3D
	{

	public:
		CSpriteTemp() = default;
		CSpriteTemp(const std::string& NomTexture, CDispositifD3D11* pDispositif);
		virtual ~CSpriteTemp();
		virtual void Anime(float tempsEcoule) {};
		virtual void Draw();
		void SetPosDim(int _x, int _y, int _dx = 0, int _dy = 0);

	private:
		static CSommetSprite sommets[6];
		ID3D11Buffer* pVertexBuffer;
		CDispositifD3D11* pDispositif;
	private:
		virtual void InitEffet();
		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;
		ID3D11ShaderResourceView* pTextureD3D;
		ID3D11SamplerState* pSampleState;

		float dimX; //Dimensions de la texture
		float dimY;
		XMMATRIX matPosDim;

	};
} // namespace PM3D