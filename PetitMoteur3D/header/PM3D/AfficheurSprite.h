#pragma once
#include "Afficheur2D.h"

using namespace DirectX;

namespace PM3D
{

	class CAfficheurSprite : public CAfficheur2D
	{

	public:
		CAfficheurSprite(CDispositifD3D11* pDispositif, std::string _typeSprite = "sprite") : CAfficheur2D(pDispositif) {
			typeSprite = _typeSprite;
			typeTag = "sprite";
		}
		virtual ~CAfficheurSprite() {
			CAfficheur2D::~CAfficheur2D();
		}

		virtual void Draw();

		void AjouterSprite(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y, std::string typeTexte);
		void SupprimerSpriteTexte(std::string typeTexte);

	private:
		// Tous nos sprites
		std::vector<std::unique_ptr<CSprite>> tabSprites;

	};
} // namespace PM3D