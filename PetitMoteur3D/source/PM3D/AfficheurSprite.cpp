#include "StdAfx.h"
#include "AfficheurSprite.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"
namespace PM3D
{

	struct ShadersParams
	{
		XMMATRIX matWVP; // la matrice totale
	};

	void CAfficheurSprite::Draw()
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Source des sommets
		UINT stride = sizeof(CSommetSprite);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);
		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
		pDispositif->ActiverMelangeAlpha();
		// Faire le rendu de tous nos sprites
		for (int i = 0; i < tabSprites.size(); ++i)
		{
			// Initialiser et sélectionner les « constantes » de l’effet
			ShadersParams sp;
			sp.matWVP = XMMatrixTranspose(tabSprites[i]->matPosDim);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
				&sp, 0, 0);
			pCB->SetConstantBuffer(pConstantBuffer);
			// Activation de la texture
			variableTexture->SetResource(tabSprites[i]->pTextureD3D);
			pPasse->Apply(0, pImmediateContext);
			// **** Rendu de l’objet
			pImmediateContext->Draw(6, 0);
		}
		pDispositif->DesactiverMelangeAlpha();
	}

	void CAfficheurSprite::AjouterSprite(const std::string& NomTexture,
		int _x, int _y,
		int _dx, int _dy)
	{
		float x, y, dx, dy;
		float posX, posY;
		float facteurX, facteurY;
		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();
		std::wstring ws(NomTexture.begin(), NomTexture.end());
		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
		pSprite->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
		// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
		if (_dx == 0 && _dy == 0)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pSprite->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);
			DXRelacher(pResource);
			DXRelacher(pTextureInterface);
			dx = float(desc.Width);
			dy = float(desc.Height);
		}
		else
		{
			dx = float(_dx);
			dy = float(_dy);
		}
		// Dimension en facteur
		facteurX = dx * 2.0f / pDispositif->GetLargeur();
		facteurY = dy * 2.0f / pDispositif->GetHauteur();
		// Position en coordonnées logiques
		// 0,0 pixel = -1,1
		x = float(_x);
		y = float(_y);
		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();
		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);
		// On l’ajoute à notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y, std::string typeTexte)
	{
		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
		pSprite->pTextureD3D = pTexture;
		pSprite->typeSprite = typeTexte;

		// Obtenir la dimension de la texture;
		ID3D11Resource* pResource;
		ID3D11Texture2D* pTextureInterface = 0;
		pSprite->pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		DXRelacher(pResource);
		DXRelacher(pTextureInterface);

		const float dx = float(desc.Width);
		const float dy = float(desc.Height);

		// Dimension en facteur
		const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
		const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonnées logiques, 0,0 pixel = -1,1   
		const float x = float(_x);
		const float y = float(_y);

		const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute à notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::SupprimerSpriteTexte(std::string typeTexte) {
		bool erased;
		do {
			erased = false;
			auto it = tabSprites.begin();
			while (it != tabSprites.end() && !erased) {
				if (it->get()->typeSprite == typeTexte) {
					erased = true;
				}
				else {
					it++;
				}
			}
			if (erased) {
				tabSprites.erase(it);
			}
		} while (erased);
	}


}

