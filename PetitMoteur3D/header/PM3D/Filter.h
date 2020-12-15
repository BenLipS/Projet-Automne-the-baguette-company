#pragma once
#include "PxPhysicsAPI.h"

namespace PM3D {

	enum FILTER_TYPE {
		VEHICULE = (1 << 0),
		OBSTACLE = (1 << 1),
		TERRAIN = (1 << 2),
		MUR = (1 << 3),
		BONUS = (1 << 4)
	};

	static physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {

		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}

		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eMODIFY_CONTACTS;

		return physx::PxFilterFlag::eDEFAULT;

	}

	static void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask) {

		physx::PxFilterData filterData;
		filterData.word0 = filterGroup;
		filterData.word1 = filterMask;

		const physx::PxU32 nbShapes = actor->getNbShapes();
		physx::PxShape** shapes = static_cast<physx::PxShape**>(malloc(sizeof(physx::PxShape*) * nbShapes));
		actor->getShapes(shapes, nbShapes);

		for (physx::PxU32 i = 0; i < nbShapes; i++) {
			physx::PxShape* shape = shapes[i];
			shape->setSimulationFilterData(filterData);
		}
		free(shapes);

	}

} // namespace PM3D