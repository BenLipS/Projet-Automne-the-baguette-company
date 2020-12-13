#include "stdafx.h"
#include "Bonus.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
#include <iostream>
#include "IndexModel.h"
#include "tools.h"
#include "Filter.h"

using namespace physx;
using namespace DirectX;


namespace PM3D
{
	//  FONCTION : CBloc, constructeur param�tr� 
	//  BUT :	Constructeur d'une classe de bloc
	//  PARAM�TRES:
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif


	struct ShadersParams {
		XMMATRIX matWorldViewProj; // la matrice totale
		XMMATRIX matWorld;    // matrice de transformation dans le monde
		XMVECTOR vLumiere1;    // la position de la source d��clairage (Point)
		XMVECTOR vLumiere2;    // la position de la source d��clairage (Point)
		XMVECTOR vCamera;    // la position de la cam�ra
		XMVECTOR vAEcl;        // la valeur ambiante de l��clairage
		XMVECTOR vAMat;     // la valeur ambiante du mat�riau
		XMVECTOR vDEcl;     // la valeur diffuse de l��clairage
		XMVECTOR vDMat;     // la valeur diffuse du mat�riau
	};


	Bonus::Bonus(Scene* _scene, PxTransform _position, const float _radius, const float _height,
		CDispositifD3D11* _pDispositif, Light_Manager _sp) : Objet3DStatic(_scene, createRigidBody(_scene, _position, _radius, _height/2))
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, radius_(_radius)
		, height_(_height)
		, LM_(_sp)

	{

		CObjetMesh::pDispositif = _pDispositif; // Prendre en note le dispositif
		CObjetMesh::matWorld = XMMatrixIdentity();
		CObjetMesh::pVertexBuffer = nullptr;
		CObjetMesh::pIndexBuffer = nullptr;
		CObjetMesh::pVertexLayout = nullptr;
		CObjetMesh::pConstantBuffer = nullptr;

		typeTag = "bonus";
		// Les points
		XMFLOAT3 point[8] =
		{
			XMFLOAT3(-_radius, _radius, -_radius),
			XMFLOAT3(_radius, _radius, -_radius),
			XMFLOAT3(_radius, -_radius, -_radius),
			XMFLOAT3(-_radius, -_radius, -_radius),
			XMFLOAT3(-_radius, _radius, _radius),
			XMFLOAT3(-_radius, -_radius, _radius),
			XMFLOAT3(_radius, -_radius, _radius),
			XMFLOAT3(_radius, _radius, _radius)
		};

		// Calculer les normales
		const XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		const XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arri�re
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

			// L'arri�re du bloc
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

		// Cr�ation du vertex buffer et copie des sommets
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

		// Cr�ation de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Filtre pour les collisions
		setupFiltering(body_, FILTER_TYPE::BONUS, FILTER_TYPE::VEHICULE);

		// Inititalisation des shaders
		InitEffet();
	}

	void Bonus::Anime(float) noexcept
	{
		matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position
	}

	Bonus::~Bonus()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	void Bonus::InitShaders()
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

		// Cr�er l'organisation des sommets
		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			&pVertexLayout),
			DXE_CREATIONLAYOUT);

		pVSBlob->Release(); //  On n'a plus besoin du blob

		// Cr�ation d'un tampon pour les constantes du VS
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

	PxRigidStatic* Bonus::createRigidBody(Scene* _scene, PxTransform _position, const float _radius, const float _Hheight)
	{
		PxShape* shape_bonus = _scene->physic_->createShape(PxCapsuleGeometry(_radius, _Hheight), *(_scene->material_), true);
		shape_bonus->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape_bonus->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		PxRigidStatic* bodyStatic = _scene->physic_->createRigidStatic(_position);
		bodyStatic->attachShape(*shape_bonus);
		return bodyStatic;
	}
} // namespace PM3D
