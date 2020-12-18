#include "StdAfx.h"
#include "AfficheurPanneau.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"

#include "PxPhysicsAPI.h"

namespace PM3D
{

	struct ShadersParams
	{
		XMMATRIX matWVP; // la matrice totale
	};

	void CAfficheurPanneau::Draw()
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
		for (int i = 0; i < tabPanneaux.size(); ++i)
		{
			// Initialiser et sélectionner les « constantes » de l’effet
			ShadersParams sp;
			const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
			tabPanneaux[i]->matPosDim = XMMatrixIdentity() * XMMatrixScaling(tabPanneaux[i]->dimension.x, tabPanneaux[i]->dimension.y, 1.0f);
			physx::PxQuat quat{ XM_PI / 3,physx::PxVec3(0.0f,1.0f,0.0f) };
			tabPanneaux[i]->matPosDim *= XMMatrixRotationQuaternion(XMVectorSet(quat.x, quat.y, quat.z, quat.w));
			tabPanneaux[i]->matPosDim *= XMMatrixTranslation(tabPanneaux[i]->position.x, tabPanneaux[i]->position.y, tabPanneaux[i]->position.z);
			//tabPanneaux[i]->matPosDim *= XMMatrixTranslation(-1500.0f, 3500.0f, 6000.0f);
			tabPanneaux[i]->matPosDim *= viewProj;
			sp.matWVP = XMMatrixTranspose(tabPanneaux[i]->matPosDim);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
			pCB->SetConstantBuffer(pConstantBuffer);
			// Activation de la texture
			variableTexture->SetResource(tabPanneaux[i]->pTextureD3D);
			pPasse->Apply(0, pImmediateContext);
			// **** Rendu de l’objet
			pImmediateContext->Draw(6, 0);
		}
		pDispositif->DesactiverMelangeAlpha();
	}

	void CAfficheurPanneau::AjouterPanneau(const std::string& NomTexture,
		const XMFLOAT3& _position,
		float _dx, float _dy)
	{
		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();
		std::wstring ws(NomTexture.begin(), NomTexture.end());
		std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>();
		pPanneau->pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir la dimension de la texture si _dx et _dy sont à 0;
		if (_dx == 0.0f && _dy == 0.0f)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pPanneau->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			pPanneau->dimension.x = float(desc.Width);
			pPanneau->dimension.y = float(desc.Height);

			// Dimension en facteur
			pPanneau->dimension.x = pPanneau->dimension.x * 300.0f / pDispositif->GetLargeur();
			pPanneau->dimension.y = pPanneau->dimension.y * 300.0f / pDispositif->GetHauteur();
		}
		else
		{
			pPanneau->dimension.x = float(_dx);
			pPanneau->dimension.y = float(_dy);
		}

		// Position en coordonnées du monde
		const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
		pPanneau->position = _position;
		pPanneau->matPosDim = XMMatrixScaling(pPanneau->dimension.x, pPanneau->dimension.y, 1.0f) * XMMatrixTranslation(pPanneau->position.x, pPanneau->position.y, pPanneau->position.z) * viewProj;
		// On l’ajoute à notre vecteur
		tabPanneaux.push_back(std::move(pPanneau));
	}
}
