#include "stdafx.h"
#include "BlocRollerDynamic.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"
#include <math.h>
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

	constexpr float BlocRollerDynamic::vitesseMaxDefault_ = 6000.0f;
	constexpr float BlocRollerDynamic::vitesseMinDefault_ = 100.0f;
	constexpr float BlocRollerDynamic::vitesseBonusMax_ = 16000.0f;

	BlocRollerDynamic::BlocRollerDynamic(Scene* _scene, PxTransform _position, const float _radius,
		CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DDynamic(_scene, createRigidBody(_scene, _position, _radius), _pDispositif, chargeurs)
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, radius_(_radius)
		, vitesseMax_(vitesseMaxDefault_)
		, vitesseMin_(vitesseMinDefault_)
		, pSampleState(nullptr)
		, pTextureD3D(nullptr)

	{
		for( unsigned int i = 0; i < 10; i++)
			speedY_buffer.push(0.0f);

		typeTag = "vehicule";

		// Filtre pour les collisions
		setupFiltering(body_, FILTER_TYPE::VEHICULE, FILTER_TYPE::OBSTACLE | FILTER_TYPE::TERRAIN);

		// Inititalisation des shaders
		//InitEffet();
	}

	void BlocRollerDynamic::Anime(float tempsEcoule)
	{
		float constexpr vitesseBonus = 2000.f;
		setVitesseMax(nbBonus_ * vitesseBonus + vitesseMaxDefault_);
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();
		auto body = static_cast<PxRigidDynamic*>(body_);
		auto speed = body->getLinearVelocity();

		if (isContact()) {
			totalTempsEcoule += tempsEcoule;
			if (totalTempsEcoule > .75f) {
				updateContact(false);
				totalTempsEcoule = 0.f;
				//body->setLinearVelocity(PxZero);
			}
		}
		
		//PxTransform terrain = CMoteurWindows::GetInstance().getTerrainNormale();
		pair<PxVec3, PxVec3> const terrainPair = CMoteurWindows::GetInstance().getTerrainPair();

		//PxVec3 normale = terrain.q.getBasisVector1();
		PxVec3 const normale = terrainPair.first;

		PxVec3 const gauche = (-normale).cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,1,0)
		PxVec3 const droite = normale.cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,-1,0)

		PxVec3 vVitesse = speed;

		// V�rifier l��tat de la touche gauche
		float constexpr coeffMoveCote = 25;
		if (!isContact()) {
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT)) {
				auto const direction = gauche * speed.magnitude();
				vVitesse += (direction.getNormalized() * (speed.magnitude() / coeffMoveCote));
			}
			// V�rifier l��tat de la touche droite
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT)) {
				auto const direction = droite * speed.magnitude();
				vVitesse += (direction.getNormalized() * (speed.magnitude() / coeffMoveCote));
			}

			vVitesse = vVitesse.getNormalized() * speed.magnitude();

			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_UP)) {
				//upPressed_ = true;
				vVitesse = vVitesse * 1.05f;
			}

			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN)) {
				vVitesse = vVitesse * 0.99f;
			}
		}

		//((PxRigidDynamic*)body_)->setAngularVelocity(PxVec3(1.0, 0.0, 0.0).getNormalized());

		/*PxTransform pose = body_->getGlobalPose();
		//pose.q = PxQuat(0.1f, PxVec3(1.0f, 0.0f, 0.0f));

		if (pose.p.x > 2375.0f) {
			pose.p.x = 2375.0f;
			PxVec3 vitesse = body->getLinearVelocity();
			body->setLinearVelocity({ vitesse.x, vitesse.y, vitesse.z });
		}
		else if (pose.p.x < -2375.0f) {
			pose.p.x = -2375.0f;
			PxVec3 vitesse = body->getLinearVelocity();
			body->setLinearVelocity({ vitesse.y, vitesse.y, vitesse.z });
		}

		body_->setGlobalPose(pose);*/

		speedY_buffer.pop();
		speedY_buffer.push(speed.y);

		float moyenne = 0.0f;

		for (unsigned int i = 0; i < 10; i++) {
			moyenne += speedY_buffer.front();
			speedY_buffer.emplace(speedY_buffer.front());
			speedY_buffer.pop();
		}

		speed.y = moyenne/10.0f;
		
		PxVec3 const direction = speed.getNormalized();
		//normale = CMoteurWindows::GetInstance().getTerrainNormale();
		PxVec3 const sens = PxVec3(0.0f, 0.0f, 1.0f);
		PxVec3 const projete = PxVec3(direction.x, 0.0f, direction.z).getNormalized();
		float angle = acos(projete.dot(sens));

		PxVec3 const directionPente = terrainPair.second;
		if ((direction.cross(directionPente).y > 0)) {
			angle = -angle;
		}


		PxVec3 const largeurPente = normale.cross(directionPente);
		PxQuat const pente = PxQuat(acos(normale.dot(PxVec3(0.0f,1.0f,0.0f))), largeurPente);
		PxQuat const orientation = PxQuat(angle, normale).getNormalized();

		float const valProjete = normale.dot(vVitesse);
		PxVec3 vitesseFinale{ vVitesse - (valProjete * normale) };
		float const ValProjPente = directionPente.dot(vitesseFinale);

		if (ValProjPente >= 0) {
			if (vitesseFinale.magnitude() > vitesseMax_)
				body->setLinearVelocity(vitesseFinale.getNormalized() * vitesseMax_);
			else if (vitesseFinale.magnitude() < vitesseMin_)
				body->setLinearVelocity(vitesseFinale.getNormalized() * vitesseMin_);
			else
				body->setLinearVelocity(vitesseFinale);
		}
		else {
			float const coeffRemontePente = 0.9f;
			if (abs(ValProjPente) > 2000.f) {//Pour pas remonter la pente
				body->setLinearVelocity(vitesseFinale * coeffRemontePente);
			}
			else
				body->setLinearVelocity(vitesseFinale);
		}
		//body->addForce({ 10000000000.f,0.f,0.f }, PxForceMode::eIMPULSE);

		//PxQuat orientation = PxQuat(3.14f/3.0f, normale);
		if (!isContact()) {
			matWorld = XMMatrixRotationQuaternion(XMVectorSet(pente.x, pente.y, pente.z, pente.w)); //Orientation
			matWorld *= XMMatrixRotationQuaternion(XMVectorSet(orientation.x, orientation.y, orientation.z, orientation.w)); //Orientation
		}
		else {
			matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
			if (abs(totalTempsEcoule - tempsEcoule) < 0.0001f) {
				body->setLinearVelocity(vitesseFinale * 0.5f);
				suppBonus();
			}
		}
		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position

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

	PxRigidDynamic* BlocRollerDynamic::createRigidBody(Scene* _scene, PxTransform _position, const float _radius)
	{
		PxRigidDynamic* bodyDynamic = PxCreateDynamic(*(_scene->physic_), _position, PxSphereGeometry(_radius), *(_scene->material_), 10.0f);
		//dynamic->setAngularDamping(0.5f);
		//dynamic->setLinearVelocity(velocity);
		//bodyDynamic->setMass(100);
		return bodyDynamic;
	}

	void BlocRollerDynamic::SetTexture(CTexture* pTexture) { pTextureD3D = pTexture->GetD3DTexture(); }
} // namespace PM3D
