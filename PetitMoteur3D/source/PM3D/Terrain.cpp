#include "StdAfx.h"
#include "Terrain.h"
#include "sommetTerrain.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
#include "ObjetMesh.h"
#include "Moteur.h"
#include "Camera.h"
#include "BlocRollerDynamic.h"

#include <fstream>

using namespace DirectX;
using namespace std;

namespace PM3D {

	struct ShadersParams {
		XMMATRIX matWorldViewProj; // la matrice totale 
		XMMATRIX matWorld; // matrice de transformation dans le monde
		XMVECTOR vLumiere1; // la position de la source d’éclairage 1 (Point)
		XMVECTOR vLumiere2; // la position de la source d’éclairage 2 (Globe)
		XMVECTOR vLumiere3; // la position de la source d’éclairage 3 (Globe)
		XMVECTOR vLumiere4; // la position de la source d’éclairage 4 (Globe)
		XMVECTOR vLumiere5; // la position de la source d’éclairage 5 (Globe)
		XMVECTOR vLumiere6; // la position de la source d’éclairage 6 (Globe)
		XMVECTOR vLumiere7; // la position de la source d’éclairage 7 (Globe)
		XMVECTOR vLumiere8; // la position de la source d’éclairage 8 (Globe)
		XMVECTOR vLumiere9; // la position de la source d’éclairage 9 (Globe)
		XMVECTOR vCamera; // la position de la caméra
		XMVECTOR vAEcl; // la valeur ambiante de l’éclairage
		XMVECTOR vAMat; // la valeur ambiante du matériau
		XMVECTOR vDEcl; // la valeur diffuse de l’éclairage
		XMVECTOR vDMat; // la valeur diffuse du matériau
		XMVECTOR vSEcl;	// la valeur spéculaire de l'éclairage
		XMVECTOR vSMat;	// la valeur spéculaire du matériau
		//XMVECTOR vLumiere1;
		//XMVECTOR vLumiere2;
		float puissance; // la puissance de spécularité
		int bTex; // Texture ou matériau
		XMFLOAT2 remplissage;
		XMVECTOR vTEcl; 			// la valeur de l'éclairage Tunnel
		/*
		XMMATRIX matWorldViewProj; // la matrice totale
		XMMATRIX matWorld; // matrice de transformation dans le monde
		XMVECTOR vLumiere; // la position de la source d’éclairage (Point)
		XMVECTOR vCamera; // la position de la caméra
		XMVECTOR vAEcl; // la valeur ambiante de l’éclairage
		XMVECTOR vAMat; // la valeur ambiante du matériau
		XMVECTOR vDEcl; // la valeur diffuse de l’éclairage
		XMVECTOR vDMat; // la valeur diffuse du matériau
		XMVECTOR vSEcl; // la valeur spéculaire de l’éclairage
		XMVECTOR vSMat; // la valeur spéculaire du matériau
		float puissance; // la puissance de spécularité
		int bTex; // Texture ou materiau
		XMFLOAT2 remplissage;
		*/
	};



