#include "stdafx.h"
#include "SkyBox.h"
#include "MoteurWindows.h"

namespace PM3D {

	void SkyBox::Anime(float) noexcept
	{
		matWorld = XMMatrixTranslationFromVector(CMoteurWindows::GetInstance().getCameraPosition()); //Position
	}

}