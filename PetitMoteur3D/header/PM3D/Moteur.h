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
#include "AfficheurPanneau.h"
#include "AfficheurTexte.h"

#include <chrono>
#include <sstream>
#include <codecvt>
#include <locale>

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
		virtual void RunEcranChargement()
		{
			Animation();
			initEcranChargement_ = false;
			InitScene();
		}

		virtual void Run()
		{
			bool bBoucle = true;

			

			while (bBoucle)
			{
				// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
				bBoucle = RunSpecific();
				if (GestionnaireDeSaisie.ToucheAppuyee(DIK_R)) {
					niveau->restart();
					resetChrono = true;
				}
				if (GestionnaireDeSaisie.ToucheAppuyee(DIK_RETURN) && isGameStarted == false ){
					isGameStarted = true;
					niveau->start();
					resetChrono = true;
					InitObjets();
				}
					
					


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
			if (TempsEcoule > EcartTemps || initEcranChargement_)
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

		PlanStatic* getTerrain(physx::PxVec3 _position) {
			std::pair< PlanStatic*, float> meilleurTerrain(nullptr, 0.0f);
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();

			std::for_each(scenePhysic_->ListeScene_.begin(), scenePhysic_->ListeScene_.end(), [&](std::unique_ptr<CObjet3D>& objet) {
				if (objet->typeTag == "pente") {
					auto temp = objet.get();
					PlanStatic* objetPlan = static_cast<PlanStatic*>(temp);
					
					float hauteur = objetPlan->getPointPlan(_position).y;
					if ((meilleurTerrain.first == nullptr || meilleurTerrain.second < hauteur) && hauteur < _position.y) {
						meilleurTerrain = { objetPlan, hauteur };
					}
				}
			});
			return meilleurTerrain.first;
		}

		physx::PxTransform getTerrainNormale() {
			auto start = scenePhysic_->ListeScene_.begin();
			auto end = scenePhysic_->ListeScene_.end();
			while (start != end && start->get()->typeTag != "pente")
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
		bool getStarted() {return isGameStarted;}

		BlocRollerDynamic* findVehiculeFromBody(physx::PxRigidActor* _body) {
			for (int i = 0; i < scenePhysic_->ListeScene_.size(); ++i) {
				if (scenePhysic_->ListeScene_[i].get()->isPhysic() && static_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[i].get())->getBody() == _body)
					return static_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[i].get());
			}
			return nullptr;
		}

		bool eraseBody(physx::PxRigidActor* _body) {
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
			return erased;
		}

		void eraseSprite(std::string _typeSprite) {
			auto it = scenePhysic_->ListeScene_.begin();
			bool erased = false;
			while (it != scenePhysic_->ListeScene_.end() && !erased) {
				if (it->get() != nullptr && it->get()->is2D()) {
					if (static_cast<CAfficheur2D*>(it->get())->typeSprite == _typeSprite) {
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
		bool eraseBonus() {
			auto it = scenePhysic_->ListeScene_.begin();
			bool erased = false;
			while (it != scenePhysic_->ListeScene_.end() && !erased) {
				if (it->get() != nullptr) {
					if (it->get()->typeTag == "bonus") {
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
			return erased;
		}

		void updatePanneauCollision() {
			auto it = scenePhysic_->ListeScene_.begin();
			while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
				it++;
			}
			if (it != scenePhysic_->ListeScene_.end()) {
				physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
				BlocRollerDynamic* vehicule = findVehiculeFromBody(body);
				physx::PxVec3 pose = static_cast<physx::PxRigidDynamic*>(body)->getGlobalPose().p;

				eraseSprite("panneau");

				std::unique_ptr<CAfficheurPanneau> pAfficheurPanneau = std::make_unique<CAfficheurPanneau>(pDispositif);
				pAfficheurPanneau->AjouterPanneau(".\\src\\PAF.dds"s, XMFLOAT3{ pose.x + 50.0f, pose.y + 20.0f, pose.z });
				scenePhysic_->ListeScene_.push_back(std::move(pAfficheurPanneau));
			}
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
				if (object3D->typeTag != "terrain" && object3D->typeTag != "pente" && object3D->typeTag != "mur" && object3D->typeTag != "sprite" && object3D->typeTag != "panneau") {
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

			//SkyBox* skybox = niveau->getSkyBox();
			//skybox->Draw();

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
		public:
			virtual int InitScene()
			{
				if (initEcranChargement_) {
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
			}

			constexpr float champDeVision = XM_PI / 3; 	// 45 degr�s
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 1.0f;
			const float planEloigne = 1000000.0f;


			m_MatProj = XMMatrixPerspectiveFovLH(
				champDeVision,
				ratioDAspect,
				planRapproche,
				planEloigne);

			camera.init(XMVectorSet(997.0f, -953.0f, 14121.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.7f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj, CCamera::CAMERA_TYPE::FREE);

			m_MatView = XMMatrixLookAtLH(XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

			m_MatViewProj = m_MatView * m_MatProj;

			if (!InitObjets())
			{
				return 1;
			}
			if (!initEcranChargement_) {
			/*BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());
			camera.update(character);*/
			}

			return 0;
		}

		bool InitObjets()
		{

			
			float largeur = static_cast<float>(pDispositif->GetLargeur());
			float hauteur = static_cast<float>(pDispositif->GetHauteur());

			if (!initEcranChargement_) {
				if (!isGameStarted) {
				std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);
				pAfficheurSprite->AjouterSprite(".\\src\\PressEnter.dds"s, static_cast<int>(largeur * 0.4f), static_cast<int>(hauteur * 0.6f));
				scenePhysic_->ListeScene_.clear();
				//Level const niveau(scenePhysic_, pDispositif, 20, 20, 75.5f, &TexturesManager); // scale en X Y et Z
				niveau = new Level(scenePhysic_, pDispositif, 20, 20, 75.5f, &TexturesManager); // scale en X Y et Z
				//std::unique_ptr<CAfficheurPanneau> pAfficheurPanneau = std::make_unique<CAfficheurPanneau>(pDispositif);
				
				
				scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
				}
				// ajout de panneaux
				//pAfficheurSprite->AjouterPanneau(".\\src\\Elcomptero.dds"s, XMFLOAT3(9980.0f, 0.0f, 19197.0f),2000,2000);
				//pAfficheurPanneau->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(1.0f, 1.0f, -2.0f));
				//pAfficheurPanneau->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(1.0f, 0.0f, -2.0f));
				//pAfficheurPanneau->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-1.0f, 0.0f, 0.5f));
				//pAfficheurPanneau->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-0.5f, 0.0f, 1.0f));
				//pAfficheurPanneau->AjouterPanneau(".\\src\\grass_v1_basic_tex.dds"s, XMFLOAT3(-2.0f, 0.0f, 2.0f));

				// Création de l’afficheur de sprites et ajout des sprites

				
				//pAfficheurSprite->AjouterSprite(".\\src\\tree02s.dds"s, 500, 500, 100, 100);
				//pAfficheurSprite->AjouterSprite(".\\src\\tree02s.dds"s, 800, 200, 100, 100);

				if (isGameStarted) {
					std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);
					CAfficheurTexte::Init();
					const Gdiplus::FontFamily oFamily(L"Comic Sans MS", nullptr);
					pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 24.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
					pTexteChrono = std::make_unique<CAfficheurTexte>(pDispositif, 200, 50, pPolice.get());
					pTexteChrono->Ecrire(L"00'00'000");
					chronoNow = std::chrono::high_resolution_clock::now();
					pAfficheurSprite->AjouterSpriteTexte(pTexteChrono->GetTextureView(), 900, 157);

					pTexteVitesse = std::make_unique<CAfficheurTexte>(pDispositif, 300, 256, pPolice.get());
					pTexteVitesse->Ecrire(L"0 km/h");
					pAfficheurSprite->AjouterSpriteTexte(pTexteVitesse->GetTextureView(), 200, 960);

					/*pTextePosition = std::make_unique<CAfficheurTexte>(pDispositif, 700, 556, pPolice.get());
					pAfficheurSprite->AjouterSpriteTexte(pTextePosition->GetTextureView(), 800, 810);*/

					scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));

					updateBonus();
					//scenePhysic_->ListeScene_.push_back(std::move(pAfficheurPanneau));
				}
					
					//scenePhysic_->ListeScene_.push_back(std::move(pAfficheurPanneau));

			}
			else {
				float largeur = static_cast<float>(pDispositif->GetLargeur());
				float hauteur = static_cast<float>(pDispositif->GetHauteur());
				std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);
				pAfficheurSprite->AjouterSprite(".\\src\\EcranChargement.dds"s, static_cast<int>(0), static_cast<int>(hauteur), static_cast<int>(largeur), static_cast<int>(hauteur));
				scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
			}

			return true;
		}


protected:
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
			if (!initEcranChargement_){
				BlocRollerDynamic* character = dynamic_cast<BlocRollerDynamic*>(scenePhysic_->ListeScene_[0].get());
				//camera.update((PxRigidBody*)character->getBody(),tempsEcoule);
				camera.update(character, tempsEcoule);
				if (isGameStarted) {
					updateChrono();

					updateSpeed();

					updateBonus();
				}

				/*if (GestionnaireDeSaisie.ToucheAppuyee(DIK_F3) && !swapPose) {
					swapPose = true;	
				}
				else if (swapPose) {
					swapPose = false;
				}

				if (swapPose) {
					updatePose();
				}*/
				if (GestionnaireDeSaisie.ToucheAppuyee(DIK_F3)) {
					swapPose = true;
				}

				if (swapPose) {
					updatePose();
				}

				totalTempsEcoule += tempsEcoule;
				if (totalTempsEcoule > 3) {
					eraseSprite("panneau");
					totalTempsEcoule = 0;
				}
			}

			return true;
		}

		void updateChrono() {
			
			auto chronoAp = std::chrono::high_resolution_clock::now();
			int dureeMin = static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(chronoAp - chronoNow).count());
			int dureeSec = abs(dureeMin * 60 - static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(chronoAp - chronoNow).count()));
			int dureeMs = abs(dureeMin * 60'000 - dureeSec * 1000 - static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(chronoAp - chronoNow).count()));
			
			auto it = scenePhysic_->ListeScene_.begin();
			while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
				it++;
			}if (it != scenePhysic_->ListeScene_.end()) {
				physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
				int posZ = static_cast<int>(body->getGlobalPose().p.z);
				if (posZ > 30000)
					stopChrono_ = true;
				else {
					tempsMin += dureeMin;
					tempsSec += dureeSec;
					tempsMs += dureeMs;
				}
				if (resetChrono) {
					tempsMin = 0;
					tempsSec = 0;
					tempsMs = 0;
					resetChrono = false;
				}
			}
			if (tempsMs > 999) {
				tempsSec += 1;
				tempsMs -= 1000;
			}

			if (tempsSec > 59) {
				tempsMin += 1;
				tempsSec -= 60;
			}
			std::stringstream str;
			if (tempsMin <= 9 && tempsSec <= 9 && tempsMs <= 9) {
				str << "0" << tempsMin << "'" << "0" << tempsSec << "'" << "00" << tempsMs;
			}
			else if (tempsMin <= 9 && tempsSec <= 9 && tempsMs <= 99) {
				str << "0" << tempsMin << "'" << "0" << tempsSec << "'" << "0" << tempsMs;
			}
			else if (tempsMin <= 9 && tempsSec <= 9 && tempsMs <= 999) {
				str << "0" << tempsMin << "'" << "0" << tempsSec << "'" << tempsMs;
			}
			else if (tempsMin <= 9 && tempsSec > 9 && tempsMs <= 9) {
				str << "0" << tempsMin << "'" << tempsSec << "'" << "00" << tempsMs;
			}
			else if (tempsMin <= 9 && tempsSec > 9 && tempsMs <= 99) {
				str << "0" << tempsMin << "'" << tempsSec << "'" << "0" << tempsMs;
			}
			else if (tempsMin <= 9 && tempsSec > 9 && tempsMs <= 999) {
				str << "0" << tempsMin << "'" << tempsSec << "'" << tempsMs;
			}
			else if (tempsMin > 9 && tempsSec <= 9 && tempsMs <= 9) {
				str << tempsMin << "'" << "0" << tempsSec << "'" << "00" << tempsMs;
			}
			else if (tempsMin > 9 && tempsSec <= 9 && tempsMs <= 99) {
				str << tempsMin << "'" << "0" << tempsSec << "'" << "0" << tempsMs;
			}
			else if (tempsMin > 9 && tempsSec <= 9 && tempsMs <= 999) {
				str << tempsMin << "'" << "0" << tempsSec << "'" << tempsMs;
			}
			else {
				str << tempsMin << "'" << tempsSec << "'" << tempsMs;
			}

			std::wstring strChrono = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.str());
			pTexteChrono->Ecrire(strChrono);
			chronoNow = std::chrono::high_resolution_clock::now();
		}

		void updateSpeed() {
			auto it = scenePhysic_->ListeScene_.begin();
			while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
				it++;
			}
			if (it != scenePhysic_->ListeScene_.end()) {
				physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
				BlocRollerDynamic* vehicule = findVehiculeFromBody(body);
				float speed = static_cast<physx::PxRigidDynamic*>(body)->getLinearVelocity().magnitude();
				float vitesseMax = vehicule->getVitesseBonusMax();
				int pourcentageVmax = static_cast<int>(speed / vitesseMax * 250);
				if (pourcentageVmax == 249)
					pourcentageVmax = 250;

				std::stringstream sstr;
				sstr << pourcentageVmax << " km/h";
				std::wstring strVitesse = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(sstr.str());
				pTexteVitesse->Ecrire(strVitesse);
			}
		}

		void updateBonus() {
			auto it = scenePhysic_->ListeScene_.begin();
			while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
				it++;
			}
			if (it != scenePhysic_->ListeScene_.end()) {
				physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
				BlocRollerDynamic* vehicule = findVehiculeFromBody(body);
				int nbBonus = vehicule->getNbBonus();

				eraseSprite("spritebonus");

				float largeur = static_cast<float>(pDispositif->GetLargeur());
				float hauteur = static_cast<float>(pDispositif->GetHauteur());
				std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif,"spritebonus");
				switch (nbBonus) {
				case 0:
					pAfficheurSprite->AjouterSprite(".\\src\\0Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				case 1:
					pAfficheurSprite->AjouterSprite(".\\src\\1Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				case 2:
					pAfficheurSprite->AjouterSprite(".\\src\\2Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				case 3:
					pAfficheurSprite->AjouterSprite(".\\src\\3Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				case 4:
					pAfficheurSprite->AjouterSprite(".\\src\\4Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				case 5:
					pAfficheurSprite->AjouterSprite(".\\src\\5Bonus.dds"s, static_cast<int>(largeur * 0.8f), static_cast<int>(hauteur * 0.95f));
					break;
				default:
					break;
				}

				scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
			}
		}

		void updatePose() {

			auto it = scenePhysic_->ListeScene_.begin();
			while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
				it++;
			}
			if (it != scenePhysic_->ListeScene_.end()) {
				physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
				BlocRollerDynamic* vehicule = findVehiculeFromBody(body);
				physx::PxVec3 pose = static_cast<physx::PxRigidDynamic*>(body)->getGlobalPose().p;
				int Px = static_cast<int>(pose.x);
				int Py = static_cast<int>(pose.y);
				int Pz = static_cast<int>(pose.z);

				int Cx = static_cast<int>(XMVectorGetX(camera.getPosition()));
				int Cy = static_cast<int>(XMVectorGetY(camera.getPosition()));
				int Cz = static_cast<int>(XMVectorGetZ(camera.getPosition()));

				std::stringstream sstr;
				sstr << "pose Camera : " << Cx << " " << Cy << " " << Cz << "\n" << "pose Personnage :" << Px << " " << Py << " " << Pz;
				std::wstring strPosition = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(sstr.str());
				pTextePosition->Ecrire(strPosition);
			}

		}

	protected:
		// Variables pour le temps de l'animation
		int64_t TempsSuivant{};
		int64_t TempsCompteurPrecedent{};

		float totalTempsEcoule{};

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

		// Le niveau
		Level* niveau;

		//ControllerManager
		//PxControllerManager * controllerManager_;

		// Gestion des collisions
		ContactModification contactModif_{};

		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;

		bool initEcranChargement_ = true;

		// Le Texte
		std::unique_ptr<CAfficheurTexte> pTexteChrono;
		std::unique_ptr<CAfficheurTexte> pTexteVitesse;
		std::unique_ptr<CAfficheurTexte> pTextePosition;

		std::chrono::steady_clock::time_point chronoNow;
		int tempsMin = 0;
		int tempsSec = 0;
		int tempsMs = 0;
		bool stopChrono_ = false;
		bool swapPose = false;
		bool resetChrono = false;
		bool isGameStarted = false;

		std::unique_ptr<Gdiplus::Font> pPolice;

	};
} // namespace PM3D
