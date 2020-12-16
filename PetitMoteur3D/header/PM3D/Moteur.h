#pragma once
#include "Singleton.h"
#include "dispositif.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

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
#include "PlanStatic.h"
#include "Level.h"
#include "Filter.h"
#include "ContactModification.h"

#include "ChargeurOBJ.h"
#include "GestionnaireDeTextures.h"
#include "ObjetMesh.h"
#include "chargeur.h"
#include "BlocEffet1.h"
#include "AfficheurSprite.h"

using namespace std;
//using namespace physx;

namespace PM3D
{

	constexpr int IMAGESPARSECONDE = 60;
	constexpr double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

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

		physx::PxTransform getTerrainNormale() {
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();
			while (start != end && start->get()->typeTag != "terrain")
				start++;

			if (start != end) {
				PlanStatic* terrain = static_cast<PlanStatic*>(start->get());
				return terrain->getTerrainNormale();
			}
			else {
				return physx::PxTransform();
			}
		}

		pair<physx::PxVec3, physx::PxVec3> getTerrainPair() {
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();
			while (start != end && start->get()->typeTag != "terrain")
				start++;

			if (start != end) {
				PlanStatic* terrain = dynamic_cast<PlanStatic*>(start->get());
				pair<physx::PxVec3, physx::PxVec3> pair{ terrain->getNormale(), terrain->getDirection() };
				return pair;
			}
			else {
				return {};
			}
		}

		CDIManipulateur& GetGestionnaireDeSaisie() noexcept { return GestionnaireDeSaisie; }

		const XMMATRIX& GetMatView() const noexcept { return m_MatView; }
		const XMMATRIX& GetMatProj() const noexcept { return m_MatProj; }
		const XMMATRIX& GetMatViewProj() const noexcept { return m_MatViewProj; }
		CCamera& getCamera() { return camera; };
		Scene* getScenePhysic() { return scenePhysic_; }

