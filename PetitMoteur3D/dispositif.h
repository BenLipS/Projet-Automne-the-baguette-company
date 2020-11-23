#pragma once

namespace PM3D
{
// Constantes pour mode fen�tr� ou plein �cran
enum CDS_MODE
{
	CDS_FENETRE,
	CDS_PLEIN_ECRAN
};

//
//  Classe : CDispositif
//
//  BUT : 	Classe servant � construire un objet Dispositif 
//				qui implantera les aspects "g�n�riques" du dispositif de 
//				rendu
//
class CDispositif
{
public:
	virtual ~CDispositif() = default;

	virtual void Present();
	virtual void PresentSpecific() = 0;
};

} // namespace PM3D
