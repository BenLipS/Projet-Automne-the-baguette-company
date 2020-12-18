#pragma once
#include "PxPhysicsAPI.h"

namespace PM3D {

	struct ContactModification : public physx::PxContactModifyCallback, public physx::PxSimulationEventCallback
	{

		void onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count);

		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);

		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
		void onWake(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
		void onSleep(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override { PX_UNUSED(count); }

	};

}