#pragma once
#include "Objet3DDynamic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"
#include "chargeur.h"

#include "PxPhysicsAPI.h"

using namespace DirectX;

namespace PM3D
{
	class CDispositifD3D11;

	class BlocRollerDynamic : public Objet3DDynamic
	{
	public:
		BlocRollerDynamic(Scene* _scene, physx::PxTransform _position, const float _radius,
			CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs);

		// Destructeur
		virtual ~BlocRollerDynamic();

		virtual void Anime(float tempsEcoule) override;

		physx::PxRigidActor* getBody() { return body_; }

		float getVitesseMax() { return vitesseMax_; }
		void setVitesseMax(float _vitesse) { vitesseMax_ = _vitesse; }

		int getNbBonus() { return nbBonus_; }
		void addBonus() {
			if (nbBonus_ < 5) {
				nbBonus_++;
			}
		}
		void suppBonus() {
			if (nbBonus_ > 0) {
				nbBonus_--;
			}
		}
		void resetBonus() {
			nbBonus_=0;
		}

		float getVitesseBonusMax() { return vitesseBonusMax_; }

		bool isContact() { return contact_; }
		bool isTrigger() { return trigger_; }
		void updateContact(bool _contact) { contact_ = _contact; }
		void updateTrigger(bool _trigger) { trigger_ = _trigger; }

	private:

		static physx::PxRigidDynamic* createRigidBody(Scene* _scene, physx::PxTransform _position, const float _radius);

		float radius_;

		float vitesseMax_;
		float vitesseMin_;
		static const float vitesseMaxDefault_;
		static const float vitesseMinDefault_;
		static const float vitesseBonusMax_;

		int nbBonus_{};

		bool contact_ = false;
		bool trigger_ = false;

		float totalTempsEcoule{};

	};

} // namespace PM3D
