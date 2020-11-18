#pragma once
#include <vector>
#include <DirectXMath.h>
#include "objet3d.h"

namespace PM3D {
	class CCamera
	{
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR direction;
		DirectX::XMVECTOR up;
		DirectX::XMMATRIX* pMatView;
		DirectX::XMMATRIX* pMatProj;
		DirectX::XMMATRIX* pMatViewProj;

	public:
		static const int HEIGHT = 100;

		enum CAMERA_TYPE {
			FREE,
			LEVEL
		};

	private:


		CAMERA_TYPE type;

	public:
		bool waitForSwap = false;
		CCamera() = default;
		CCamera(const DirectX::XMVECTOR& position_in, const DirectX::XMVECTOR& direction_in, const DirectX::XMVECTOR& up_in, DirectX::XMMATRIX* pMatView_in, DirectX::XMMATRIX* pMatProj_in, DirectX::XMMATRIX* pMatViewProj_in, CAMERA_TYPE type = FREE);

		void init(const DirectX::XMVECTOR& position_in, const DirectX::XMVECTOR& direction_in, const DirectX::XMVECTOR& up_in, DirectX::XMMATRIX* pMatView_in, DirectX::XMMATRIX* pMatProj_in, DirectX::XMMATRIX* pMatViewProj_in, CAMERA_TYPE type = FREE);

		void setPosition(const DirectX::XMVECTOR& position_in);
		void setDirection(const DirectX::XMVECTOR& direction_in);
		void setUp(const DirectX::XMVECTOR& up_in);

		CAMERA_TYPE getType() { return type; }
		XMVECTOR getPosition() { return position; }

		void swapCameraMode();

		void update(float tempsEcoule = 0.0f);
		void update(float y, float tempsEcoule);
	};
}