#pragma once
#include "sommetbloc.h"

namespace PM3D {

	class CSommetTerrain : public CSommetBloc {
	public:
		CSommetTerrain() = default;
		CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& coordTex = XMFLOAT2(0.0f, 0.0f))
		: CSommetBloc(position, normal, coordTex) {
			
		}
	};
}