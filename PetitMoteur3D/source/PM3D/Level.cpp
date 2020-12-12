#include "StdAfx.h"
#include "Level.h"
#include "PlanStatic.h"

#include <cmath>


using namespace physx;

namespace PM3D {
	Level::Level(Scene* _sPhysique, CDispositifD3D11* _pDispositif, float _scaleX, float _scaleY, float _scaleZ)
		:scenePhysic_{ _sPhysique }, pDispositif_{ _pDispositif }, scaleX_{ _scaleX }, scaleY_{ _scaleY }, scaleZ_{ _scaleZ }, scaleFixX_{ 1000 }, scaleFixY_{ 1000 }, scaleFixZ_{ 254 }
	{
		anglePente_ = atan(scaleFixZ_ * scaleZ_ / (scaleFixX_ * scaleX_)) + 0.002f ;
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

		initJoueur();
		initPente( LMB);
		initHM(LMB, 0);
		initHM(LMB, 1);

		initBloc(LMBOr, 100, 100); //X Y
		initBloc(LMBOr , 220, -100);
		initBloc(LMBOr, 420, 40);
		initBloc(LMBOr, 550, -35);
		initBloc(LMBOr, 680, -18);
		initBloc(LMBOr, 730, -90);
		initBloc(LMBOr, 730, -10);
		initBloc(LMBOr, 730, 50);
		initBloc(LMBOr, 920, 20);

		initBonus(LMB, 100, 0);
		initBonus(LMB, 200, 0);
		initBonus(LMB, 300, 0);
		initBonus(LMB, 400, 0);
		initBonus(LMB, 500, 0);
		initBonus(LMB, 600, 0);
		initBonus(LMB, 700, 0);
		initBonus(LMB, 800, 0);
		// Mur final
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 0.0f, 10000.0f), 5000.0f, 20000.0f, 10.0f, pDispositif_, LMBOr));

		// Mur d�but
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 1000.0f, -10000.0f), 5000.0f, 1300.0f, 10.0f, pDispositif_, LMBOr));

		// Tremplin
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(0.0f, 600.0f, -5000.0f, PxQuat(-0.5f, PxVec3(1.0f, 0.0f, 0.0f))), 1000.0f, 1000.0f, 1000.0f, pDispositif_, LMP));
		// Terrain
		/*char* filename = new char[50]{ "./src/heighmap_Proj52.bmp" };
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_,scaleFixX_, scaleFixY_, scaleFixZ_, 0));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, 1));*/
	};
	void Level::initJoueur() {
		// Joueur
		float const posX = -scaleX_ * scaleFixX_ / 2 + scaleZ_; //longueur  // -scaleX_ * 1000 / 2 = pos du debut de la pente
		float constexpr posY = 0.0f; // largeur // au centre de la pente
		float const posZ = scaleFixZ_ * scaleZ_ + 200; // hauteur // scaleFixZ_ * scaleZ_ = hauteur du debut de la pente
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, PxTransform(0.0f, 12900.0f, -9800.0f, PxQuat(0.064f, PxVec3(1.0f, 0.0f, 0.0f))), 200.0f, pDispositif_));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocRollerDynamic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), 200.0f, pDispositif_));
	}
	void Level::initHM(Light_Manager _lm, int numPente) {
		char* filename{};
		if (numPente == 0) {
			filename = new char[50]{ "./src/heighmap_Proj52_part1.bmp" };
		}
		else if (numPente == 1) {
			filename = new char[50]{ "./src/heighmap_Proj52_part2.bmp" };
		}
		else {
			filename = new char[50]{ "./src/heighmap_Proj52_part2.bmp" };
		}
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<Terrain>(filename, XMFLOAT3(scaleX_, scaleZ_, scaleY_), pDispositif_, scaleFixX_, scaleFixY_, scaleFixZ_, numPente));
	};
	void Level::initPente(Light_Manager _lm) {
		// Pente
		float constexpr posX = 0; //longueur  // au centre
		float constexpr posY = 0.0f; // largeur // au centre
		float const posZ = scaleFixZ_ * scaleZ_ / 2  - 55.f; // hauteur // centre de la pente � la mi hauteur de la pente

		float const longueur = sqrt(scaleFixZ_ * scaleZ_ * scaleFixZ_ * scaleZ_ + scaleX_ * scaleFixX_ * scaleX_ * scaleFixX_); // pythagor
		float constexpr largeur = 80000.0f;
		float constexpr epaisseur = 0.1f;
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<TerrainStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, longueur, pDispositif_, _lm));

		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<TerrainStatic>(scenePhysic_, PxTransform(posY, posZ - scaleFixZ_ * scaleZ_ * numPente, posX + scaleFixX_ * scaleX_ * numPente, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, longueur, pDispositif_, _lm));

		//mur invisible
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(largeur / 2, 0.0f, 0.0f), PxVec3(-1.0f, 0.0f,0.0f), PxVec3(0.0f, -1.0f, 0.0f)));
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(-largeur / 2, 0.0f, 0.0f), PxVec3(1.0f, 0.0f, 0.0f), PxVec3(0.0f, -1.0f, 0.0f)));

		/*PxVec3 const ptHautPente{ 0, scaleFixZ_ * scaleZ_,-longueur / 2  };
		PxVec3 const ptzzz{ 0,0,0 };
		PxVec3 const vecPente = (ptzzz - ptHautPente).getNormalized();
		PxVec3 const vecX = { 1, 0, 0 };
		PxVec3 const normPente = (vecPente.cross(vecX)).getNormalized();*/

		PxQuat quatPente = PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f));
		PxVec3 const normPente = quatPente.getBasisVector1();
		PxVec3 const vecPente = quatPente.getBasisVector2();
		scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3( 0.0f, scaleZ_ * scaleFixZ_ /2, 0.0f), normPente, vecPente));
		//scenePhysic_->ListeScene_.emplace_back(std::make_unique<PlanStatic>(scenePhysic_, PxVec3(0.0f, 0.0f, 0.0f), PxVec3(0.0f, 1.0f, 0.0f)));
	}

	void Level::initBloc(Light_Manager _lm, float _x, float _y) {
		// Pente
		float constexpr longueur = 500.0f;
		float constexpr largeur = 500.0f;
		float constexpr epaisseur = 500.0f;

		float const offsetZ = 250 / (cos(XM_PI - anglePente_));
		float const posZ =  tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_ )/ 2;
		float const posY = _y * scaleY_;



		scenePhysic_->ListeScene_.emplace_back(std::make_unique<BlocStatic>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), largeur, epaisseur, longueur, pDispositif_, _lm));
	}

	void Level::initBonus(Light_Manager _lm, float _x, float _y) {
		// Pente
		float constexpr rayon = 500.0f;
		float constexpr demiHauteur = 200.0f;

		float const offsetZ = 250 / (cos(XM_PI - anglePente_));
		float const posZ = tan(anglePente_) * abs(scaleX_ * scaleFixX_ - _x * scaleX_) - offsetZ; // hauteur //A REVOIR
		float const posX = _x * scaleX_ - (scaleX_ * scaleFixX_) / 2;
		float const posY = _y * scaleY_;

		scenePhysic_->ListeScene_.push_back(std::make_unique<Bonus>(scenePhysic_, PxTransform(posY, posZ, posX, PxQuat(anglePente_, PxVec3(1.0f, 0.0f, 0.0f))), rayon, demiHauteur, pDispositif_, _lm));
		
	}
}
