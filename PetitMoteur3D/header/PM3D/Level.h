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


#include <d3d11.h>
#include <PxPhysicsAPI.h>
#include <fstream>
#include<vector>
#include <algorithm>

namespace PM3D {

	class CDispositifD3D11;

	class Level {
	public:
		Level(Scene* sPhysique, CDispositifD3D11* pDispositif, float scaleX, float scaleY, float scaleZ, CGestionnaireDeTextures* gTexture);
		Level(const Level&) = delete;
		~Level() = default;

		SkyBox* getSkyBox() { return skyBox_.get(); }
		
		void restart();
		void start();
	private:

		//DELETE THAT
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
		void initPente(Light_Manager lm);
		
		void initAllBonus();

		void initBonus(Light_Manager lm, float _x, float _y);
		void initBlocChiz(Light_Manager lm, float _x, float _y);
		void initBlocSnow(Light_Manager lm, float _x, float _y);
		void initBlocRondin(Light_Manager lm, float _x, float _y);

		void initBonusAbs(Light_Manager lm, float _x, float _y, float _z);
		void initBlocChizAbs(Light_Manager lm, float _x, float _y, float _z);
		void initBlocSnowAbs(Light_Manager lm, float _x, float _y, float _z);
		void initBlocRondinAbs(Light_Manager lm, float _x, float _y, float _z);

		void initSkyBox();
		void initTunnel(float _x, float _y);
		void initHM(Light_Manager lm, int numPente, bool alpha = false);
		float getYwithZ(float _z) {
			float hauteur = 38202;
			float longueur = 39692;
			float coordZ = _z + 9960;
			float coordY = tan(anglePente_) * (longueur - coordZ);
			float y = coordY - 18977;
			return y;
		}

	};
}