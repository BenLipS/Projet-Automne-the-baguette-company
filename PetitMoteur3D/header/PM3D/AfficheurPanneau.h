#pragma once
#include "Afficheur2D.h"


using namespace DirectX;

namespace PM3D
{

	class CAfficheurPanneau : public CAfficheur2D
	{
	public:
		CAfficheurPanneau(CDispositifD3D11* pDispositif) : CAfficheur2D(pDispositif) {
			typeTag = "panneau";
		}
		virtual ~CAfficheurPanneau() { CAfficheur2D::~CAfficheur2D(); }
		virtual void Draw();
		void AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position, float _dx = 0.0f, float _dy = 0.0f);

	private:
		// Tous nos sprites
		std::vector<std::unique_ptr<CSprite>> tabSprites;

		class CPanneau : public CSprite
		{
		public:
			XMFLOAT3 position;
			XMFLOAT2 dimension;
			CPanneau()
			{
				bPanneau = true;
			}
		};
	};
} // namespace PM3D