#pragma once
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"

namespace PM3D {
	static const uint16_t index_bloc[36] = {
						0,1,2,			// devant
						0,2,3,			// devant
						5,6,7,			// arri�re
						5,7,4,			// arri�re
						8,9,10,			// dessous
						8,10,11,		// dessous
						13,14,15,		// dessus
						13,15,12,		// dessus
						19,16,17,		// gauche
						19,17,18,		// gauche
						20,21,22,		// droite
						20,22,23		// droite
	};

	static const uint16_t index_pyramid[12] = {
		0,1,2,			// devant
		3,4,5,
		6,7,8,
		9,10,11
	};
}