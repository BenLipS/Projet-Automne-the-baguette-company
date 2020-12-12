#pragma once
#include "Objet3DDynamic.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "tools.h"
#include <queue>

#include "PxPhysicsAPI.h"
using namespace DirectX;

namespace PM3D
{
	class CDispositifD3D11;


	//  Classe : CBloc
	//
	//  BUT : 	Classe de bloc
	//
	class BlocRollerDynamic : public Objet3DDynamic
	{
	public:
		BlocRollerDynamic(Scene* _scene, PxTransform _position, const float _radius,
			CDispositifD3D11* _pDispositif);

		// Destructeur
		virtual ~BlocRollerDynamic();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;


		PxRigidActor* getBody() { return body_; }
		float getVitesseMax() { return vitesseMax_; }
		void setVitesseMax(float _vitesse) { vitesseMax_ = _vitesse; }

		int getNbBonus() { return nbBonus_; }
		void addBonus() {
			if (nbBonus_ < 5) {
				vitesseMax_ += 2000.0f;
				nbBonus_++;
			}
		}
		float getVitesseBonusMax() { return vitesseBonusMax_; }

	private:
		CDispositifD3D11* pDispositif;
		void InitShaders();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		// D�finitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		//float rotation;
		static PxRigidDynamic* createRigidBody(Scene* _scene, PxTransform _position, const float _radius);

		float radius_;
		float vitesseMax_;
		float vitesseMin_;
		static const float vitesseMaxDefault_;
		static const float vitesseMinDefault_;

		static const float vitesseBonusMax_;

		int nbBonus_{};

		bool upPressed_ = false;
		std::queue<float> speedY_buffer;
	};

} // namespace PM3D
