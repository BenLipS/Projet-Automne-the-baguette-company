#include "stdafx.h"
#include "BlocRollerDynamic.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
#include <iostream>
#include "IndexModel.h"

using namespace physx;
using namespace DirectX;

namespace PM3D
{
	//  FONCTION : CBloc, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc
	//  PARAMÈTRES:
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif

	struct ShadersParams {
		XMMATRIX matWorldViewProj; // la matrice totale
		XMMATRIX matWorld;    // matrice de transformation dans le monde
		XMVECTOR vLumiere;    // la position de la source d’éclairage (Point)
		XMVECTOR vCamera;    // la position de la caméra
		XMVECTOR vAEcl;		// la valeur ambiante de l’éclairage
		XMVECTOR vAMat;     // la valeur ambiante du matériau
		XMVECTOR vDEcl;     // la valeur diffuse de l’éclairage
		XMVECTOR vDMat;     // la valeur diffuse du matériau
	};


	BlocRollerDynamic::BlocRollerDynamic(Scene* _scene, PxTransform _position, const float _radius,
		CDispositifD3D11* _pDispositif) : Objet3DDynamic(_scene->scene_, createRigidBody(_scene, _position, _radius))
		, pDispositif(_pDispositif) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, pVertexLayout(nullptr)
		, pConstantBuffer(nullptr)
		, radius_(_radius)

	{
		typeTag = "Bloc";
		// Les points
		XMFLOAT3 point[8] =
		{
			XMFLOAT3(-_radius / 2, _radius / 2, -_radius / 2),
			XMFLOAT3(_radius / 2, _radius / 2, -_radius / 2),
			XMFLOAT3(_radius / 2, -_radius / 2, -_radius / 2),
			XMFLOAT3(-_radius / 2, -_radius / 2, -_radius / 2),
			XMFLOAT3(-_radius / 2, _radius / 2, _radius / 2),
			XMFLOAT3(-_radius / 2, -_radius / 2, _radius / 2),
			XMFLOAT3(_radius / 2, -_radius / 2, _radius / 2),
			XMFLOAT3(_radius / 2, _radius / 2, _radius / 2)
		};

		// Calculer les normales
		const XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		const XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arrière
		const XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		const XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		const XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		const XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

		CSommetBloc sommets[24] =
		{
			// Le devant du bloc
			CSommetBloc(point[0], n0),
			CSommetBloc(point[1], n0),
			CSommetBloc(point[2], n0),
			CSommetBloc(point[3], n0),

			// L'arrière du bloc
			CSommetBloc(point[4], n1),
			CSommetBloc(point[5], n1),
			CSommetBloc(point[6], n1),
			CSommetBloc(point[7], n1),

			// Le dessous du bloc
			CSommetBloc(point[3], n2),
			CSommetBloc(point[2], n2),
			CSommetBloc(point[6], n2),
			CSommetBloc(point[5], n2),

			// Le dessus du bloc
			CSommetBloc(point[0], n3),
			CSommetBloc(point[4], n3),
			CSommetBloc(point[7], n3),
			CSommetBloc(point[1], n3),

			// La face gauche
			CSommetBloc(point[0], n4),
			CSommetBloc(point[3], n4),
			CSommetBloc(point[5], n4),
			CSommetBloc(point[4], n4),

			// La face droite
			CSommetBloc(point[1], n5),
			CSommetBloc(point[7], n5),
			CSommetBloc(point[6], n5),
			CSommetBloc(point[2], n5)
		};

		// Création du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(sommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Inititalisation des shaders
		InitShaders();
	}

	void BlocRollerDynamic::Anime(float tempsEcoule)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();
		auto body = (PxRigidDynamic*)(body_);
		auto speed = body->getLinearVelocity();
		
