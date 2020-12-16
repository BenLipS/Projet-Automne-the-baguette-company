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

	class CSprite
	{
	public:
		ID3D11ShaderResourceView* pTextureD3D;
		XMMATRIX matPosDim;
		bool bPanneau;
		CSprite()
			: bPanneau(false)
			, pTextureD3D(nullptr)
		{
		}
	};

	class CAfficheurSprite : public CObjet3D
	{

	public:
		CAfficheurSprite() = default;
		CAfficheurSprite(CDispositifD3D11* pDispositif);
		virtual ~CAfficheurSprite();
		virtual void Anime(float tempsEcoule) {};
		virtual void Draw();
		//void SetPosDim(int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterSprite(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position,float _dx = 0.0f, float _dy = 0.0f);
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
		//ID3D11ShaderResourceView* pTextureD3D;
		ID3D11SamplerState* pSampleState;

		//float dimX; //Dimensions de la texture
		//float dimY;
		//XMMATRIX matPosDim;
		// Tous nos sprites
		std::vector<std::unique_ptr<CSprite>> tabSprites;

		class CPanneau : public CSprite
		{
		public:
			XMFLOAT3 position;
			XMFLOAT2 dimension;
			CPanneau()
			{
				bPanneau = true;
			}
		};

	};
} // namespace PM3D