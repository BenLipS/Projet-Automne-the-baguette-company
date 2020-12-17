#pragma once
#include "objet3d.h"
#include <string>
#include "d3dx11effect.h"
#include "sommetTerrain.h"
#include "Texture.h"

namespace PM3D
{
	class CDispositifD3D11;

	class Terrain : public CObjet3D {
	public:

		Terrain(char* filename, XMFLOAT3 scale, CDispositifD3D11* pDispositif, float scaleFixX, float scaleFixY, float scaleFixZ, int numTerrain = 0, bool alpha = false);

		// Destructeur
		virtual ~Terrain();

		virtual void Anime(float) noexcept override {};
		virtual void Draw() override;


		float getHeight(float x, float z);
		int height;
		int width;

		XMFLOAT3 scale;

		void SetTexture(CTexture* pTexture);

		void SetAlphaTexture(CTexture* pTexture1, CTexture* pTexture2, CTexture* pTextureMasque);

	private:
		CSommetTerrain* sommets;

		char* filename;
		CDispositifD3D11* pDispositif;
		void InitShaders() {};
		void InitEffect();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		unsigned int* pIndices;
		int nbSommets;
		int nbPolygones;

		// D�finitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;

		//Pour les textures
		ID3D11ShaderResourceView* pTextureD3D;

		ID3D11ShaderResourceView* pTexture1;
		ID3D11ShaderResourceView* pTexture2;
		ID3D11ShaderResourceView* pTextureMasque;

		ID3D11SamplerState* pSampleState;

		int numTerrain_;
		float scaleFixX_, scaleFixZ_, scaleFixY_;

		bool alpha = false;
	};
}