#include "stdafx.h"
#include "Objet3DPhysic.h"
#include "MoteurWindows.h"

PM3D::Objet3DPhysic::~Objet3DPhysic() {
	scene_->scene_->removeActor(*body_);
}