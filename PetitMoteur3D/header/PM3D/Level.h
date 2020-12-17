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

#include <d3d11.h>
#include <PxPhysicsAPI.h>
#include <fstream>

namespace PM3D {

	class CDispositifD3D11;

	class Level {
	public:
		Level(Scene* sPhysique, CDispositifD3D11* pDispositif, float scaleX, float scaleY, float scaleZ, CGestionnaireDeTextures* gTexture);
		Level(const Level&) = delete;
		~Level() = default;

		SkyBox* getSkyBox() { return skyBox_.get(); }

	private:
		CChargeurOBJ chizHDModel;
		CChargeurOBJ chizMidModel;
		CChargeurOBJ chizLowModel;

		CChargeurOBJ snowHDModel;
		CChargeurOBJ snowMidModel;
		CChargeurOBJ snowLowModel;

		CChargeurOBJ bonusHDModel;
		CChargeurOBJ bonusMidModel;
		CChargeurOBJ bonusLowModel;

		void initlevel();

		float scaleX_, scaleY_, scaleZ_;
		float scaleFixX_, scaleFixZ_, scaleFixY_;
		float anglePente_;
		Scene* scenePhysic_;
		CDispositifD3D11* pDispositif_;
		CGestionnaireDeTextures* TexturesManager;

		std::unique_ptr<SkyBox> skyBox_;

		void initJoueur();
		void initPente(Light_Manager lm);
		void initBonus(Light_Manager lm, float _x, float _y);
		void initBloc(Light_Manager lm, float _x, float _y);
		void initHM(Light_Manager lm, int numPente);

		void initSkyBox();
	};
}