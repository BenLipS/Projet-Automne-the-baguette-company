#pragma once
#include "objetMesh.h"


namespace PM3D
{
	//  Classe : Objet3Dvisuel
	//
	//  BUT : 	Affichage uniquement
	//
	class Objet3Dvisuel :
		public CObjetMesh
	{
		Objet3Dvisuel(IChargeur* chargeur, CDispositifD3D11* pDispositif) : CObjetMesh(chargeurs, _pDispositif)
	};
}