#pragma once
#include <vector>
#include <DirectXMath.h>
#include "objet3d.h"
#include "PxPhysicsAPI.h"

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
			CUBE
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
		void update(physx::PxRigidBody*);

		/*void SuivreObjet(XMVECTOR e_PositionObjet, XMVECTOR e_DirectionObjet)
		{
			XMMatrixLookToLH(e_PositionObjet - (3 * e_DirectionObjet), e_DirectionObjet, XMVectorSet(0.0f,1.0f,0.0f,0));
		}*/
	};
}