	Terrain::Terrain(char* filenameBMP, XMFLOAT3 scale, CDispositifD3D11* pDispositif_, float scaleFixX, float scaleFixY, float scaleFixZ, int numTerrain, bool _alpha)
		: pDispositif(pDispositif_) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, rotation(0.0f)
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, pVertexLayout(nullptr)
		, pConstantBuffer(nullptr)
		, filename(filenameBMP)
		, scaleFixX_(scaleFixX)
		, scaleFixY_(scaleFixY)
		, scaleFixZ_(scaleFixZ)
		, numTerrain_(numTerrain)
		, pSampleState(nullptr)
		, pTextureD3D(nullptr)
		, alpha(_alpha)
	{
		this->scale = scale;
		typeTag = "terrain";
		int error;
		unsigned long long count;
		BITMAPFILEHEADER bitmapFileHeader;
		BITMAPINFOHEADER bitmapInfoHeader;

		//Lecture du fichier BMP
		FILE* filePtr;
		error = fopen_s(&filePtr, filename, "rb");
		if (error != 0) return;

		// Lecture du header du fichier Bitmap
		count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
		if (count != 1) return;

		// Lecture du info header du Bitmap
		count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
		if (count != 1) return;

		height = bitmapInfoHeader.biHeight;
		width = bitmapInfoHeader.biWidth;

		nbSommets = height * width;
		nbPolygones = (height - 1) * (width - 1) * 2;

		this->sommets = new CSommetTerrain[nbSommets];
		int imageSize, index;
		bool extrabit = ((nbSommets) % 2 != 0);
		if (!extrabit) imageSize = height * (width * 3);
		else imageSize = height * ((width * 3) + 1);

		// Cr�ation du tableau pour stocker les valeurs du BMP
		unsigned char* bitmapImage = new unsigned char[imageSize];
		if (!bitmapImage) return;

		// Lecture des donn�es de l'image BMP
		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
		count = fread(bitmapImage, 1, imageSize, filePtr);
		if (count != imageSize) return;

		// Fermeture du fichier
		error = fclose(filePtr);
		if (error != 0) return;

		int k{};

		XMFLOAT3 normale(0.0f, 1.0f, 0.0f);

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				// association des coordonn�es en x,z avec les hauteurs r�cup�r�es
				index = (width * (height - 1 - j)) + i;

				int heightValue = bitmapImage[k];
				
				// Cr�ation des sommmets aux bonnes coordonn�es (avec une normale par d�faut qui sera calcul�e plus tard)
				sommets[index] = CSommetTerrain(XMFLOAT3((float)((i +/* scaleFixX_ * numTerrain*/ - width/2) * scale.x), (float)((heightValue - (scaleFixZ_ - 0.33f) * numTerrain)* scale.y), (float)((j + (scaleFixX_ -5)* numTerrain -height/2)* scale.z)),normale, XMFLOAT2((float)i, (float)j));

				k += 3;
			}
			// Compensation de l'extra byte a la fin de chaque ligne dans les fichiers bitmaps de taille non divible par 2 (eg. 257x257).
			if (extrabit) k++;
		}

		for (int j = 0; j < height ; j++) {
			for (int i = 0; i < width ; i++) {

				// Calcul des normales
				XMVECTOR n1, n2, n3, n4;
				XMVECTOR v1, v2, v3, v4;
				int index0, index1, index2, index3, index4;

				n1 = n2 = n3 = n4 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

				index0 = (width * (height - 1 - j)) + i;
				index1 = (width * (height - 1 - (j+1))) + i;
				index2 = (width * (height - 1 - j)) + (i+1);
				index3 = (width * (height - 1 - (j-1))) + i;
				index4 = (width * (height - 1 - j)) + (i-1);

				if (j < width - 1)	v1 = XMVectorSet(sommets[index1].getPosition().x, sommets[index1].getPosition().y, sommets[index1].getPosition().z, 0.0f) - XMVectorSet(sommets[index0].getPosition().x, sommets[index0].getPosition().y, sommets[index0].getPosition().z, 0.0f);
				if (i < height - 1) v2 = XMVectorSet(sommets[index2].getPosition().x, sommets[index2].getPosition().y, sommets[index2].getPosition().z, 0.0f) - XMVectorSet(sommets[index0].getPosition().x, sommets[index0].getPosition().y, sommets[index0].getPosition().z, 0.0f);
				if (j > 0)			v3 = XMVectorSet(sommets[index3].getPosition().x, sommets[index3].getPosition().y, sommets[index3].getPosition().z, 0.0f) - XMVectorSet(sommets[index0].getPosition().x, sommets[index0].getPosition().y, sommets[index0].getPosition().z, 0.0f);
				if (i > 0)			v4 = XMVectorSet(sommets[index4].getPosition().x, sommets[index4].getPosition().y, sommets[index4].getPosition().z, 0.0f) - XMVectorSet(sommets[index0].getPosition().x, sommets[index0].getPosition().y, sommets[index0].getPosition().z, 0.0f);
			
				if (j < width - 1 && i < height - 1)	n1 = XMVector3Cross(v2,v1);
				if (j > 0 && i < height - 1)			n2 = XMVector3Cross(v3,v2);
				if (j > 0 && i > 0)						n3 = XMVector3Cross(v4,v3);
				if (j < width - 1 && i > 0)				n4 = XMVector3Cross(v1,v4);

				n1 = n1 + n2 + n3 + n4;
				n1 = -XMVector3Normalize(n1);
				XMFLOAT3 resultat;
				XMStoreFloat3(&resultat, n1);

				sommets[index0].setNormal(resultat);

			}
		}

		pIndices = new unsigned int[nbPolygones * 3];

		k = 0;
		for (int j = 0; j < height - 1; j++) {
			for (int i = 0; i < width - 1; i++) {
				// Cr�ation des indexs pour rendu
				pIndices[k++] = j* width + i;
				pIndices[k++] = j * width + (i + 1);
				pIndices[k++] = (j + 1) * width + (i + 1);
				pIndices[k++] = j * width + i;
				pIndices[k++] = (j + 1) * width + (i + 1);
				pIndices[k++] = (j + 1) * width + i;
			}
		}

		// Cr�ation du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetTerrain)*(nbSommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Cr�ation de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(unsigned int)*(nbPolygones*3);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pIndices;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Inititalisation des shaders
		InitEffect();
	}

	void Terrain::InitEffect() {  
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif-> GetD3DDevice(); 

		// Cr�ation d�un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) ); 

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);

		// Pour l�effet
		ID3DBlob* pFXBlob = NULL;
		if (!alpha) {
			DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0, "fx_5_0", 0, 0, &pFXBlob, 0), DXE_ERREURCREATION_FX);
		}
		else {
			DXEssayer(D3DCompileFromFile(L"MiniPhongAlpha.fx", 0, 0, 0, "fx_5_0", 0, 0, &pFXBlob, 0), DXE_ERREURCREATION_FX);
		}
		D3DX11CreateEffectFromMemory(  pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();
		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Cr�er l�organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc); 

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = NULL;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout, CSommetBloc::numElements, vsCodePtr, vsCodeLen, &pVertexLayout), DXE_CREATIONLAYOUT);

		// Initialisation des paramètres de sampling de la texture
		D3D11_SAMPLER_DESC samplerDesc; 
		
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; 
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; 
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
		samplerDesc.MipLODBias = 0.0f; 
		samplerDesc.MaxAnisotropy = 1; 
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0; 
		samplerDesc.BorderColor[2] = 0; 
		samplerDesc.BorderColor[3] = 0; 
		samplerDesc.MinLOD = 0; 
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; 
		
		// Création de l’état de sampling 
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}


	/*
		Interpole les hauteurs des points suivants pour approximer la hauteur du terrain aux coordonn�es envoy�es en param�tres
	*/

	float Terrain::getHeight(float x, float z)
	{
		x = ((x) / (scale.x) + (width / 2));  // on remet les coordonn�es dans le r�f�renciel de la heightmap
		z = ((z) / (scale.z) + (height / 2));	// on remet les coordonn�es dans le r�f�renciel de la heightmap
		int intX = static_cast<int>(x);
		int intZ = static_cast<int>(z);
		float dx = x - intX;
		float dz = z - intZ;
		
		float counter = 1.0f;

		float hxz = sommets[(width * (height - 1 - intZ)) + intX].getPosition().y;
		float y = hxz;

		if (intX < width - 1) {
			y += hxz + (dx * (sommets[(width * (height - 1 - intZ)) + (intX + 1)].getPosition().y - hxz));
			++counter;
		}

		if (intZ < height - 1) {
			y += hxz + (dz * (sommets[(width * (height - 1 - (intZ + 1))) + intX].getPosition().y - hxz));
			++counter;
		}

		if ((intX < width - 1) && (intZ < height - 1)) {

			y += hxz + static_cast<float>((sqrt(pow(dx, 2) + pow(dz, 2)) * (sommets[(width * (height - 1 - (intZ + 1))) + (intX + 1)].getPosition().y - hxz)));
			++counter;
		}
		
		return y / counter;
	}

	void Terrain::Draw()
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetBloc);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Source des index
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		// Initialiser et s�lectionner les �constantes� du VS
		ShadersParams sp;
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

		XMVECTOR view_camera = rMoteur.getCameraPosition();
		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		Scene* scenephysic = rMoteur.getScenePhysic();

		sp.vLumiere1 = XMVectorSet(0.0f, 300000.0f, 200000.0f, 1.0f);
		sp.vLumiere2 = XMVectorSet(0.0f, -3500.0f, 13500.0f, 1.0f);
		sp.vLumiere3 = XMVectorSet(0.0f, -4500.0f, 14500.0f, 1.0f);
		sp.vLumiere4 = XMVectorSet(0.0f, -5500.0f, 15500.0f, 1.0f);
		sp.vLumiere5 = XMVectorSet(0.0f, -6500.0f, 16500.0f, 1.0f);
		sp.vLumiere6 = XMVectorSet(0.0f, -7500.0f, 17500.0f, 1.0f);
		sp.vLumiere7 = XMVectorSet(0.0f, -8500.0f, 18500.0f, 1.0f);
		sp.vLumiere8 = XMVectorSet(0.0f, -9500.0f, 19500.0f, 1.0f);
		sp.vLumiere9 = XMVectorSet(0.0f, -10500.0f, 20500.0f, 1.0f);
		sp.vCamera = view_camera;
		sp.vAEcl = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f);
		sp.vDEcl = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vSMat = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vTEcl = XMVectorSet(0.8f, 0.8f, 0.2f, 1.0f);
		sp.puissance = 5;
		

		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		// Nous n�avons qu�un seul CBuffer
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(pConstantBuffer);
		
		if (!alpha) {
			ID3DX11EffectShaderResourceVariable* variableTexture;
			variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
			variableTexture->SetResource(pTextureD3D);
		}
		else {
			ID3DX11EffectShaderResourceVariable* variableTexture1;
			variableTexture1 = pEffet->GetVariableByName("texture1")->AsShaderResource();
			variableTexture1->SetResource(pTexture1);

			ID3DX11EffectShaderResourceVariable* variableTexture2;
			variableTexture2 = pEffet->GetVariableByName("texture2")->AsShaderResource();
			variableTexture2->SetResource(pTexture2);

			ID3DX11EffectShaderResourceVariable* variableTextureMasque;
			variableTextureMasque = pEffet->GetVariableByName("textureMasque")->AsShaderResource();
			variableTextureMasque->SetResource(pTextureMasque);
		}

		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);
		sp.bTex = 1;
		
		// **** Rendu de l�objet
		pPasse->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed(nbPolygones * 3, 0, 0);
	}

	Terrain::~Terrain()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	void Terrain::SetTexture(CTexture* pTexture) { pTextureD3D = pTexture->GetD3DTexture(); }

	void Terrain::SetAlphaTexture(CTexture* texture1, CTexture* texture2, CTexture* textureMasque) {
		pTexture1 = texture1->GetD3DTexture();
		pTexture2 = texture2->GetD3DTexture();
		pTextureMasque = textureMasque->GetD3DTexture();
	}
}