		PxVec3 droite = PxVec3(0.0f, -1.0f, 0.0f).cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,1,0)
		PxVec3 gauche = PxVec3(0.0f, 1.0f, 0.0f).cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,-1,0)

		// Vérifier l’état de la touche gauche
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT)) {
			auto direction = speed + (droite * speed.magnitude() / 50);
			body->setLinearVelocity(direction.getNormalized() * speed.magnitude());
		}

		// Vérifier l’état de la touche droite
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT)) {
			auto direction = speed + (gauche * speed.magnitude() / 50);
			body->setLinearVelocity(direction.getNormalized() * speed.magnitude());
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_UP)) {
			auto direction = speed + (PxVec3{ 0.0f,0.0f,1.0f } * speed.magnitude() / 80);
			body->setLinearVelocity(direction.getNormalized() * speed.magnitude());
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN)) {
			auto direction = speed + (PxVec3{ 0.0f,0.0f,-10.0f } *speed.magnitude() / 20);
			if (direction.z < 1000.0f)
				body->setLinearVelocity(PxVec3{direction.x, direction.y, 0.0f}.getNormalized() * speed.magnitude());
			body->setLinearVelocity(direction.getNormalized() * speed.magnitude());
		}

		tempsEcoule;

		//((PxRigidDynamic*)body_)->setAngularVelocity(PxVec3(1.0, 0.0, 0.0).getNormalized());

		PxTransform pose = body_->getGlobalPose();
		pose.q = PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f));

		if (pose.p.x > 475.0f) {
			pose.p.x = 475.0f;
			PxVec3 vitesse = body->getLinearVelocity();
			body->setLinearVelocity({ 0.0f, vitesse.y, vitesse.z });
		}
		else if (pose.p.x < -475.0f) {
			pose.p.x = -475.0f;
			PxVec3 vitesse = body->getLinearVelocity();
			body->setLinearVelocity({ 0.0f, vitesse.y, vitesse.z });
			
		}

		body_->setGlobalPose(pose);

		matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position
	}

	void BlocRollerDynamic::Draw()
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
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Activer le VS
		pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);

		// Initialiser et sélectionner les «constantes» du VS
		ShadersParams sp;
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);
		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		// Pas de Geometry Shader
		pImmediateContext->GSSetShader(nullptr, nullptr, 0);

		// Activer le PS
		pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
		pImmediateContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

		// **** Rendu de l'objet
		pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
	}

	BlocRollerDynamic::~BlocRollerDynamic()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	void BlocRollerDynamic::InitShaders()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		ID3DBlob* pVSBlob = nullptr;
		DXEssayer(D3DCompileFromFile(L"VS_MiniPhong.hlsl",
			nullptr, nullptr,
			"MiniPhongVS",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&pVSBlob, nullptr), DXE_FICHIER_VS);

		DXEssayer(pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			nullptr,
			&pVertexShader),
			DXE_CREATION_VS);

		// Créer l'organisation des sommets
		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			&pVertexLayout),
			DXE_CREATIONLAYOUT);

		pVSBlob->Release(); //  On n'a plus besoin du blob

		// Création d'un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Compilation et chargement du pixel shader
		ID3DBlob* pPSBlob = nullptr;
		DXEssayer(D3DCompileFromFile(L"PS_MiniPhong.hlsl",
			nullptr, nullptr,
			"MiniPhongPS",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&pPSBlob,
			nullptr), DXE_FICHIER_PS);

		DXEssayer(pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
			pPSBlob->GetBufferSize(),
			nullptr,
			&pPixelShader),
			DXE_CREATION_PS);

		pPSBlob->Release(); //  On n'a plus besoin du blob
	}

	PxRigidDynamic* BlocRollerDynamic::createRigidBody(Scene* _scene, PxTransform _position, const float _radius)
	{
		PxRigidDynamic* bodyDynamic = PxCreateDynamic(*(_scene->physic_), _position, PxSphereGeometry(_radius), *(_scene->material_), 10.0f);
		//dynamic->setAngularDamping(0.5f);
		//dynamic->setLinearVelocity(velocity);
		return bodyDynamic;
	}
} // namespace PM3D
