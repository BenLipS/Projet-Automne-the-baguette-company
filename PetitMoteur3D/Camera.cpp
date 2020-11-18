#include "stdafx.h"
#include "Camera.h"

#include "MoteurWindows.h"

using namespace DirectX;

namespace PM3D {
	CCamera::CCamera(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in, CCamera::CAMERA_TYPE type_in) {
		init(position_in, direction_in, up_in, pMatView_in, pMatProj_in, pMatViewProj_in, type_in);
	}

	void CCamera::init(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in, CCamera::CAMERA_TYPE type_in) {

		type = type_in;
		waitForSwap = false;

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

	void CCamera::swapCameraMode()
	{
		if (type == CCamera::CAMERA_TYPE::FREE) type = CCamera::CAMERA_TYPE::LEVEL;
		else type = CCamera::CAMERA_TYPE::FREE;

		waitForSwap = false;
	}

	void CCamera::update(float tempsEcoule)
	{
		tempsEcoule;
		
		// Pour les mouvements, nous utilisons le gestionnaire de saisie
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		float coeffMove = 500.0f;
		XMVECTOR relativeZ = XMVector3Normalize(XMVector3Cross(direction, up));
		

		// Vérifier l’état de la touche gauche
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A)) {
			position += (coeffMove * relativeZ * tempsEcoule);
		}

		// Vérifier l’état de la touche droite
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D)) {
			position -= (coeffMove * relativeZ * tempsEcoule);
		}

		// Vérifier l'état de la touche forward
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W)) {
			position += (coeffMove * direction * tempsEcoule);
		}

		// Vérifier l’état de la touche backward
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S)) {
			position -= (coeffMove * direction * tempsEcoule);
		}
		
		// Vérifier l’état de la touche SwapMode
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_M)) {
			waitForSwap = true;
		}
		else {
			if (waitForSwap) swapCameraMode();
		}
		
		// ******** POUR LA SOURIS ************  
		//Vérifier si déplacement vers la gauche
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX < 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationY(-XM_PI / (5000.0f * tempsEcoule)));
		}

		// Vérifier si déplacement vers la droite
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX > 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationY(XM_PI / (5000.0f * tempsEcoule)));
		}

		//Vérifier si déplacement vers le haut
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY < 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, XM_PI / (5000.0f * tempsEcoule)));
		}

		// Vérifier si déplacement vers le bas
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY > 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, -XM_PI / (5000.0f * tempsEcoule)));
		}

		// Matrice de la vision
		*pMatView = XMMatrixLookAtLH(position, position + direction, up);

		// Recalculer matViewProj
		*pMatViewProj = (*pMatView) * (*pMatProj);
	}

	void CCamera::update(float y, float tempsEcoule)
	{
		tempsEcoule;

		// Pour les mouvements, nous utilisons le gestionnaire de saisie
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		float coeffMove = 500.0f;
		XMVECTOR relativeZ = XMVector3Normalize(XMVector3Cross(direction, up));
		XMVECTOR forward = -XMVector3Normalize(XMVector3Cross(relativeZ, up));


		// Vérifier l’état de la touche gauche
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A)) {
			position += (coeffMove * relativeZ * tempsEcoule);
		}

		// Vérifier l’état de la touche droite
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D)) {
			position -= (coeffMove * relativeZ * tempsEcoule);
		}

		// Vérifier l'état de la touche forward
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W)) {
			position += (coeffMove * forward * tempsEcoule);
		}

		// Vérifier l’état de la touche backward
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S)) {
			position -= (coeffMove * forward * tempsEcoule);
		}

		// Vérifier l’état de la touche SwapMode
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_M)) {
			waitForSwap = true;
		}
		else {
			if (waitForSwap) swapCameraMode();
		}

		// ******** POUR LA SOURIS ************  
		//Vérifier si déplacement vers la gauche
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX < 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationY(-XM_PI / (5000.0f * tempsEcoule)));
		}

		// Vérifier si déplacement vers la droite
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lX > 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationY(XM_PI / (5000.0f * tempsEcoule)));
		}

		//Vérifier si déplacement vers le haut
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY < 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, XM_PI / (5000.0f * tempsEcoule)));
		}

		// Vérifier si déplacement vers le bas
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) && (rGestionnaireDeSaisie.EtatSouris().lY > 0)) {
			direction = XMVector3Transform(direction, XMMatrixRotationAxis(relativeZ, -XM_PI / (5000.0f * tempsEcoule)));
		}

		position.vector4_f32[1] = y;

		// Matrice de la vision
		*pMatView = XMMatrixLookAtLH(position, position + direction, up);

		// Recalculer matViewProj
		*pMatViewProj = (*pMatView) * (*pMatProj);
	}
}