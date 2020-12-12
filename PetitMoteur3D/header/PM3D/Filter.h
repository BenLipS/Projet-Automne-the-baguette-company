#pragma once
#include "PxPhysicsAPI.h"

using namespace physx;

namespace PM3D {

	enum FILTER_TYPE {
		VEHICULE = (1 << 0),
		OBSTACLE = (1 << 1),
		TERRAIN = (1 << 2),
		MUR = (1 << 3),
		BONUS = (1 << 4)
	};

	static PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {

		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eMODIFY_CONTACTS;

		return PxFilterFlag::eDEFAULT;

	}

	static void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask) {

		PxFilterData filterData;
		filterData.word0 = filterGroup;
		filterData.word1 = filterMask;

		const PxU32 nbShapes = actor->getNbShapes();
		PxShape** shapes = static_cast<PxShape**>(malloc(sizeof(PxShape*) * nbShapes));
		actor->getShapes(shapes, nbShapes);

		for (PxU32 i = 0; i < nbShapes; i++) {
			PxShape* shape = shapes[i];
			shape->setSimulationFilterData(filterData);
		}
		free(shapes);

	}

} // namespace PM3D