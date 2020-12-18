#include "stdafx.h"
#include "ContactModification.h"
#include "Filter.h"
#include "MoteurWindows.h"

using namespace physx;

namespace PM3D {

	void ContactModification::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {

		PX_UNUSED(pairHeader);
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);

	}

	void ContactModification::onTrigger(PxTriggerPair* pairs, PxU32 count) {

		for (PxU32 i = 0; i < count; ++i) {

			PxTriggerPair& pair = pairs[i];

			if (pair.triggerActor != NULL && pair.otherActor != NULL) {

				auto bodyTrigger = pair.triggerActor;
				bool erased = CMoteurWindows::GetInstance().eraseBody(bodyTrigger);
				auto bodyVehicule = static_cast<PxRigidDynamic*>(pair.otherActor);
				BlocRollerDynamic* vehicule = CMoteurWindows::GetInstance().findVehiculeFromBody(bodyVehicule);
				if (vehicule != nullptr && erased) {
					vehicule->addBonus();
				}

			}

		}

	}

	void ContactModification::onContactModify(PxContactModifyPair* const pairs, PxU32 count) {

		for (PxU32 i = 0; i < count; ++i) {

			PxContactModifyPair& pair = pairs[i];

			if (pair.actor[0] != NULL && pair.actor[1] != NULL) {

				auto act0 = pair.shape[0]->getSimulationFilterData().word0;
				auto act1 = pair.shape[1]->getSimulationFilterData().word0;

				PxRigidDynamic* body0 = static_cast<PxRigidDynamic*>(const_cast<PxRigidActor*>(pair.actor[0]));
				PxRigidStatic* body1 = static_cast<PxRigidStatic*>(const_cast<PxRigidActor*>(pair.actor[0]));

				BlocRollerDynamic* vehicule;
				switch (act0 | act1) {

				case FILTER_TYPE::VEHICULE | FILTER_TYPE::OBSTACLE:
					vehicule = CMoteurWindows::GetInstance().findVehiculeFromBody(body0);
					vehicule->updateContact(true);
					CMoteurWindows::GetInstance().updatePanneauCollision();
					break;
				case FILTER_TYPE::VEHICULE | FILTER_TYPE::TERRAIN:
					break;
				case FILTER_TYPE::VEHICULE | FILTER_TYPE::MUR:
					vehicule = CMoteurWindows::GetInstance().findVehiculeFromBody(body0);
					vehicule->updateContact(true);
					CMoteurWindows::GetInstance().updatePanneauCollision();
					break;
				default:
					break;

				}

			}

		}

	}

}