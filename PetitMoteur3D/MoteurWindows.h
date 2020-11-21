#pragma once

#include "moteur.h"
#include "dispositifD3D11.h"
#include "Horloge.h"

namespace PM3D
{
#define MAX_LOADSTRING 100

class CMoteurWindows final : public CMoteur<CMoteurWindows, CDispositifD3D11>
{
public:
	void SetWindowsAppInstance(HINSTANCE hInstance) noexcept;

private:
	ATOM MyRegisterClass(HINSTANCE hInstance) noexcept;
	bool InitAppInstance();
	int Show() noexcept;

	// Les fonctions sp�cifiques
	int InitialisationsSpecific() override;
	bool RunSpecific() noexcept override;
	int64_t GetTimeSpecific() const override;
	double GetTimeIntervalsInSec(int64_t start, int64_t stop) const override;
	CDispositifD3D11* CreationDispositifSpecific(const CDS_MODE cdsMode) override;
	void BeginRenderSceneSpecific() override;
	void EndRenderSceneSpecific() noexcept override;

	// Fonctions "Callback" -- Doivent �tre statiques
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM) noexcept;
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM) noexcept;

	HACCEL hAccelTable;						// handle Windows de la table des acc�l�rateurs
	static HINSTANCE hAppInstance;			// handle Windows de l'instance actuelle de l'application
	HWND hMainWnd;							// handle Windows de la fen�tre principale
	TCHAR szWindowClass[MAX_LOADSTRING];	// le nom de la classe de fen�tre principale

	Horloge m_Horloge;
};

} // namespace PM3D
