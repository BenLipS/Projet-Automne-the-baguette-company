#include "StdAfx.h"
#include "Level.h"
#include "PlanStatic.h"
#include "GestionnaireDeTextures.h"
#include <cmath>
#include <vector>
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
		CParametresChargement paramOBJBonus0 = CParametresChargement("bonusrocket_LOD0.obj", ".\\modeles\\", true, false);
		bonusHDModel = CChargeurOBJ();
		bonusHDModel.Chargement(paramOBJBonus0);

		CParametresChargement paramOBJBonus1 = CParametresChargement("bonusrocket_LOD1.obj", ".\\modeles\\", true, false);
		bonusMidModel = CChargeurOBJ();
		bonusMidModel.Chargement(paramOBJBonus1);

		CParametresChargement paramOBJBonus2 = CParametresChargement("bonusrocket_LOD2.obj", ".\\modeles\\", true, false);
		bonusLowModel = CChargeurOBJ();
		bonusLowModel.Chargement(paramOBJBonus2);

		CParametresChargement paramOBJSky = CParametresChargement("skybox.obj", ".\\modeles\\", true, false);
		skyboxModel = CChargeurOBJ();
		skyboxModel.Chargement(paramOBJSky);

		CParametresChargement paramOBJ3 = CParametresChargement("tunnelMoinsSimple.obj", ".\\modeles\\", true, false);
		tunnelModel = CChargeurOBJ();
		tunnelModel.Chargement(paramOBJ3);

		CParametresChargement paramOBJChiz0 = CParametresChargement("chizbox_LOD0.obj", ".\\modeles\\", true, false);
		chizHDModel = CChargeurOBJ();
		chizHDModel.Chargement(paramOBJChiz0);

		CParametresChargement paramOBJChiz1 = CParametresChargement("chizbox_LOD1.obj", ".\\modeles\\", true, false);
		chizMidModel = CChargeurOBJ();
		chizMidModel.Chargement(paramOBJChiz1);

		CParametresChargement paramOBJChiz2 = CParametresChargement("chizbox_LOD2.obj", ".\\modeles\\", true, false);
		chizLowModel = CChargeurOBJ();
		chizLowModel.Chargement(paramOBJChiz2);

		CParametresChargement paramOBJSnow0 = CParametresChargement("snowbox_LOD0.obj", ".\\modeles\\", true, false);
		snowHDModel = CChargeurOBJ();
		snowHDModel.Chargement(paramOBJSnow0);

		CParametresChargement paramOBJSnow1 = CParametresChargement("snowbox_LOD1.obj", ".\\modeles\\", true, false);
		snowMidModel = CChargeurOBJ();
		snowMidModel.Chargement(paramOBJSnow1);

		CParametresChargement paramOBJSnow2 = CParametresChargement("snowbox_LOD2.obj", ".\\modeles\\", true, false);
		snowLowModel = CChargeurOBJ();
		snowLowModel.Chargement(paramOBJSnow2);

		CParametresChargement voitureOBJ = CParametresChargement("UdeSSavon8.obj", ".\\modeles\\", true, false);
		voiture = CChargeurOBJ();
		voiture.Chargement(voitureOBJ);

		CParametresChargement paramOBJrondin0 = CParametresChargement("rond1_LOD0.obj", ".\\modeles\\", true, false);
		rondinHDModel = CChargeurOBJ();
		rondinHDModel.Chargement(paramOBJrondin0);

		CParametresChargement paramOBJrondin1 = CParametresChargement("rond1_LOD1.obj", ".\\modeles\\", true, false);
		rondinMidModel = CChargeurOBJ();
		rondinMidModel.Chargement(paramOBJrondin1);

		CParametresChargement paramOBJrondin2 = CParametresChargement("rond1_LOD2.obj", ".\\modeles\\", true, false);
		rondinLowModel = CChargeurOBJ();
		rondinLowModel.Chargement(paramOBJrondin2);

		initJoueur();
		initPente();
		//initHM( -200);
		initHM( 0, true);
		initHM( 1, true);
		initHM( 2, true);

		initBlocChiz( 135, 0); //X Y
		initBlocChiz( 180, 0);
		initBlocChiz(215, -5);
		initBlocChiz( 215, -15);
		initBlocChiz(260, -20);
		initBlocChiz(280, -20);
		initBlocChiz( 260, 20);
		initBlocChiz( 280, 20);

		initBlocChiz( 350, 20);
		initBlocChiz( 350, -20);
		initBlocChiz(370, 20);
		initBlocChiz(370, -20);
		initBlocChiz( 390, 20);
		initBlocChiz(390, -20);
		initBlocChizAbs( 0, 0, 0);

		//Bottleneck
		initBlocChiz( 670, 30);
		initBlocChiz( 680, 22);
		initBlocSnow( 690, 14);
		initBlocSnow( 700, 10);
		initBlocChiz( 710, 14);
		initBlocSnow( 720, 22);
		initBlocChiz( 730, 30);
		initBlocSnow( 670, -30);
		initBlocChiz( 680, -22);
		initBlocSnow( 690, -14);
		initBlocChiz( 700, -10);
		initBlocChiz( 710, -14);
		initBlocSnow( 720, -22);
		initBlocChiz( 730, -30);
		initBlocSnow( 990, 0);
		initBlocChizAbs( -450, 0, 12500);
		initBlocChizAbs( 450, 0, 12500);

		initBlocChizAbs( 0, 0, 15000);
		initBlocChizAbs( 200, 0, 15000);
		initBlocSnowAbs( -200, 0, 15000);
		initBlocChizAbs( 0, 0, 15200);
		initBlocSnowAbs( 0, 0, 14800);

		initBlocChizAbs(-1000, 0, 30000);
		initBlocSnowAbs( -800, 0, 30000);
		initBlocChizAbs( -600, 0, 30000);
		initBlocChizAbs( 1000, 0, 30000);
		initBlocSnowAbs( 800, 0, 30000);
		initBlocChizAbs( 600, 0, 30000);

		initBlocChizAbs( -100, 0, 10440);
		initBlocChizAbs( 100, 0, 23873);


		initBlocRondin( 75, -20);
		initBlocRondin(75, 20);
		initBlocRondin( 150, -20);
		initBlocRondin( 150, 0.1f);
		initBlocRondin( 200, 7.5);
		initBlocRondin(200, 20);
		initBlocRondin( 500, 20);
		initBlocRondin( 550, -20);

		initBlocRondin( 760, -20);
		initBlocRondin(775, -20);
		initBlocRondin( 790, -20);

		initBlocRondin( 830, 20);
		initBlocRondin(845, 20);
		initBlocRondin( 860, 20);
		initBlocRondinAbs(-373, 0, 13200);
		initBlocRondinAbs(330, 0, 16175);
		initBlocRondinAbs( -230, 0, 20000);
		initBlocRondinAbs( 200, 0, 26500);
		initBlocRondinAbs(-535, 0, 24600);

		initBlocRondinAbs(-100, 0, 22533);
		initBlocRondinAbs( 500, 0, 29779);
		initBlocRondinAbs(-500, 0, 29779);

		initBlocRondinAbs(-456, 0, 17181);
		initBlocRondinAbs(22, 0, 18617);

		initBlocSnowAbs(383, 0, 17067);



		initBlocRondin(1000, 0);

		initBlocChizAbs(0, 0, 28000);


		initAllBonus();

		initTunnel(1000, 0);

		initSkyBox();
	}

	void Level::initJoueur() {

		CChargeurOBJ* voitureCOBJ = new CChargeurOBJ(voiture);
		const std::vector<IChargeur*> listModels{ voitureCOBJ };

		// Joueur
		float const posX = -scaleX_ * scaleFixX_ / 2 + scaleZ_ + 150.f; //longueur  // -scaleX_ * 1000 / 2 = pos du debut de la pente
		float constexpr posY = 0.0f ; // largeur // au centre de la pente
		float const posZ = scaleFixZ_ * scaleZ_ -150.f; // hauteur // scaleFixZ_ * scaleZ_ = hauteur du debut de la pente
		posDepart_ = PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f)));

		float rayon = voitureCOBJ->GetDistanceY() - 40.f;

		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, posDepart_, rayon, pDispositif_, listModels));
	}

	void Level::initHM(int numPente, bool alpha) {
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
				HM->SetAlphaTexture(TexturesManager->GetNewTexture(L".\\src\\Snow.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\herbe.dds", pDispositif_), TexturesManager->GetNewTexture(L".\\src\\Mask.dds", pDispositif_));
			}

			scenePhysic_->ListeScene_.emplace_back(move(HM));
		}
		else {
			filename = new char[50]{ "./src/HM_DDD.bmp" };

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
	void Level::initPente() {
		// Pente
		float constexpr posX = 0; //longueur  // au centre
		float constexpr posY = 0.0f; // largeur // au centre
		float const posZ = scaleFixZ_ * scaleZ_ / 2 - 55.f; // hauteur // centre de la pente � la mi hauteur de la pente

		float const longueur = sqrt(scaleFixZ_ * scaleZ_ * scaleFixZ_ * scaleZ_ + scaleX_ * scaleFixX_ * scaleX_ * scaleFixX_); // pythagor
		float constexpr largeur = 1300.0f;
		float constexpr epaisseur = 0.1f;

		//Arrivée
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, -(0.985f * scaleFixZ_ * scaleZ_), (1.0f * scaleFixX_ * scaleX_)), PxVec3(0.0f, 1.0f, 0.01f).getNormalized()));

		//mur invisible
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(largeur / 2 + 30.f, 0.0f, 0.0f), PxVec3(-1.0f, 0.0f, 0.0f)));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(-largeur / 2 - 30.f, 0.0f, 0.0f), PxVec3(1.0f, 0.0f, 0.0f)));

		PxQuat quatPente = PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f));
		PxVec3 const normPente = quatPente.getBasisVector1();
		PxVec3 const vecPente = quatPente.getBasisVector2();
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, scaleZ_ * scaleFixZ_ / 2, 0.0f), normPente));
	}

	void Level::initBlocChiz(float _x, float _y) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(chizHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(chizMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(chizLowModel);
		const std::vector<IChargeur*> listModels{ chiz0Instance, chiz1Instance, chiz2Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels));
	}
	void Level::initBlocChizAbs(float _x, float _y, float _z) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(chizHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(chizMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(chizLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();


		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(_x, getYwithZ(_z) + 15, _z, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels));
	}

	void Level::initBlocSnow(float _x, float _y) {


		CChargeurOBJ* snow0Instance = new CChargeurOBJ(snowHDModel);
		CChargeurOBJ* snow1Instance = new CChargeurOBJ(snowMidModel);
		CChargeurOBJ* snow2Instance = new CChargeurOBJ(snowLowModel);
		const std::vector<IChargeur*> listModels{ snow0Instance, snow1Instance, snow2Instance };

		// Pente
		float longueur = snow0Instance->GetDistanceX();
		float largeur = snow0Instance->GetDistanceY();
		float epaisseur = snow0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels));
	}
	void Level::initBlocSnowAbs(float _x, float _y, float _z) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(snowHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(snowMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(snowLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();


		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(_x, getYwithZ(_z) + 15, _z, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, listModels));
	}

	void Level::initBlocRondin(float _x, float _y) {

		CChargeurOBJ* rondin0Instance = new CChargeurOBJ(rondinHDModel);
		CChargeurOBJ* rondin1Instance = new CChargeurOBJ(rondinMidModel);
		CChargeurOBJ* rondin2Instance = new CChargeurOBJ(rondinLowModel);
		const std::vector<IChargeur*> listModels{ rondin0Instance, rondin1Instance, rondin2Instance };

		// Pente
		float longueur = rondin0Instance->GetDistanceX();
		float largeur = rondin0Instance->GetDistanceY();
		float epaisseur = rondin0Instance->GetDistanceZ();

		float const offsetZ = 250 / (cos(XM_PI - anglePente_)) + 300;
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), longueur, largeur, epaisseur, pDispositif_, listModels));
	}
	void Level::initBlocRondinAbs( float _x, float _y, float _z) {

		CChargeurOBJ* chiz0Instance = new CChargeurOBJ(rondinHDModel);
		CChargeurOBJ* chiz1Instance = new CChargeurOBJ(rondinMidModel);
		CChargeurOBJ* chiz2Instance = new CChargeurOBJ(rondinLowModel);
		const std::vector<IChargeur*> listModels{ chiz2Instance, chiz1Instance, chiz0Instance };

		// Pente
		float longueur = chiz0Instance->GetDistanceX();
		float largeur = chiz0Instance->GetDistanceY();
		float epaisseur = chiz0Instance->GetDistanceZ();

		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(_x, getYwithZ(_z) + 50, _z, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), longueur, largeur, epaisseur, pDispositif_, listModels));
	}
	void Level::initAllBonus() {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		while (rMoteur.eraseBonus());


		initBonus( 170, 0);
		initBonus(300, 0);
		initBonus( 400, 0);
		initBonus( 710, -7.5);
		initBonus( 1400, -9);
		initBonusAbs( -130, 0, 11367);

	}

	void Level::initBonus(float _x, float _y) {

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

		scenePhysic_->ListeScene_.push_back(std::make_unique<Bonus>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), rayon, demiHauteur, pDispositif_, listModels));

	}

	void Level::initBonusAbs( float _x, float _y, float _z) {

		CChargeurOBJ* bonus0Instance = new CChargeurOBJ(bonusHDModel);
		CChargeurOBJ* bonus1Instance = new CChargeurOBJ(bonusMidModel);
		CChargeurOBJ* bonus2Instance = new CChargeurOBJ(bonusLowModel);
		const std::vector<IChargeur*> listModels{ bonus0Instance, bonus1Instance, bonus2Instance };

		// Pente
		float rayon = bonus0Instance->GetDistanceY() / 2;
		float demiHauteur = bonus0Instance->GetDistanceZ();


		scenePhysic_->ListeScene_.push_back(std::make_unique<Bonus>(scenePhysic_, PxTransform(_x, getYwithZ(_z) + 5, _z, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), rayon, demiHauteur, pDispositif_, listModels));

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
		}
		if (it != scenePhysic_->ListeScene_.end()) {
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

			std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif_,"spritecompteur");
			pAfficheurSprite->AjouterSprite(".\\src\\Elcomptero.dds"s, static_cast<int>(largeur * 0.05f), static_cast<int>(hauteur * 0.95f));
			scenePhysic_->ListeScene_.push_back(std::move(pAfficheurSprite));
		};
	}
}
