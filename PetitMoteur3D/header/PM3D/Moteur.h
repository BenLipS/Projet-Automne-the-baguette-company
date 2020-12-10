#pragma once
#include "Singleton.h"
#include "dispositif.h"

#include <vector>
#include <iostream>

#include "Objet3D.h"
#include "Terrain.h"
#include "DIManipulateur.h"
#include "Camera.h"
#include "PxPhysicsAPI.h"
#include "stdafx.h"
#include "tools.h"
#include "BlocDynamic.h"
#include "BlocStatic.h"
#include "BlocRollerDynamic.h"
#include "ChargeurOBJ.h"
#include "GestionnaireDeTextures.h"
#include "ObjetMesh.h"
#include "AfficheurSprite.h"

#include <fstream>

using namespace physx;

namespace PM3D
{

	const int IMAGESPARSECONDE = 60;
	const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

	//
	//   TEMPLATE�: CMoteur
	//
	//   BUT�: Template servant � construire un objet Moteur qui implantera les
	//         aspects "g�n�riques" du moteur de jeu
	//
	//   COMMENTAIRES�:
	//
	//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques
	//        du syst�me (ex: le moteur lui-m�me, le lien vers
	//        le dispositif Direct3D), l'utilisation d'un singleton
	//        nous simplifiera plusieurs aspects.
	//
	template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
	{
	public:
		virtual void Run()
		{
			bool bBoucle = true;

			while (bBoucle)
			{
				// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
				bBoucle = RunSpecific();

				// appeler la fonction d'animation
				if (bBoucle)
				{
					bBoucle = Animation();
				}
			}
		}

		virtual int Initialisations()
		{
			// Propre � la plateforme
			InitialisationsSpecific();

			// * Initialisation du dispositif de rendu
			pDispositif = CreationDispositifSpecific(CDS_FENETRE);
			//pDispositif = CreationDispositifSpecific(CDS_PLEIN_ECRAN);

			// * Initialisation de la sc�ne
			InitScene();

			// * Initialisation des param�tres de l'animation et
			//   pr�paration de la premi�re image
			InitAnimation();

			return 0;
		}

		virtual bool Animation()
		{
			// m�thode pour lire l'heure et calculer le
			// temps �coul�
			const int64_t TempsCompteurCourant = GetTimeSpecific();
			const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

			// Est-il temps de rendre l'image?
			if (TempsEcoule > EcartTemps)
			{
				// Affichage optimis�
				pDispositif->Present(); // On enlevera �//� plus tard

				// On pr�pare la prochaine image
				AnimeScene(static_cast<float>(TempsEcoule));

				// On rend l'image sur la surface de travail
				// (tampon d'arri�re plan)
				RenderScene();

				// Calcul du temps du prochain affichage
				TempsCompteurPrecedent = TempsCompteurCourant;
			}

			return true;
		}
		// 06/12/2020
		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }
		//

		CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }

		const XMMATRIX& GetMatView() const { return m_MatView; }
		const XMMATRIX& GetMatProj() const { return m_MatProj; }
		const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

	protected:

		virtual ~CMoteur()
		{
			Cleanup();
		}

		// Sp�cifiques - Doivent �tre implant�s
		virtual bool RunSpecific() = 0;
		virtual int InitialisationsSpecific() = 0;

		virtual int64_t GetTimeSpecific() const = 0;
		virtual double GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

		virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
		virtual void BeginRenderSceneSpecific() = 0;
		virtual void EndRenderSceneSpecific() = 0;

		// Autres fonctions
		virtual int InitAnimation()
		{
			TempsSuivant = GetTimeSpecific();
			TempsCompteurPrecedent = TempsSuivant;

			// premi�re Image
			RenderScene();

			return true;
		}

		// Fonctions de rendu et de pr�sentation de la sc�ne
		virtual bool RenderScene()
		{
			BeginRenderSceneSpecific();

			// Appeler les fonctions de dessin de chaque objet de la sc�ne
			for (auto& object3D : scenePhysic_->ListeScene_)
			{
				object3D->Draw();
			}

			EndRenderSceneSpecific();
			return true;
		}

		virtual void Cleanup()
		{
			// d�truire les objets
			scenePhysic_->ListeScene_.clear();

			// D�truire le dispositif
			if (pDispositif)
			{
				delete pDispositif;
				pDispositif = nullptr;
			}

			if (scenePhysic_) {
				delete scenePhysic_;
			}
		}

		virtual int InitScene()
		{
			// Scene physX
			scenePhysic_ = new Scene();
			//Partie physique
			scenePhysic_->foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, scenePhysic_->allocator_, scenePhysic_->errorCallback_);
			//scenePhysic_->pvd_ = PxCreatePvd(*(scenePhysic_->foundation_));
			scenePhysic_->physic_ = PxCreatePhysics(PX_PHYSICS_VERSION, *(scenePhysic_->foundation_), PxTolerancesScale(), true);

			PxSceneDesc sceneDesc(scenePhysic_->physic_->getTolerancesScale());
			sceneDesc.gravity = PxVec3(0.0f, -800.0f, 0.0f);
			scenePhysic_->dispatcher_ = PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = scenePhysic_->dispatcher_;
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;
			scenePhysic_->scene_ = scenePhysic_->physic_->createScene(sceneDesc);

			scenePhysic_->material_ = scenePhysic_->physic_->createMaterial(0.5f, 0.5f, 0.6f);

			//managerController
			//PxCreateControllerManager(*(scenePhysic_->scene_));

			// Initialisation des objets 3D - cr�ation et/ou chargement
			if (!InitObjets())
			{
				return 1;
			}

			/*
			// Initialisation des matrices View et Proj
			// Dans notre cas, ces matrices sont fixes
			m_MatView = XMMatrixLookAtLH(XMVectorSet(-2000.0f, 2000.0f, 2000.0f, 1.0f),
			//m_MatView = XMMatrixLookAtLH(XMVectorSet(5.0f, 2.0f, 5.0f, 1.0f),
				XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f),
				XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

			// Calcul de VP � l'avance
			m_MatViewProj = m_MatView * m_MatProj;
			*/

			const float champDeVision = XM_PI / 4; 	// 45 degr�s
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 2.0f;
			const float planEloigne = 10000.0f;

			m_MatProj = XMMatrixPerspectiveFovLH(
				champDeVision,
				ratioDAspect,
				planRapproche,
				planEloigne);

			camera.init(XMVectorSet(0.0f, 500.0f, -300.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.7f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj, CCamera::CAMERA_TYPE::CUBE);

			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[1].get());

			camera.update((PxRigidBody*)character->getBody());

			return 0;
		}

		bool InitObjets()
		{
			//Affichage du tunnel (10/12/2020)

			//CObjetMesh* pMesh;

			// Constructeur abec format bianaire
			std::unique_ptr<CObjetMesh> pMesh = std::make_unique<CObjetMesh>("C:\\Users\\reymi\\OneDrive\\Documents\\GitHub\\projet-automne-the-baguette-company\\PetitMoteur3D\\src\\Jin\\jin.OMB", pDispositif);

			// Puis il est ajouté à la scène
			scenePhysic_->ListeScene_.push_back(std::move(pMesh));

			// Création de l'afficheur de sprites et ajout des sprites
			//d::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);j


			Light_Manager LMP{
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f) // vDMat
			};

			// Puis, il est ajout� � la sc�ne
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, PxTransform(0.0f, 1450.0f, -9800.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 200.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 620.0f, 0.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 4760.0f, 0.1f, 20000.0f, pDispositif, LMP));

			Light_Manager LMB{

			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f) // vDMat
			};

			Light_Manager LMBOr{
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f) // vDMat
			};

			// Blocs statiques
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 1000.0f, 1000.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 1000.0f, 1000.0f, 1000.0f, pDispositif, LMB));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(1100.0f, 1200.0f, -5000.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 500.0f, 500.0f, 500.0f, pDispositif, LMB));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-1500.0f, 1200.0f, -5000.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 500.0f, 500.0f, 500.0f, pDispositif, LMBOr));

			// Mur final
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 0.0f, 10000.0f), 5000.0f, 20000.0f, 10.0f, pDispositif, LMBOr));

			// Mur début
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 1000.0f, -10000.0f), 5000.0f, 1300.0f, 10.0f, pDispositif, LMBOr));

			// Tremplin
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 600.0f, -5000.0f, PxQuat(-0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 1000.0f, 1000.0f, 1000.0f, pDispositif, LMP));

			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-100.0f, -2500.0f, 5000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, -3000.0f, 6000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-400.0f, -3500.0f, 7000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(300.0f, -4000.0f, 8000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(200.0f, -4500.0f, 9000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-100.0f, -5000.0f, 10000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));

			char* filename = new char[50]("./src/heighmap_Proj52.bmp");
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename,XMFLOAT3(20.0f,5.0f,20.0f),pDispositif));
			//ListeScene.emplace_back(std::make_unique<CBlocEffect1>(2.0f, 2.0f, 2.0f, pDispositif));
			/*ListeScene.emplace_back(std::make_unique<BlocDynamic>(PxTransform(0.0f, 0.0f, 0.0f),
				10.0f,
				10.0f,
				10.0f,
				pDispositif, scenePhysic_));*/
			return true;
		}



		bool AnimeScene(float tempsEcoule)
		{
			scenePhysic_->scene_->simulate(1.0f / 60.0f);
			scenePhysic_->scene_->fetchResults(true);
			// Prendre en note le statut du clavier
			GestionnaireDeSaisie.StatutClavier();
			// Prendre en note l��tat de la souris
			GestionnaireDeSaisie.SaisirEtatSouris();

			if (camera.getType() == CCamera::CAMERA_TYPE::FREE){

				for (auto& object3D : scenePhysic_->ListeScene_)
				{
					object3D->Anime(tempsEcoule);
				}
				camera.update(tempsEcoule);
			}
			else {
			for (auto& object3D : scenePhysic_->ListeScene_)
			{
				object3D->Anime(tempsEcoule);
			}
			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[1].get());

			camera.update((PxRigidBody*)character->getBody());
			}
			return true;
		}

	protected:
		// Variables pour le temps de l'animation
		int64_t TempsSuivant;
		int64_t TempsCompteurPrecedent;

		// Le dispositif de rendu
		TClasseDispositif* pDispositif;

		// La seule sc�ne
		//std::vector<std::unique_ptr<CObjet3D>> ListeScene;

		// Les matrices
		XMMATRIX m_MatView;
		XMMATRIX m_MatProj;
		XMMATRIX m_MatViewProj;

		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;

		// Les saisies
		CDIManipulateur GestionnaireDeSaisie;

		// La camera
		CCamera camera;

		// Le Terrain
		//Terrain* terrain;

		// La scene physique
		Scene* scenePhysic_;

		//ControllerManager
		//PxControllerManager * controllerManager_;
	};
} // namespace PM3D
