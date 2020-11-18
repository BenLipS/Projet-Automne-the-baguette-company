#pragma once 

namespace PM3D {

	class CInfoDispositif {
	public:

		explicit CInfoDispositif(int NoAdaptateur);
		CInfoDispositif(DXGI_MODE_DESC modeDesc);

		enum INFODISPO_TYPE { 
			ADAPTATEUR_COURANT 
		};

		bool EstValide() const;
		int GetLargeur() const;
		int GetHauteur() const;
		int GetMemoire() const;
		const wchar_t* GetNomCarte() const;
		void GetDesc(DXGI_MODE_DESC& modeDesc);
		
	private:
		bool valide;
		int largeur, hauteur, memoire;
		wchar_t nomcarte[100];
		DXGI_MODE_DESC mode;
	};

}