		BlocRollerDynamic* findVehiculeFromBody(physx::PxRigidActor* _body) {
			for (int i = 0; i < scenePhysic_->ListeScene_.size(); ++i) {
				if (scenePhysic_->ListeScene_[i].get()->isPhysic() && static_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[i].get())->getBody() == _body)
					return static_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[i].get());
			}
			return nullptr;
		}

		void eraseBody(physx::PxRigidActor* _body) {
			auto it = scenePhysic_->ListeScene_.begin();
			bool erased = false;
			while (it != scenePhysic_->ListeScene_.end() && !erased) {
				if (it->get() != nullptr && it->get()->isPhysic()) {
					if (static_cast<Objet3DPhysic*>(it->get())->getBody() == _body) {
						erased = true;
					}
					else {
						it++;
					}
				}
				else {
					it++;
				}
			}
			if (erased) 
				scenePhysic_->ListeScene_.erase(it);
		}

		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }

		XMVECTOR getCameraPosition() { return camera.getPosition(); }

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
				if (object3D->typeTag != "terrain" && object3D->typeTag != "mur" && object3D->typeTag != "sprite") {
					CObjetMesh* objetMesh = static_cast<CObjetMesh*>(object3D.get());
					std::vector<IChargeur*> chargeurs = objetMesh->getChargeurs();
					if (object3D.get()->isPhysic()) {
						Objet3DPhysic* objetPhys = static_cast<Objet3DPhysic*>(object3D.get());
						physx::PxRigidActor* body = objetPhys->getBody();
						IChargeur* chargeur = objetPhys->getChargeurLODMoteur(chargeurs, body);
						if (chargeur->GetNomFichier() != objetMesh->getChargeurCourant()->GetNomFichier()) {
							objetMesh->setChargeurCourant(chargeur);
							objetMesh->TransfertObjet(*chargeur);
							objetMesh->InitEffet();
						}
					}
				}
				object3D->Draw();
			}

			EndRenderSceneSpecific();
			return true;
		}

		virtual void Cleanup() noexcept
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
			scenePhysic_->physic_ = PxCreatePhysics(PX_PHYSICS_VERSION, *(scenePhysic_->foundation_), physx::PxTolerancesScale(), true);

			physx::PxSceneDesc sceneDesc(scenePhysic_->physic_->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, -200.0f, 0.0f);
			scenePhysic_->dispatcher_ = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = scenePhysic_->dispatcher_;
			//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
			scenePhysic_->filterShader = FilterShader;
			sceneDesc.filterShader = scenePhysic_->filterShader;
			scenePhysic_->eventCallback = &contactModif_;
			sceneDesc.simulationEventCallback = scenePhysic_->eventCallback;
			scenePhysic_->modifyCallback = &contactModif_;
			sceneDesc.contactModifyCallback = scenePhysic_->modifyCallback;
			scenePhysic_->scene_ = scenePhysic_->physic_->createScene(sceneDesc);

			scenePhysic_->material_ = scenePhysic_->physic_->createMaterial(0.5f, 0.5f, 0.6f);

			//managerController
			//PxCreateControllerManager(*(scenePhysic_->scene_));

			// Initialisation des objets 3D - cr�ation et/ou chargement
			

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

			constexpr float champDeVision = XM_PI / 3; 	// 45 degr�s
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 2.0f;
			const float planEloigne = 1000000.0f;


			m_MatProj = XMMatrixPerspectiveFovLH(
				champDeVision,
				ratioDAspect,
				planRapproche,
				planEloigne);

			camera.init(XMVectorSet(0.0f, 500.0f, -300.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.7f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj,CCamera::CAMERA_TYPE::CUBE);

			if (!InitObjets())
			{
				return 1;
			}

			BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());

			camera.update(character);
			
			return 0;
		}

		bool InitObjets()
		{
			float largeur = static_cast<float>(pDispositif->GetLargeur());
			float hauteur = static_cast<float>(pDispositif->GetHauteur());

			Level const niveau(scenePhysic_, pDispositif, 20, 20, 75.5f, &TexturesManager); // scale en X Y et Z

			std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);
			// ajout de panneaux
			pAfficheurSprite->AjouterPanneau(".\\src\\Elcomptero.dds"s, XMFLOAT3(9980.0f, 0.0f, 19197.0f),2000,2000);
			pAfficheurSprite->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(0.0f, 0.0f, -1.0f));
			pAfficheurSprite->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-1.0f, 0.0f, 0.5f));
			pAfficheurSprite->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-0.5f, 0.0f, 1.0f));
			pAfficheurSprite->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-2.0f, 0.0f, 2.0f));

			// Création de l’afficheur de sprites et ajout des sprites
			
			/*pAfficheurSprite->AjouterSprite(".\\src\\Elcomptero.dds"s, static_cast<int>(largeur * 0.05f), static_cast<int>(hauteur * 0.95f));
			pAfficheurSprite->AjouterSprite(".\\src\\tree02s.dds"s, 500, 500, 100, 100);
			pAfficheurSprite->AjouterSprite(".\\src\\tree02s.dds"s, 800, 200, 100, 100);*/


			scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
			
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

			for (auto& object3D : scenePhysic_->ListeScene_)
			{
				object3D->Anime(tempsEcoule);
			}

			if (camera.getType() == CCamera::CAMERA_TYPE::FREE){
				camera.update(tempsEcoule);
			}
			else {
				BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());
				//camera.update((PxRigidBody*)character->getBody(),tempsEcoule);
				camera.update(character, tempsEcoule);
			}
			return true;
		}

	protected:
		// Variables pour le temps de l'animation
		int64_t TempsSuivant{};
		int64_t TempsCompteurPrecedent{};

		// Le dispositif de rendu
		TClasseDispositif* pDispositif{};

		// La seule sc�ne
		//std::vector<std::unique_ptr<CObjet3D>> ListeScene;

		// Les matrices
		XMMATRIX m_MatView{};
		XMMATRIX m_MatProj{};
		XMMATRIX m_MatViewProj{};

		// Les saisies
		CDIManipulateur GestionnaireDeSaisie{};

		// La camera
		CCamera camera{};

		// Le Terrain
		//Terrain* terrain;

		// La scene physique
		Scene* scenePhysic_{};

		//ControllerManager
		//PxControllerManager * controllerManager_;

		// Gestion des collisions
		ContactModification contactModif_{};

		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;

	
	};
} // namespace PM3D
