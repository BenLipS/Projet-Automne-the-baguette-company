#pragma once
#include "dispositif.h"
#include <string>

using namespace DirectX;

namespace PM3D
{
//  Classe : CObjet3D
//
//  BUT : 	Classe de base de tous nos objets 3D
//
class CObjet3D
{
public:
	virtual ~CObjet3D() = default;

	virtual void Anime(float) = 0;
	virtual void Draw() = 0;
	virtual bool isPhysic() { return false; }
	virtual bool is2D() { return false; }

	std::string typeTag;
};

} // namespace PM3D
