#pragma once
#include <vector>
#include <DirectXMath.h>
#include "BlocRollerDynamic.h"
#include "objet3d.h"

#include "PxPhysicsAPI.h"

namespace PM3D {

	class CCamera
	{
		DirectX::XMVECTOR position{};
		DirectX::XMVECTOR direction{};
		DirectX::XMVECTOR up{};
		DirectX::XMMATRIX* pMatView{};
		DirectX::XMMATRIX* pMatProj{};
		DirectX::XMMATRIX* pMatViewProj{};

	public:
		static const int HEIGHT = 100;

		enum class CAMERA_TYPE {
			FREE,
			CUBE,
			FPCUBE
		};

	private:

		CAMERA_TYPE type;

	public:
		bool waitForSwapFree = false;
		bool waitForSwapFP = false;

		CCamera() = default;
		CCamera(const DirectX::XMVECTOR& position_in, const DirectX::XMVECTOR& direction_in, const DirectX::XMVECTOR& up_in, DirectX::XMMATRIX* pMatView_in, DirectX::XMMATRIX* pMatProj_in, DirectX::XMMATRIX* pMatViewProj_in, CAMERA_TYPE type = CAMERA_TYPE::FREE);

		void init(const DirectX::XMVECTOR& position_in, const DirectX::XMVECTOR& direction_in, const DirectX::XMVECTOR& up_in, DirectX::XMMATRIX* pMatView_in, DirectX::XMMATRIX* pMatProj_in, DirectX::XMMATRIX* pMatViewProj_in, CAMERA_TYPE type = CAMERA_TYPE::FREE);

		void setPosition(const DirectX::XMVECTOR& position_in);
		void setDirection(const DirectX::XMVECTOR& direction_in);
		void setUp(const DirectX::XMVECTOR& up_in);

		CAMERA_TYPE getType() noexcept { return type; }

		XMVECTOR getPosition() noexcept { 
			XMVECTOR positionTest = position;  
			return position;
		}

		void swapCameraModeFree();
		void swapCameraModeFP();

		void update(float tempsEcoule = 0.0f);
		void update(float y, float tempsEcoule);
		void update(BlocRollerDynamic*, float tempsEcoule = 0);
	};
}