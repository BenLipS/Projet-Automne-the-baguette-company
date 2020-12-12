#pragma once
#include <dinput.h>

namespace PM3D {
	class CDIManipulateur
	{
		IDirectInput8* pDirectInput;
		IDirectInputDevice8* pClavier;
		IDirectInputDevice8* pSouris;
		IDirectInputDevice8* pJoystick;

		static bool bDejaInit;

		char tamponClavier[256];

	public:

		CDIManipulateur();
		~CDIManipulateur();
		bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd);

		void CDIManipulateur::StatutClavier();
		bool CDIManipulateur::ToucheAppuyee(UINT touche);
		void SaisirEtatSouris();
		const DIMOUSESTATE& EtatSouris();

	protected:
		DIMOUSESTATE mouseState;
	};

}