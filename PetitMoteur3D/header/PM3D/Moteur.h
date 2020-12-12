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
#include "BlocEffet1.h"
#include "BlocDynamic.h"
#include "BlocStatic.h"
#include "BlocRollerDynamic.h"
#include "ChargeurOBJ.h"
#include "GestionnaireDeTextures.h"
#include "ObjetMesh.h"
#include "AfficheurSprite.h"
#include "PanneauPE.h"
#include "chargeur.h"
#include "TerrainStatic.h"
#include "PlanStatic.h"
#include "Level.h"
#include "Filter.h"
#include "ContactModification.h"

using namespace std;
using namespace physx;

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
		// 06/12/2020
		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }
		//

		PxTransform getTerrainNormale() {
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();
			while (start != end && start->get()->typeTag != "terrain")
				start++;

			if (start != end) {
				PlanStatic* terrain = static_cast<PlanStatic*>(start->get());
				return terrain->getTerrainNormale();
			}
			else {
				return PxTransform();
			}
		}

		pair<PxVec3, PxVec3> getTerrainPair() {
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();
			while (start != end && start->get()->typeTag != "terrain")
				start++;

			if (start != end) {
				PlanStatic* terrain = dynamic_cast<PlanStatic*>(start->get());
				pair<PxVec3, PxVec3> pair{ terrain->getNormale(), terrain->getDirection() };
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
			scenePhysic_->physic_ = PxCreatePhysics(PX_PHYSICS_VERSION, *(scenePhysic_->foundation_), PxTolerancesScale(), true);

			PxSceneDesc sceneDesc(scenePhysic_->physic_->getTolerancesScale());
			sceneDesc.gravity = PxVec3(0.0f, -2000.0f, 0.0f);
			scenePhysic_->dispatcher_ = PxDefaultCpuDispatcherCreate(2);
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

			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());
			//BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());

			camera.update(character);

			return 0;
		}

		bool InitObjets()
		{
			//Affichage du tunnel (10/12/2020)


			// Constructeur abec format bianaire
			
			//std::unique_ptr<CObjetMesh> pMesh = std::make_unique<CObjetMesh>("C:\\Users\\reymi\\OneDrive\\Documents\\GitHub\\projet-automne-the-baguette-company\\PetitMoteur3D\\src\\Jin\\jin.OMB", pDispositif);

			//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 0.0f, 10000.0f), 5000.0f, 20000.0f, 10.0f, pDispositif, LMBOr));

			// Création de l'afficheur de sprites et ajout des sprites
			//std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);

			//pAfficheurSprite->AjouterSprite("tree02s.dds", 0,0);
			//pAfficheurSprite->AjouterSprite("tree02s.dds", 500,500, 100, 100);
			//pAfficheurSprite->AjouterSprite("tree02s.dds", 800,200, 100, 100);

			// Lighe suivante lève EXCEPTION
			//scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));



			//bloc avec texture
			//std::unique_ptr<CBlocEffet1> bloc = std::make_unique<CBlocEffet1>(500.0f, 500.0f, 500.0f, pDispositif);
			//bloc->SetTexture(TexturesManager.GetNewTexture(L".\\src\\dirt.dds", pDispositif));			
			//scenePhysic_->ListeScene_.emplace_back(move(bloc));

			Level const niveau(scenePhysic_, pDispositif, 200, 200, 755.0f); // scale en X Y et Z

			//charger un obj
			/*CParametresChargement paramOBJ = CParametresChargement("jin.obj", ".\\modeles\\jin\\", true, false);
			CChargeurOBJ chargeur = CChargeurOBJ();
			chargeur.Chargement(paramOBJ);
			std::unique_ptr<CObjetMesh> pMesh = std::make_unique<CObjetMesh>(chargeur, pDispositif);
			// Puis il est ajouté à la scène
			scenePhysic_->ListeScene_.push_back(std::move(pMesh));*/

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
			BlocRollerDynamic* character = reinterpret_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());
			//BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());

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

		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;

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
	};
} // namespace PM3D
