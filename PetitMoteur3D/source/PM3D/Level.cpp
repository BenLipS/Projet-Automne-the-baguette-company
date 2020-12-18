#include "StdAfx.h"
#include "Level.h"
#include "PlanStatic.h"
#include "GestionnaireDeTextures.h"
#include <cmath>
#include <vector>
#include "BlocEffet1.h"
#include "MoteurWindows.h"

using namespace physx;

namespace PM3D {
	Level::Level(Scene* _sPhysique, CDispositifD3D11* _pDispositif, float _scaleX, float _scaleY, float _scaleZ, CGestionnaireDeTextures* gTexture)
		:scenePhysic_{ _sPhysique }, pDispositif_{ _pDispositif }, scaleX_{ _scaleX }, scaleY_{ _scaleY }, scaleZ_{ _scaleZ }, scaleFixX_{ 1000 }, scaleFixY_{ 1000 }, scaleFixZ_{ 254 }, TexturesManager{ gTexture }
	{
		anglePente_ = atan(scaleFixZ_ * scaleZ_ / (scaleFixX_ * scaleX_)) + 0.002f;
		initlevel();
	}



	void Level::initlevel()
	{
		Light_Manager LMBOr{
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f) // vDMat
		};
		Light_Manager LMP{
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 125000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f) // vDMat
		};
		Light_Manager LMB{

			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f) // vDMat
		};

		// PROVISOIRE
		CParametresChargement paramOBJBonus0 = CParametresChargement("bonusrocket_LOD0.obj", ".\\modeles\\jin\\", true, false);
		bonusHDModel = CChargeurOBJ();
		bonusHDModel.Chargement(paramOBJBonus0);

		CParametresChargement paramOBJBonus1 = CParametresChargement("bonusrocket_LOD1.obj", ".\\modeles\\jin\\", true, false);
		bonusMidModel = CChargeurOBJ();
		bonusMidModel.Chargement(paramOBJBonus1);

		CParametresChargement paramOBJBonus2 = CParametresChargement("bonusrocket_LOD2.obj", ".\\modeles\\jin\\", true, false);
		bonusLowModel = CChargeurOBJ();
		bonusLowModel.Chargement(paramOBJBonus2);


		CParametresChargement paramOBJSky = CParametresChargement("skybox.obj", ".\\modeles\\jin\\", true, false);

		skyboxModel = CChargeurOBJ();
		skyboxModel.Chargement(paramOBJSky);

		CParametresChargement paramOBJ3 = CParametresChargement("tunnelMoinsSimple.obj", ".\\modeles\\jin\\", true, false);
		tunnelModel = CChargeurOBJ();
		tunnelModel.Chargement(paramOBJ3);

		CParametresChargement paramOBJChiz0 = CParametresChargement("chizbox_LOD0.obj", ".\\modeles\\jin\\", true, false);
		chizHDModel = CChargeurOBJ();
		chizHDModel.Chargement(paramOBJChiz0);

		CParametresChargement paramOBJChiz1 = CParametresChargement("chizbox_LOD1.obj", ".\\modeles\\jin\\", true, false);
		chizMidModel = CChargeurOBJ();
		chizMidModel.Chargement(paramOBJChiz1);

		CParametresChargement paramOBJChiz2 = CParametresChargement("chizbox_LOD2.obj", ".\\modeles\\jin\\", true, false);
		chizLowModel = CChargeurOBJ();
		chizLowModel.Chargement(paramOBJChiz2);


		CParametresChargement paramOBJSnow0 = CParametresChargement("Soapbox_Car.obj", ".\\modeles\\jin\\", true, false);
		snowHDModel = CChargeurOBJ();
		snowHDModel.Chargement(paramOBJSnow0);

		CParametresChargement paramOBJSnow1 = CParametresChargement("snowbox_LOD1.obj", ".\\modeles\\jin\\", true, false);
		snowMidModel = CChargeurOBJ();
		snowMidModel.Chargement(paramOBJSnow1);

		CParametresChargement paramOBJSnow2 = CParametresChargement("snowbox_LOD2.obj", ".\\modeles\\jin\\", true, false);
		snowLowModel = CChargeurOBJ();
		snowLowModel.Chargement(paramOBJSnow2);

		CParametresChargement voitureOBJ = CParametresChargement("Soapbox_Car.obj", ".\\modeles\\jin\\", true, false);
		voiture = CChargeurOBJ();
		voiture.Chargement(voitureOBJ);

		CParametresChargement paramOBJrondin0 = CParametresChargement("rond1.obj", ".\\modeles\\jin\\", true, false);
		rondinHDModel = CChargeurOBJ();
		rondinHDModel.Chargement(paramOBJrondin0);

		CParametresChargement paramOBJrondin1 = CParametresChargement("rond1.obj", ".\\modeles\\jin\\", true, false);
		rondinMidModel = CChargeurOBJ();
		rondinMidModel.Chargement(paramOBJrondin1);

		CParametresChargement paramOBJrondin2 = CParametresChargement("rond1.obj", ".\\modeles\\jin\\", true, false);
		rondinLowModel = CChargeurOBJ();
		rondinLowModel.Chargement(paramOBJrondin2);

		initJoueur();
		initPente(LMB);
		initHM(LMB, -200);
		initHM(LMB, 0, true);
		initHM(LMB, 1, true);
		initHM(LMB, 2, true);

		initBlocChiz(LMBOr, 135, 0); //X Y
		initBlocChiz(LMBOr, 180, 0);
		initBlocChiz(LMBOr, 215, -5);
		initBlocChiz(LMBOr, 215, -15);
		initBlocChiz(LMBOr, 680, -18);
		initBlocChiz(LMBOr, 730, -90);
		initBlocChiz(LMBOr, 730, -10);
		initBlocChiz(LMBOr, 730, 50);
		initBlocChiz(LMBOr, 920, 20);


		initBlocRondin(LMBOr, 75, -20);
		initBlocRondin(LMBOr, 75, 20);
		initBlocRondin(LMBOr, 150, -20);
		initBlocRondin(LMBOr, 150, 0);
		initBlocRondin(LMBOr, 160, 7.5);
		initBlocRondin(LMBOr, 190, 20);
		//initBlocRondin(LMBOr, 200, -7.5);


		initAllBonus();

		initTunnel(1000,0);

		initSkyBox();

		// Mur final
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 0.0f, 10000.0f), 5000.0f, 20000.0f, 10.0f, pDispositif_, LMBOr));

		// Mur d�but
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 1000.0f, -10000.0f), 5000.0f, 1300.0f, 10.0f, pDispositif_, LMBOr));

		// Tremplin
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 600.0f, -5000.0f, PxQuat(-0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 1000.0f, 1000.0f, 1000.0f, pDispositif_, LMP));
		// Terrain
		/*char* filename = new char[50]{ "./src/heighmap_Proj52.bmp" };
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_,scaleFixX_, scaleFixY_, scaleFixZ_, 0));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, 1));*/
	};
	void Level::initJoueur() {

		CChargeurOBJ* voitureCOBJ = new CChargeurOBJ(voiture);
		const std::vector<IChargeur*> listModels{ voitureCOBJ };

		// Joueur
		float const posX = -scaleX_ * scaleFixX_ / 2 + scaleZ_; //longueur  // -scaleX_ * 1000 / 2 = pos du debut de la pente
		float constexpr posY = 0.0f; // largeur // au centre de la pente
		float const posZ = scaleFixZ_ * scaleZ_ + 20; // hauteur // scaleFixZ_ * scaleZ_ = hauteur du debut de la pente
		posDepart_ = PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f)));

		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, posDepart_, 20.0f, pDispositif_, listModels));

		//std::unique_ptr<CBlocEffet1> bloc = std::make_unique<CBlocEffet1>(500.0f, 500.0f, 500.0f, pDispositif_);
		//bloc->SetTexture(TexturesManager->GetNewTexture(L".\\src\\dirt.dds", pDispositif_));

		//scenePhysic_->ListeScene_.emplace_back(move(bloc));
	}

	void Level::initHM(Light_Manager _lm, int numPente, bool alpha) {
		char* filename{};
		if (numPente == 0) {
			filename = new char[50]{ "./src/HM_Montagne.bmp" };


			std::unique_ptr<Terrain> HM = std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, numPente, alpha);
			if (!alpha) {
				HM->SetTexture(TexturesManager->GetNewTexture(L".\\src\\Neige2.dds", pDispositif_));
			}
			else {
				HM->SetAlphaTexture(TexturesManager->GetNewTexture(L".\\src\\snow10.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\neige2.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\Mask.dds", pDispositif_));
			}

			scenePhysic_->ListeScene_.emplace_back(move(HM));

		}
		else if (numPente == 1) {
			filename = new char[50]{ "./src/HM_Prairie.bmp" };

			std::unique_ptr<Terrain> HM = std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, numPente, alpha);
			if (!alpha) {
				HM->SetTexture(TexturesManager->GetNewTexture(L".\\src\\Neige2.dds", pDispositif_));
			}
			else {
				HM->SetAlphaTexture(TexturesManager->GetNewTexture(L".\\src\\snow10.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\Neige2.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\Mask.dds", pDispositif_));
			}

			scenePhysic_->ListeScene_.emplace_back(move(HM));
		}
		else if (numPente == 2) {
			filename = new char[50]{ "./src/HM_DDD.bmp" };

			std::unique_ptr<Terrain> HM = std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, numPente, alpha);
			if (!alpha) {
				HM->SetTexture(TexturesManager->GetNewTexture(L".\\src\\Neige2.dds", pDispositif_));
			}
			else {
				HM->SetAlphaTexture(TexturesManager->GetNewTexture(L".\\src\\snow.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\herbe.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\Mask.dds", pDispositif_));
			}

			scenePhysic_->ListeScene_.emplace_back(move(HM));
		}
		else {
			filename = new char[50]{ "./src/heighmap_Proj52_part2_prairie.bmp" };

			std::unique_ptr<Terrain> HM = std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, numPente, alpha);
			if (!alpha) {
				HM->SetTexture(TexturesManager->GetNewTexture(L".\\src\\Neige2.dds", pDispositif_));
			}
			else {
				HM->SetAlphaTexture(TexturesManager->GetNewTexture(L".\\src\\frozengrass100.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\rockgrass.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\mask.dds", pDispositif_));
			}

			scenePhysic_->ListeScene_.emplace_back(move(HM));
		}

	};
	void Level::initPente(Light_Manager _lm) {
		// Pente
		float constexpr posX = 0; //longueur  // au centre
		float constexpr posY = 0.0f; // largeur // au centre
		float const posZ = scaleFixZ_ * scaleZ_ / 2 - 55.f; // hauteur // centre de la pente � la mi hauteur de la pente

		float const longueur = sqrt(scaleFixZ_ * scaleZ_ * scaleFixZ_ * scaleZ_ + scaleX_ * scaleFixX_ * scaleX_ * scaleFixX_); // pythagor
		float constexpr largeur = 1300.0f;
		float constexpr epaisseur = 0.1f;

		//Arrivée
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, -(0.985f * scaleFixZ_ * scaleZ_), (1.0f * scaleFixX_ * scaleX_)), PxVec3(0.0f, 1.0f, 0.01f).getNormalized()));
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, -28000.0f, 0.0f), PxVec3(0.0f, 1.0f, 0.01f).getNormalized()))
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, posZ - 50.0f, 0.0f), PxVec3(0.0f, 1.0f, 0.1f).getNormalized()));


		//mur invisible
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(largeur / 2, 0.0f, 0.0f), PxVec3(-1.0f, 0.0f, 0.0f)));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(-largeur / 2, 0.0f, 0.0f), PxVec3(1.0f, 0.0f, 0.0f)));

		PxQuat quatPente = PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f));
		PxVec3 const normPente = quatPente.getBasisVector1();
		PxVec3 const vecPente = quatPente.getBasisVector2();
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, scaleZ_ * scaleFixZ_ / 2, 0.0f), normPente));
	}

	void Level::initBlocChiz(Light_Manager _lm, float _x, float _y) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(chizHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(chizMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(chizLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels, _lm));
	}

	void Level::initBlocSnow(Light_Manager _lm, float _x, float _y) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(snowHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(snowMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(snowLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels, _lm));
	}

	void Level::initBlocRondin(Light_Manager _lm, float _x, float _y) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(rondinHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(rondinMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(rondinLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels, _lm));
	}

	void Level::initAllBonus() {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		while (rMoteur.eraseBonus());

		Light_Manager LMB{
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere1
			XMVectorSet(10000.0f, 3000.0f, -10000.0f, 1.0f), // vLumiere2
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), // vCamera
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f), // vAEc1
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f), // vAMat
			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // vDEcl
			XMVectorSet(0.4f, 0.2f, 0.0f, 1.0f) // vDMat
		};
		initBonus(LMB, 175, 0);
		initBonus(LMB, 200, 0);
		
	}

	void Level::initBonus(Light_Manager _lm, float _x, float _y) {

		CChargeurOBJ* bonus0Instance = new CChargeurOBJ(bonusHDModel);
		CChargeurOBJ* bonus1Instance = new CChargeurOBJ(bonusMidModel);
		CChargeurOBJ* bonus2Instance = new CChargeurOBJ(bonusLowModel);
		const std::vector<IChargeur*> listModels{ bonus0Instance, bonus1Instance, bonus2Instance };

		// Pente
		float rayon = bonus0Instance->GetDistanceY() / 2;
		float demiHauteur = bonus0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 340;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;

		scenePhysic_->ListeScene_.push_back(std::make_unique<Bonus>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), rayon, demiHauteur, pDispositif_, listModels, _lm));

	}

	void Level::initTunnel(float _x, float _y) {

		CChargeurOBJ* TunnelInstance = new CChargeurOBJ(tunnelModel);

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 180;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;

		TunnelInstance->Placement(XMFLOAT3(posY, posZ, posX));

		scenePhysic_->ListeScene_.push_back(std::make_unique<Objet3Dvisuel>(TunnelInstance, pDispositif_, anglePente_, posY, posZ, posX));

	}

	void Level::initSkyBox() {

		CChargeurOBJ* skyboxInstance = new CChargeurOBJ(skyboxModel);

		scenePhysic_->ListeScene_.emplace_back(std::make_unique<SkyBox>(pDispositif_, &skyboxModel));
	}

	void Level::restart() {
		initAllBonus();
		auto it = scenePhysic_->ListeScene_.begin();
		while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
			it++;
		}if (it != scenePhysic_->ListeScene_.end()) {
			physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
			body->setGlobalPose(posDepart_);
			PxRigidDynamic* bodyD = static_cast<PxRigidDynamic*>(body);
			bodyD->setLinearVelocity(PxZero);
			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
			BlocRollerDynamic* vehicule = rMoteur.findVehiculeFromBody(body);
			vehicule->resetBonus();
		};

	}
	void Level::start() {
		auto it1 = scenePhysic_->ListeScene_.begin();
		while (it1 != scenePhysic_->ListeScene_.end() && it1->get()->typeTag != "sprite") {
			it1++;
		}if (it1 != scenePhysic_->ListeScene_.end()) {
			scenePhysic_->ListeScene_.erase(it1);
		}

		auto it = scenePhysic_->ListeScene_.begin();
		while (it != scenePhysic_->ListeScene_.end() && it->get()->typeTag != "vehicule") {
			it++;
		}if (it != scenePhysic_->ListeScene_.end()) {
			physx::PxRigidActor* body = static_cast<Objet3DPhysic*>(it->get())->getBody();
			body->setGlobalPose(posDepart_);
			PxRigidDynamic* bodyD = static_cast<PxRigidDynamic*>(body);
			bodyD->setLinearVelocity(PxZero);
			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
			BlocRollerDynamic* vehicule = rMoteur.findVehiculeFromBody(body);
			vehicule->resetBonus();
			//set la cam
			CCamera& cam = rMoteur.getCamera();
			cam.swapCameraModeFree();

			float largeur = static_cast<float>(pDispositif_->GetLargeur());
			float hauteur = static_cast<float>(pDispositif_->GetHauteur());

			std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif_);
			pAfficheurSprite->AjouterSprite(".\\src\\Elcomptero.dds"s, static_cast<int>(largeur * 0.05f), static_cast<int>(hauteur * 0.95f));
			scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
		};
	}

}
