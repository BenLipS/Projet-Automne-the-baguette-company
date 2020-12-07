#include "stdafx.h"
#include "DIManipulateur.h"
#include "util.h"
#include "resource.h" 

using namespace PM3D;

bool CDIManipulateur::bDejaInit = false;


CDIManipulateur::CDIManipulateur()
{
	pDirectInput = nullptr;
	pClavier = nullptr;
	pSouris = nullptr;
	pJoystick = nullptr;
}

CDIManipulateur::~CDIManipulateur()
{
	if (pClavier) {
		pClavier->Unacquire();
		pClavier->Release();
		pClavier = nullptr;
	}

	if (pSouris) {
		pSouris->Unacquire();
		pSouris->Release();
		pSouris = nullptr;
	}

	if (pJoystick) {
		pJoystick->Unacquire();
		pJoystick->Release();
		pJoystick = nullptr;
	}

	if (pDirectInput) {
		pDirectInput->Release();
		pClavier = nullptr;
	}
}

bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd)
{
	// Un seul objet DirectInput est permis
	if (!bDejaInit) {
		// Objet DirectInput
		DXEssayer(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& pDirectInput, NULL), ERREUR_CREATION_DIRECTINPUT);
		
		// Objet Clavier
		DXEssayer(pDirectInput->CreateDevice(GUID_SysKeyboard, &pClavier, NULL), ERREUR_CREATION_CLAVIER);
		DXEssayer(pClavier->SetDataFormat(&c_dfDIKeyboard), ERREUR_CREATION_FORMAT_CLAVIER);
		pClavier->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		pClavier->Acquire();

		// Objet Souris
		DXEssayer(pDirectInput->CreateDevice(GUID_SysMouse, &pSouris, NULL), ERREUR_CREATION_SOURIS);
		DXEssayer(pSouris->SetDataFormat(&c_dfDIMouse), ERREUR_CREATION_FORMAT_SOURIS);
		pSouris->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		pSouris->Acquire();

		// Objet Joystick 
		bDejaInit = true;
	}
	return true;
}

void PM3D::CDIManipulateur::StatutClavier()
{
	pClavier->Acquire();
	pClavier->GetDeviceState(sizeof(tamponClavier), (void*)& tamponClavier);
}

bool PM3D::CDIManipulateur::ToucheAppuyee(UINT touche)
{
	return (tamponClavier[touche] & 0x80);
}

void CDIManipulateur::SaisirEtatSouris() {
	pSouris->Acquire();
	pSouris->GetDeviceState(sizeof(mouseState), (void*)& mouseState);
}

const DIMOUSESTATE& CDIManipulateur::EtatSouris() {
	return mouseState;
}