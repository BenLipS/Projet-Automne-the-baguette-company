#include "stdafx.h"
#include "Camera.h"

#include "MoteurWindows.h"

using namespace physx;
using namespace DirectX;

namespace PM3D {
	CCamera::CCamera(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in, CCamera::CAMERA_TYPE type_in) {
		init(position_in, direction_in, up_in, pMatView_in, pMatProj_in, pMatViewProj_in, type_in);
	}

	void CCamera::init(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in, CCamera::CAMERA_TYPE type_in) {

		type = type_in;
		waitForSwapFree = false;
		waitForSwapFP = false;

		position = position_in;
		direction = XMVector3Normalize(direction_in);
		up = XMVector3Normalize(up_in);

		pMatView = pMatView_in;

		pMatProj = pMatProj_in;

		pMatViewProj = pMatViewProj_in;
	}

	void CCamera::setPosition(const XMVECTOR& position_in) { position = position_in; }

	void CCamera::setDirection(const XMVECTOR& direction_in) { direction = XMVector3Normalize(direction_in); }

	void CCamera::setUp(const XMVECTOR& up_in) { up = XMVector3Normalize(up_in); }

	void CCamera::swapCameraModeFree()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		if (rMoteur.getStarted()) {
			if (type == CCamera::CAMERA_TYPE::FREE) { type = CCamera::CAMERA_TYPE::CUBE; }
			else  type = CCamera::CAMERA_TYPE::FREE ;
			waitForSwapFree = false;
		}
	}

	void CCamera::swapCameraModeFP() {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		if (rMoteur.getStarted()) {
			if (type == CCamera::CAMERA_TYPE::FPCUBE) type = CCamera::CAMERA_TYPE::CUBE;
			else type = CCamera::CAMERA_TYPE::FPCUBE;

			waitForSwapFP = false;
		}
	}

	void CCamera::update(float tempsEcoule)
	{
		tempsEcoule;
		
		// Pour les mouvements, nous utilisons le gestionnaire de saisie
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		float coeffMove = 5000.0f;
		XMVECTOR relativeZ = XMVector3Normalize(XMVector3Cross(direction, up));


		
		if (rMoteur.getStarted()) {
			// V�rifier l��tat de la touche gauche
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A)) {
				position += (coeffMove * relativeZ * tempsEcoule);
			}

			// V�rifier l��tat de la touche droite
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D)) {
				position -= (coeffMove * relativeZ * tempsEcoule);
			}

			// V�rifier l'�tat de la touche forward
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W)) {
				position += (coeffMove * direction * tempsEcoule);
			}

			// V�rifier l��tat de la touche backward
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S)) {
				position -= (coeffMove * direction * tempsEcoule);
			}

			// V�rifier l��tat de la touche SwapMode
			if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_M)) {
				waitForSwapFree = true;
			}
			else {
				if (waitForSwapFree) swapCameraModeFree();
			}

			// ******** POUR LA SOURIS ************  
			//V�rifier si d�placement vers la gauche
			if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX < 0)) {
				direction = XMVector3Transform(direction, XMMatrixRotationY(-XM_PI / (3000.0f * tempsEcoule)));
			}

			// V�rifier si d�placement vers la droite
			if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX > 0)) {
				direction = XMVector3Transform(direction, XMMatrixRotationY(XM_PI / (3000.0f * tempsEcoule)));
			}

			//V�rifier si d�placement vers le haut
			if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY < 0)) {
				direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, XM_PI / (3000.0f * tempsEcoule)));
			}

			// V�rifier si d�placement vers le bas
			if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY > 0)) {
				direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, -XM_PI / (3000.0f * tempsEcoule)));
			}
		}
		// Matrice de la vision
		*pMatView = XMMatrixLookAtLH(position, position + direction, up);

		// Recalculer matViewProj
		*pMatViewProj = (*pMatView) * (*pMatProj);
	}

	void CCamera::update(BlocRollerDynamic* _character, float tempsEcoule)
	{
		tempsEcoule;
		// Pour les mouvements, nous utilisons le gestionnaire de saisie
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		//float coeffMove = 3000.0f;
		XMVECTOR relativeZ = XMVector3Normalize(XMVector3Cross(direction, up));
		PxRigidBody* body = static_cast<PxRigidBody*>(_character->getBody());

		PxTransform pose = _character->getBody()->getGlobalPose();
		PxVec3 vecVitesse = body->getLinearVelocity();
		
		if (type == CAMERA_TYPE::CUBE) {

			float vitesseMax = _character->getVitesseBonusMax();
			float pourcentageVmax = vecVitesse.magnitude() / vitesseMax;

			if (pourcentageVmax < 0.25f)// pour eviter les tremblement a faible vitesse
				pourcentageVmax = 0.25f;

			float offsetY;
			float offsetZ;
			if (pose.p.z < 30000.f) {
				offsetY = 100.f + 500.f * pourcentageVmax;
				offsetZ = 50.f + 200.f * pourcentageVmax;
			}
			else {
				offsetY = 100.f + 500.f * pourcentageVmax;
				offsetZ = 350.0f + 200.f * pourcentageVmax;
			}

			setPosition(XMVECTOR{ pose.p.x, pose.p.y + offsetY, pose.p.z - offsetZ });
			setDirection(XMVECTOR{ 0.0f, -offsetY, offsetZ });

		} else if (type == CAMERA_TYPE::FPCUBE) {
			if (pose.p.z < 29600.f) {
				setPosition(XMVECTOR{ pose.p.x, pose.p.y + 100.0f, pose.p.z });
				setDirection(XMVECTOR{ vecVitesse.getNormalized().x, vecVitesse.getNormalized().y, vecVitesse.getNormalized().z });
			}
			else {
				float vitesseMax = _character->getVitesseBonusMax();
				float pourcentageVmax = vecVitesse.magnitude() / vitesseMax;

				if (pourcentageVmax < 0.25f)// pour eviter les tremblement a faible vitesse
					pourcentageVmax = 0.25f;

				float offsetY = 100.f + 500.f * pourcentageVmax;
				float offsetZ = 350.0f + 200.f * pourcentageVmax;
				setPosition(XMVECTOR{ pose.p.x, pose.p.y + offsetY, pose.p.z - offsetZ });
				setDirection(XMVECTOR{ 0.0f, -offsetY, offsetZ });
			}
		}
		
		// V�rifier l��tat de la touche SwapMode
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_M)) {
			waitForSwapFree = true;
		}
		else {
			if (waitForSwapFree) swapCameraModeFree();
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_1)) {
			waitForSwapFP = true;
		}
		else {
			if (waitForSwapFP) swapCameraModeFP();
		}

		// Matrice de la vision
		*pMatView = XMMatrixLookAtLH(position, position + direction, up);

		// Recalculer matViewProj
		*pMatViewProj = (*pMatView) * (*pMatProj);
	}

}