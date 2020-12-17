#pragma once
#include "ObjetMesh.h"
#include "tools.h"

namespace PM3D {

    class SkyBox : public CObjetMesh
    {
	public:
		SkyBox(CDispositifD3D11* _pDispositif, IChargeur* chargeur) : CObjetMesh(chargeur, _pDispositif) {
			typeTag = "skybox";
		}


		// Destructeur
		virtual ~SkyBox() = default;

		virtual void Anime(float tempsEcoule) noexcept override;

    };

} // namespace PM3D