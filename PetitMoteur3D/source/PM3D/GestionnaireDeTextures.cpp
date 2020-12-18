#include "StdAfx.h"
#include "dispositifD3D11.h"
#include "GestionnaireDeTextures.h"

namespace PM3D
{

CTexture* const CGestionnaireDeTextures::GetNewTexture(const std::wstring& filename, CDispositifD3D11* pDispositif)
{
	// On v�rifie si la texture est d�j� dans notre liste
	CTexture* pTexture = GetTexture(filename);

	// Si non, on la cr�e
	if (!pTexture)
	{
		auto texture = std::make_unique<CTexture>(filename, pDispositif);
		pTexture = texture.get();
		// Puis, il est ajout� � la sc�ne
		ListeTextures.push_back(std::move(texture));
	}

	assert(pTexture);
	return pTexture;
}

CTexture* const CGestionnaireDeTextures::GetTexture(const std::wstring& filename)
{
	CTexture* pTexture = nullptr;

	for (auto& texture : ListeTextures)
	{
		if (texture->GetFilename() == filename)
		{
			pTexture = texture.get();
			break;
		}
	}

	return pTexture;
}

} // namespace PM3D*


