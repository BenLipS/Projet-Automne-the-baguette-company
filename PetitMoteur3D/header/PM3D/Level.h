#pragma once
#include "tools.h"
#include "stdafx.h"
#include "dispositif.h"
#include "BlocDynamic.h"
#include "BlocStatic.h"
#include "BlocRollerDynamic.h"
#include "Camera.h"
#include "Terrain.h"
#include "Bonus.h"
#include "ChargeurOBJ.h"
#include "GestionnaireDeTextures.h"
#include "SkyBox.h"
#include "Objet3Dvisuel.h"

#include "PxPhysicsAPI.h"

#include <d3d11.h>
#include <fstream>
#include <vector>
#include <algorithm>

namespace PM3D {

	class CDispositifD3D11;

	class Level {
	public:
		Level(Scene* sPhysique, CDispositifD3D11* pDispositif, float scaleX, float scaleY, float scaleZ, CGestionnaireDeTextures* gTexture);
		Level(const Level&) = delete;
		~Level() = default;

		void restart();
		void start();

	private:

		CChargeurOBJ skyboxModel;

		CChargeurOBJ tunnelModel;

		CChargeurOBJ voiture;

		CChargeurOBJ chizHDModel;
		CChargeurOBJ chizMidModel;
		CChargeurOBJ chizLowModel;

		CChargeurOBJ snowHDModel;
		CChargeurOBJ snowMidModel;
		CChargeurOBJ snowLowModel;

		CChargeurOBJ bonusHDModel;
		CChargeurOBJ bonusMidModel;
		CChargeurOBJ bonusLowModel;

		CChargeurOBJ rondinHDModel;
		CChargeurOBJ rondinMidModel;
		CChargeurOBJ rondinLowModel;

		void initlevel();

		float scaleX_, scaleY_, scaleZ_;
		float scaleFixX_, scaleFixZ_, scaleFixY_;

		float anglePente_;

		Scene* scenePhysic_;
		CDispositifD3D11* pDispositif_;
		CGestionnaireDeTextures* TexturesManager;

		std::unique_ptr<SkyBox> skyBox_;

		physx::PxTransform posDepart_;

		void initJoueur();
		void initPente();
		void initBonus(float _x, float _y);
		void initAllBonus();
		void initBlocChiz(float _x, float _y);
		void initBlocSnow(float _x, float _y);
		void initBlocRondin(float _x, float _y);

		void initBonusAbs( float _x, float _y, float _z);
		void initBlocChizAbs( float _x, float _y, float _z);
		void initBlocSnowAbs( float _x, float _y, float _z);
		void initBlocRondinAbs( float _x, float _y, float _z);

		void initSkyBox();
		void initTunnel(float _x, float _y);


		float getYwithZ(float _z) {
			float hauteur = 38202;
			float longueur = 39692;
			float coordZ = _z + 9960;
			float coordY = tan(anglePente_) * (longueur - coordZ);
			float y = coordY - 18977;
			return y;
		}

		void initHM(int numPente, bool alpha = false);


	};
}
