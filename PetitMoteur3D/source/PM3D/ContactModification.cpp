#include "stdafx.h"
#include "ContactModification.h"
#include "Filter.h"

namespace PM3D {

	void ContactModification::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {

		PX_UNUSED(pairHeader);
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);

	}

	void ContactModification::onContactModify(PxContactModifyPair* const pairs, PxU32 count) {

		for (PxU32 i = 0; i < count; ++i) {

			PxContactModifyPair& pair = pairs[i];

			if (pair.actor[0] != NULL && pair.actor[1] != NULL) {

				auto act0 = pair.shape[0]->getSimulationFilterData().word0;
				auto act1 = pair.shape[1]->getSimulationFilterData().word0;

				PxRigidDynamic* body0 = static_cast<PxRigidDynamic*>(const_cast<PxRigidActor*>(pair.actor[0]));
				PxRigidStatic* body1 = static_cast<PxRigidStatic*>(const_cast<PxRigidActor*>(pair.actor[0]));

				PxVec3 vitesseVehicule = body0->getLinearVelocity();
				/*PxTransform poseTerrain;
				PxVec3 normaleTerrain;
				PxVec3 vitesseVehicule;
				float projete;
				PxVec3 vitesseFinale;*/
				switch (act0 | act1) {

				case FILTER_TYPE::VEHICULE | FILTER_TYPE::OBSTACLE:
					/*body0->addForce(- vitesseVehicule.getNormalized() * 3000.0f, PxForceMode::eIMPULSE);
					//body0->setLinearVelocity(PxVec3{ 0.0f,0.0f,0.0f });
					body0->setLinearDamping(1000);*/
					break;
				case FILTER_TYPE::VEHICULE | FILTER_TYPE::TERRAIN:
					/*poseTerrain = body1->getGlobalPose();
					normaleTerrain = poseTerrain.q.getBasisVector1();
					vitesseVehicule = body0->getLinearVelocity();
					projete = normaleTerrain.dot(vitesseVehicule);
					vitesseFinale = vitesseVehicule - (projete * normaleTerrain * 0.5);
					body0->setLinearVelocity(vitesseFinale);*/
					break;
				case FILTER_TYPE::VEHICULE | FILTER_TYPE::MUR:
					body0->setLinearDamping(1000);
					break;
				default:
					/*for (PxU32 j = 0; j < pair.contacts.size(); j++) {
						pair.contacts.ignore(j);
					}*/
					break;

				}

			}

		}

	}

}