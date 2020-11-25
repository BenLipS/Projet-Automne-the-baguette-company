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

#include <fstream>

using namespace physx;

namespace PM3D
{

	const int IMAGESPARSECONDE = 60;
	const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

	//
	//   TEMPLATE : CMoteur
	//
	//   BUT : Template servant à construire un objet Moteur qui implantera les
	//         aspects "génériques" du moteur de jeu
	//
	//   COMMENTAIRES :
	//
	//        Comme plusieurs de nos objets représenteront des éléments uniques 
	//        du système (ex: le moteur lui-même, le lien vers 
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
				// Propre à la plateforme - (Conditions d'arrêt, interface, messages)
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
			// Propre à la plateforme
			InitialisationsSpecific();

			// * Initialisation du dispositif de rendu
			pDispositif = CreationDispositifSpecific(CDS_FENETRE);
			//pDispositif = CreationDispositifSpecific(CDS_PLEIN_ECRAN);

			// * Initialisation de la scène
			InitScene();

			// * Initialisation des paramètres de l'animation et 
			//   préparation de la première image
			InitAnimation();

			return 0;
		}

		virtual bool Animation()
		{
			// méthode pour lire l'heure et calculer le 
			// temps écoulé
			const int64_t TempsCompteurCourant = GetTimeSpecific();
			const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

			// Est-il temps de rendre l'image?
			if (TempsEcoule > EcartTemps)
			{
				// Affichage optimisé
				pDispositif->Present(); // On enlevera «//» plus tard

				// On prépare la prochaine image
				AnimeScene(static_cast<float>(TempsEcoule));

				// On rend l'image sur la surface de travail
				// (tampon d'arrière plan)
				RenderScene();

				// Calcul du temps du prochain affichage
				TempsCompteurPrecedent = TempsCompteurCourant;
			}

			return true;
		}

		CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }

		const XMMATRIX& GetMatView() const { return m_MatView; }
		const XMMATRIX& GetMatProj() const { return m_MatProj; }
		const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

	protected:

		virtual ~CMoteur()
		{
			Cleanup();
		}

		// Spécifiques - Doivent être implantés
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

			// première Image
			RenderScene();

			return true;
		}

		// Fonctions de rendu et de présentation de la scène
		virtual bool RenderScene()
		{
			BeginRenderSceneSpecific();

			// Appeler les fonctions de dessin de chaque objet de la scène
			for (auto& object3D : scenePhysic_->ListeScene_)
			{
				object3D->Draw();
			}

			EndRenderSceneSpecific();
			return true;
		}

		virtual void Cleanup()
		{
			// détruire les objets
			scenePhysic_->ListeScene_.clear();

			// Détruire le dispositif
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
			sceneDesc.gravity = PxVec3(0.0f, -200.0f, 0.0f);
			scenePhysic_->dispatcher_ = PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = scenePhysic_->dispatcher_;
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;
			scenePhysic_->scene_ = scenePhysic_->physic_->createScene(sceneDesc);

			scenePhysic_->material_ = scenePhysic_->physic_->createMaterial(0.5f, 0.5f, 0.6f);

			//managerController
			//PxCreateControllerManager(*(scenePhysic_->scene_));

			// Initialisation des objets 3D - création et/ou chargement
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

			// Calcul de VP à l'avance
			m_MatViewProj = m_MatView * m_MatProj;
			*/

			const float champDeVision = XM_PI / 4; 	// 45 degrés
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 2.0f;
			const float planEloigne = 10000.0f;

			m_MatProj = XMMatrixPerspectiveFovLH(
				champDeVision,
				ratioDAspect,
				planRapproche,
				planEloigne);

			camera.init(XMVectorSet(0.0f, 500.0f, -300.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.7f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj, CCamera::CAMERA_TYPE::CUBE);

			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());

			camera.update((PxRigidBody*)character->getBody());
		
			return 0;
		}

		bool InitObjets()
		{
			// Puis, il est ajouté à la scène
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, PxTransform(0.0f, 50.0f, 0.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 50.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 0.0f, 0.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 1000.0f, 0.1f, 100000.0f, pDispositif));

			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, -500.0f, 1000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(100.0f, -1000.0f, 2000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-300.0f, -1500.0f, 3000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(400.0f, -2000.0f, 4000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-100.0f, -2500.0f, 5000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, -3000.0f, 6000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-400.0f, -3500.0f, 7000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(300.0f, -4000.0f, 8000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(200.0f, -4500.0f, 9000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(-100.0f, -5000.0f, 10000.0f, PxQuat(0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 10.0f, 1000.0f, 10.0f, pDispositif));
			
			char* filename = new char[50]("./src/heighmap_Proj51.bmp");
			scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename,XMFLOAT3(20.0f,0.0f,10.0f),pDispositif));

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
			// Prendre en note l’état de la souris
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
			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());

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

		// La seule scène
		//std::vector<std::unique_ptr<CObjet3D>> ListeScene;

		// Les matrices
		XMMATRIX m_MatView;
		XMMATRIX m_MatProj;
		XMMATRIX m_MatViewProj;

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
