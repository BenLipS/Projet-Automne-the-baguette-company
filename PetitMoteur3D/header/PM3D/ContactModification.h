#pragma once
#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

namespace PM3D {

	class ContactModification : public PxContactModifyCallback, public PxSimulationEventCallback
	{
	public:

		void onContactModify(PxContactModifyPair* const pairs, PxU32 count);

		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
		void onWake(PxActor** actors, PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
		void onSleep(PxActor** actors, PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
		void onTrigger(PxTriggerPair* pairs, PxU32 count);
		void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override { PX_UNUSED(count); }

	};

}