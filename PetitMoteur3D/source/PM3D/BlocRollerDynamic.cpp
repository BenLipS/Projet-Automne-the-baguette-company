#include "stdafx.h"
#include "BlocRollerDynamic.h"
#include "util.h"
#include "DispositifD3D11.h"
#include <math.h>
#include "MoteurWindows.h"
#include "tools.h"
#include "Filter.h"

using namespace physx;
using namespace DirectX;

namespace PM3D
{

	constexpr float BlocRollerDynamic::vitesseMaxDefault_ = 800.0f;
	constexpr float BlocRollerDynamic::vitesseMinDefault_ = 10.0f;
	constexpr float BlocRollerDynamic::vitesseBonusMax_ = 1800.0f;

	BlocRollerDynamic::BlocRollerDynamic(Scene* _scene, PxTransform _position, const float _radius,
		CDispositifD3D11* _pDispositif, const std::vector<IChargeur*> chargeurs) : Objet3DDynamic(_scene, createRigidBody(_scene, _position, _radius), _pDispositif, chargeurs)
		, radius_(_radius)
		, vitesseMax_(vitesseMaxDefault_)
		, vitesseMin_(vitesseMinDefault_)

	{
		typeTag = "vehicule";

		// Filtre pour les collisions
		setupFiltering(body_, FILTER_TYPE::VEHICULE, FILTER_TYPE::OBSTACLE | FILTER_TYPE::MUR);
	}

	void BlocRollerDynamic::Anime(float tempsEcoule)
	{
		float constexpr vitesseBonus = 200.f;
		setVitesseMax(nbBonus_ * vitesseBonus + vitesseMaxDefault_);
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		auto body = static_cast<PxRigidDynamic*>(body_);
		auto speed = body->getLinearVelocity();

		PlanStatic* terrain = CMoteurWindows::GetInstance().getTerrain(getPosition());
		PxVec3 const normale = terrain->getNormale();

		if (body->getGlobalPose().p.z > 30000.f) {
			PxVec3 vitesseFinale = speed * 0.9f;
			if (vitesseFinale.magnitude() < 10.0f) {
				vitesseFinale = PxZero;
			}
			body->setLinearVelocity(vitesseFinale);

			PxVec3 const direction = vitesseFinale.getNormalized();
			PxVec3 const sens = PxVec3(0.0f, 0.0f, 1.0f);
			PxVec3 const projete = PxVec3(direction.x, 0.0f, direction.z).getNormalized();
			float angle = acos(projete.dot(sens));

			PxVec3 const directionPente = terrain->getDirection();
			if ((direction.cross(directionPente).y > 0)) {
				angle = -angle;
			}


			PxVec3 const largeurPente = normale.cross(directionPente);
			PxQuat const pente = PxQuat(acos(normale.dot(PxVec3(0.0f, 1.0f, 0.0f))), largeurPente);
			PxQuat const orientation = PxQuat(angle, normale).getNormalized();

			matWorld = XMMatrixRotationQuaternion(XMVectorSet(pente.x, pente.y, pente.z, pente.w)); //Orientation
			matWorld *= XMMatrixRotationQuaternion(XMVectorSet(orientation.x, orientation.y, orientation.z, orientation.w)); //Orientation
		}
		else {

			if (isContact()) {
				totalTempsEcoule += tempsEcoule;
				if (totalTempsEcoule > .75f) {
					updateContact(false);
					totalTempsEcoule = 0.f;
				}
			}

			PxVec3 const gauche = (-normale).cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,1,0)
			PxVec3 const droite = normale.cross(speed.getNormalized()); //produit vectoriel(speed.norme * 0,-1,0)

			PxVec3 vVitesse = speed;

			// V�rifier l��tat de la touche gauche
			float constexpr coeffMoveCote = 50;
			if (!isContact()) {
				if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT)) {
					auto const direction = gauche * speed.magnitude();
					vVitesse += (direction.getNormalized() * (speed.magnitude() / coeffMoveCote));
				}
				// V�rifier l��tat de la touche droite
				if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT)) {
					auto const direction = droite * speed.magnitude();
					vVitesse += (direction.getNormalized() * (speed.magnitude() / coeffMoveCote));
				}

				vVitesse = vVitesse.getNormalized() * speed.magnitude();

				if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_UP)) {
					vVitesse = vVitesse * 1.02f;
				}

				if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN)) {
					vVitesse = vVitesse * 0.99f;
				}
			}

			PxVec3 const direction = speed.getNormalized();
			PxVec3 const sens = PxVec3(0.0f, 0.0f, 1.0f);
			PxVec3 const projete = PxVec3(direction.x, 0.0f, direction.z).getNormalized();
			float angle = acos(projete.dot(sens));

			PxVec3 const directionPente = terrain->getDirection();
			if ((direction.cross(directionPente).y > 0)) {
				angle = -angle;
			}


			PxVec3 const largeurPente = normale.cross(directionPente);
			PxQuat const pente = PxQuat(acos(normale.dot(PxVec3(0.0f, 1.0f, 0.0f))), largeurPente);
			PxQuat const orientation = PxQuat(angle, normale).getNormalized();

			float const valProjete = normale.dot(vVitesse);
			PxVec3 vitesseFinale{ vVitesse - (valProjete * normale) };
			float const ValProjPente = directionPente.dot(vitesseFinale);

			if (ValProjPente >= 0) {
				if (vitesseFinale.magnitude() > vitesseMax_)
					body->setLinearVelocity(vitesseFinale.getNormalized() * vitesseMax_);
				else if (vitesseFinale.magnitude() < vitesseMin_)
					body->setLinearVelocity(vitesseFinale.getNormalized() * vitesseMin_);
				else
					body->setLinearVelocity(vitesseFinale);
			}
			else {
				float const coeffRemontePente = 0.9f;
				if (abs(ValProjPente) > 200.f) {//Pour pas remonter la pente
					body->setLinearVelocity(vitesseFinale * coeffRemontePente);
				}
				else
					body->setLinearVelocity(vitesseFinale);
			}

			if (!isContact()) {
				matWorld = XMMatrixRotationQuaternion(XMVectorSet(pente.x, pente.y, pente.z, pente.w)); //Orientation
				matWorld *= XMMatrixRotationQuaternion(XMVectorSet(orientation.x, orientation.y, orientation.z, orientation.w)); //Orientation
			}
			else {
				matWorld = XMMatrixRotationQuaternion(XMVectorSet(body_->getGlobalPose().q.x, body_->getGlobalPose().q.y, body_->getGlobalPose().q.z, body_->getGlobalPose().q.w)); //Orientation
				if (abs(totalTempsEcoule - tempsEcoule) < 0.0001f) {
					body->setLinearVelocity(vitesseFinale * 0.5f);
					suppBonus();
				}
			}

		}

		matWorld *= XMMatrixTranslationFromVector(XMVectorSet(body_->getGlobalPose().p.x, body_->getGlobalPose().p.y, body_->getGlobalPose().p.z, 1)); //Position

	}

	BlocRollerDynamic::~BlocRollerDynamic()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	PxRigidDynamic* BlocRollerDynamic::createRigidBody(Scene* _scene, PxTransform _position, const float _radius)
	{
		PxRigidDynamic* bodyDynamic = PxCreateDynamic(*(_scene->physic_), _position, PxSphereGeometry(_radius), *(_scene->material_), 10.0f);
		return bodyDynamic;
	}

} // namespace PM3D
